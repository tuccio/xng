#include <xng/dx11/gpu_profiler.hpp>
#include <xng/graphics/api_context.hpp>
#include <xng/os/timestamp.hpp>

using namespace xng::dx11;
using namespace xng::graphics;

// gpu_profile_data

UINT64 gpu_profile_data::get_time_delta(ID3D11DeviceContext * context) const
{
    UINT64 begin, end;

    if (XNG_HR_FAILED(context->GetData(m_begin.get(), &begin, sizeof(UINT64), 0)) ||
        XNG_HR_FAILED(context->GetData(m_end.get(), &end, sizeof(UINT64), 0)))
    {
        return 0;
    }
    else
    {
        return end - begin;
    }
}

bool gpu_profile_data::init(ID3D11Device * device)
{
    if (!XNG_HR_FAILED(device->CreateQuery(&CD3D11_QUERY_DESC(D3D11_QUERY_TIMESTAMP), m_begin.reset_and_get_address())) &&
        !XNG_HR_FAILED(device->CreateQuery(&CD3D11_QUERY_DESC(D3D11_QUERY_TIMESTAMP), m_end.reset_and_get_address())))
    {
        return true;
    }

    return false;
}

void gpu_profile_data::begin(ID3D11DeviceContext * context)
{
    context->End(m_begin.get());
}

void gpu_profile_data::end(ID3D11DeviceContext * context)
{
    context->End(m_end.get());
}

// gpu_profile_buffer

bool gpu_profile_buffer::init(ID3D11Device * device, uint32_t buffers, uint32_t samples)
{
    m_used = 0;
    m_next = 0;

    m_samplesSum = 0.f;
    m_numSamples = samples;

    m_data.resize(buffers);

    for (auto & t : m_data)
    {
        if (!t.init(device))
        {
            clear();
            return false;
        }
    }

    return true;
}

void gpu_profile_buffer::clear(void)
{
    m_data.clear();
    m_data.shrink_to_fit();
}

void gpu_profile_buffer::reset(void)
{
    m_started = false;
}

bool gpu_profile_buffer::was_started(void) const
{
    return m_started;
}

bool gpu_profile_buffer::is_ready(void) const
{
    return m_used == m_data.size();
}

UINT64 gpu_profile_buffer::get_time_delta(ID3D11DeviceContext * context) const
{
    return m_data[m_next].get_time_delta(context);
}

void gpu_profile_buffer::update_moving_average(ID3D11DeviceContext * context, float invFrequency)
{
    if (m_samples.size() == m_numSamples)
    {
        m_samplesSum -= m_samples.back();
        m_samples.pop_back();
    }

    float t = (get_time_delta(context) * 1000) * invFrequency;

    m_samplesSum += t;
    m_samples.push_front(t);
}

float gpu_profile_buffer::get_moving_average(void) const
{
    return m_samplesSum / m_samples.size();
}

void gpu_profile_buffer::begin(ID3D11DeviceContext * context)
{
    m_started = true;
    m_data[m_next].begin(context);
}

void gpu_profile_buffer::end(ID3D11DeviceContext * context)
{
    m_data[m_next].end(context);
}

void gpu_profile_buffer::frame_complete(void)
{
    m_used = std::min(m_data.size(), m_used + 1);
    m_next = (m_next + 1) % m_data.size();
}

// gpu_profiler

bool gpu_profiler::init(ID3D11Device * device, uint32_t buffers)
{
    m_device  = device;
    m_buffers = buffers;

    m_used = 0;
    m_next = 0;

    m_disjointQueries.resize(buffers);

    for (auto & q : m_disjointQueries)
    {
        if (XNG_HR_FAILED(device->CreateQuery(&CD3D11_QUERY_DESC(D3D11_QUERY_TIMESTAMP_DISJOINT), q.reset_and_get_address())))
        {
            clear();
            return false;
        }
    }

    return true;
}

void gpu_profiler::clear(void)
{
    m_queries.clear();

    m_disjointQueries.clear();
    m_disjointQueries.shrink_to_fit();
}

void gpu_profiler::profile_start(const char * str, ID3D11DeviceContext * context)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_queries.find(str);

    if (it == m_queries.end())
    {
        it = m_queries.emplace(str, gpu_profile_buffer()).first;
        it->second.init(m_device.get(), m_buffers, 10);
    }

    it->second.begin(context);
}

void gpu_profiler::profile_complete(const char * str, ID3D11DeviceContext * context)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_queries[str].end(context);
}

void gpu_profiler::frame_start(ID3D11DeviceContext * context)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    context->Begin(m_disjointQueries[m_next].get());
}

void gpu_profiler::frame_complete(ID3D11DeviceContext * context)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto & p : m_queries)
    {
        p.second.frame_complete();
    }

    context->End(m_disjointQueries[m_next].get());

    m_used = std::min(m_buffers, m_used + 1);
    m_next = (m_next + 1) % m_buffers;
}

profiler_data gpu_profiler::collect_data(ID3D11DeviceContext * context)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    profiler_data data;

    if (m_used == m_buffers)
    {
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;

        high_resolution_timestamp begin = os::timestamp();
        
        while (context->GetData(m_disjointQueries[m_next].get(), &tsDisjoint, sizeof(tsDisjoint), 0) == S_FALSE);

        if (!tsDisjoint.Disjoint)
        {
            float invFreq = 1.f / tsDisjoint.Frequency;
            
            for (auto & p : m_queries)
            {
                if (p.second.was_started() && p.second.is_ready())
                {
                    p.second.update_moving_average(context, invFreq);
                    float dt = p.second.get_moving_average();
                    data.push_back(profiler_entry{ p.first, dt });
                    p.second.reset();
                }
            }
        }        

        high_resolution_timestamp end = os::timestamp();

        data.push_back(profiler_entry{ "Profiling Queries", to_milliseconds<float>(end - begin) });
    }

    return data;
}
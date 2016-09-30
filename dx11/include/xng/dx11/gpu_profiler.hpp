#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/graphics/api_context.hpp>

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <deque>

namespace xng
{
	namespace dx11
	{
		class gpu_profile_data
		{

		public:

			gpu_profile_data(void) = default;
			gpu_profile_data(const gpu_profile_data &) = default;
			gpu_profile_data(gpu_profile_data &&) = default;

			bool init(ID3D11Device * device);

			void begin(ID3D11DeviceContext * context);
			void end(ID3D11DeviceContext * context);

			UINT64 get_time_delta(ID3D11DeviceContext * context) const;

		private:

			com_ptr<ID3D11Query> m_begin;
			com_ptr<ID3D11Query> m_end;

		};

		class gpu_profile_buffer
		{

		public:

			bool init(ID3D11Device * device, uint32_t buffers, uint32_t samples);
			void clear(void);

			bool is_ready(void) const;

			UINT64 get_time_delta(ID3D11DeviceContext * context) const;

			void update_moving_average(ID3D11DeviceContext * context, float invFrequency);
			float get_moving_average(void) const;

			void frame_complete(void);

			void begin(ID3D11DeviceContext * context);
			void end(ID3D11DeviceContext * context);

		private:

			std::vector<gpu_profile_data> m_data;
			std::deque<float>             m_samples;

			uint32_t m_used;
			uint32_t m_next;

			float    m_samplesSum;
			uint32_t m_numSamples;

		};

		class gpu_profiler
		{

		public:

			bool init(ID3D11Device * device, uint32_t buffers);
			void clear(void);

			void profile_start(const char * str, ID3D11DeviceContext * context);
			void profile_complete(const char * str, ID3D11DeviceContext * context);

			void frame_start(ID3D11DeviceContext * context);
			void frame_complete(ID3D11DeviceContext * context);

			graphics::profiler_data collect_data(ID3D11DeviceContext * context);
			
		private:

			uint32_t m_buffers;
			uint32_t m_used;
			uint32_t m_next;

			std::unordered_map<std::string, gpu_profile_buffer> m_queries;

			std::vector<com_ptr<ID3D11Query>> m_disjointQueries;
			com_ptr<ID3D11Device>             m_device;

			mutable std::mutex m_mutex;

		};

	}
}
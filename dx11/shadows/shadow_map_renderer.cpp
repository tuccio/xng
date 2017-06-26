#include <xng/dx11/shadows/shadow_map_renderer.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::math;

bool shadow_map_renderer::init(dx11_api_context * context, const render_variables & rvars)
{
    m_context = context;
    m_rvars   = rvars;
    return m_shadowMapping.init(context);
}

void shadow_map_renderer::shutdown(void)
{
    m_shadowMapping.shutdown();
}

void shadow_map_renderer::update_render_variables(const render_variables & rvars, const render_variables_updates & updates)
{
    m_rvars = rvars;
}

namespace
{
    inline std::vector<float4x4> compute_directional_light_matrices(const extracted_scene & scene, const extracted_light & light)
    {
        const aabb & aabb = scene.get_aabb();

        camera lightCamera;
        lightCamera.look_at(float3(0, 0, 0), float3(0, 1, 0), light.direction);

        const float4x4 & view = lightCamera.get_directx_view_matrix();
        
        auto corners = aabb.get_corners();

        float3 m(std::numeric_limits<float>::max()), M(-std::numeric_limits<float>::max());

        for (auto corner : corners)
        {
            auto transformed = to_float3(view * float4(corner, 1));
            m = min(m, transformed);
            M = max(M, transformed);
        }

        float4x4 proj = ortho_lh_directx(m.x, M.x, m.y, M.y, m.z, M.z);
        float4x4 projView = proj * view;

        return std::vector<float4x4> { projView };
    }

    inline std::vector<float4x4> compute_light_matrices(const extracted_scene & scene, const extracted_light & light)
    {
        switch (light.type)
        {
        case XNG_LIGHT_DIRECTIONAL:
            return compute_directional_light_matrices(scene, light);
        default:
            return std::vector<float4x4>{};
        }
    }
}

void shadow_map_renderer::render(ID3D11DeviceContext * deviceContext,
                                 std::unordered_map<int, std::vector<shadow_map>> & shadowMaps,
                                 render_resource_manager & renderResourceManager,
                                 const extracted_scene & scene)
{
    m_context->profile_start("Shadow Maps", deviceContext);

    auto lights = scene.get_lights();

    std::vector<int> castingLights;

    std::unordered_map<xng_light_type, int> lightScores = {
        { XNG_LIGHT_DIRECTIONAL, 0 }
    };

    // Extract the casting lights (also according to the supported casting types)

    for (int i = 0; i < lights.size(); ++i)
    {
        const auto & light = lights[i];

        if (lightScores.find(light.type) != lightScores.end() &&
            light.shadowCasting)
        {
            castingLights.emplace_back(i);
        }
    }

    // TODO: Sort the lights by influence and pick the most influencing subset

    // Sort the lights by type for rendering

    std::sort(castingLights.begin(),
              castingLights.end(),
              [&](int lhs, int rhs)
    {
        return lightScores[lights[lhs].type] < lightScores[lights[rhs].type];
    });

    // Render

    for (int lightIndex : castingLights)
    {
        const auto & light = lights[lightIndex];

        auto lightMatrices = compute_light_matrices(scene, light);
        std::vector<shadow_map> sm(lightMatrices.size());

        for (int i = 0; i < lightMatrices.size(); ++i)
        {
            sm[i].matrix    = lightMatrices[i];
            sm[i].shadowMap = renderResourceManager.get_texture_2d(m_context->get_device(),
                                                                   CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R24G8_TYPELESS,
                                                                                         m_rvars.shadow_map_resolution,
                                                                                         m_rvars.shadow_map_resolution,
                                                                                         1,
                                                                                         0,
                                                                                         D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE),
                                                                   nullptr,
                                                                   &CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS),
                                                                   nullptr,
                                                                   &CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT));

            m_shadowMapping.render(deviceContext,
                                   sm[i].shadowMap->get_render_resource()->get_depth_stencil_view(),
                                   sm[i].matrix,
                                   scene,
                                   uint2(m_rvars.shadow_map_resolution),
                                   uint2(0),
                                   m_rvars);

        }

        shadowMaps.emplace(lightIndex, std::move(sm));
    }

    m_context->profile_complete("Shadow Maps", deviceContext);
}

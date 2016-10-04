#include <xng/dx11/forward_renderer.hpp>

#include <xng/dx11.hpp>
#include <xng/graphics/mesh.hpp>
#include <xng/dx11/gpu_mesh.hpp>
#include <xng/dx11/texture.hpp>

#include <algorithm>
#include <iterator>

#include <boost/range/join.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

#define NORMAL_MAP   1
#define BASE_TEXTURE 2
#define SPECULAR_MAP 4

struct alignas(16) __cbPerObject
{
	float4x4 modelMatrix;
	float4x4 modelViewMatrix;
	float4x4 modelViewProjectionMatrix;
};

struct alignas(16) __cbPerFrame
{
	unsigned int numLights;
};

struct __Light
{
	float3 luminance;
	int    type;
	float3 ambient;
	float  __fill0;
	float3 position;
	float  __fill1;
	float3 direction;
	float  cutoff;
};

bool forward_renderer::init(dx11_api_context * context, const render_variables & rvars)
{
	m_apiContext = context;

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);

	rasterizerDesc.FrontCounterClockwise = TRUE;

	CD3D11_DEPTH_STENCIL_DESC depthCompareDesc(D3D11_DEFAULT);

	depthCompareDesc.DepthFunc      = D3D11_COMPARISON_EQUAL;
	depthCompareDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	D3D11_BLEND_DESC blendNoRTDesc = {};

	switch (rvars.culling_mode)
	{
	case XNG_CULLING_MODE_NONE:
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		break;
	default:
	case XNG_CULLING_MODE_FRONT:
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		break;
	case XNG_CULLING_MODE_BACK:
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		break;
	}

	ID3D11Device * device = m_apiContext ? m_apiContext->get_device() : nullptr;
	
	m_program.set_ilv_functor([](D3D11_INPUT_ELEMENT_DESC * inputElementDesc)
	{
		inputElementDesc->InputSlot = strcmp(inputElementDesc->SemanticName, "POSITION") ? 1 : 0;
	});

	m_maxLights = 0;

	return device != nullptr &&
		m_program.preprocess(XNG_DX11_SHADER_FILE("forward.xhlsl")) &&
		m_cbPerFrame.create<__cbPerFrame>(device) &&
		m_cbPerObject.create<__cbPerObject>(device) &&
		m_cbPerObjectDepth.create<float4x4>(device) &&
		!XNG_HR_FAILED(device->CreateBlendState(&blendNoRTDesc, &m_blendNoRTState)) &&
		!XNG_HR_FAILED(device->CreateDepthStencilState(&depthCompareDesc, &m_depthCompareState)) &&
		!XNG_HR_FAILED(device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState));
}

void forward_renderer::shutdown(void)
{
	m_apiContext = nullptr;
	m_rasterizerState.reset();
	m_cbPerObject.reset();
	m_program.clear();
	m_vbFactory.clear();
}

void make_shader_permutation(uint32_t & shaderPermutationKey, const material_ptr material)
{
	shaderPermutationKey = 0;

	if (material->get_normal_map())
	{
		shaderPermutationKey |= NORMAL_MAP;
	}

	if (material->get_base_texture())
	{
		shaderPermutationKey |= BASE_TEXTURE;
	}

	if (material->get_specular_map())
	{
		shaderPermutationKey |= SPECULAR_MAP;
	}
}

void make_shader_permutation_macros(std::vector<shader_macro> & macros, uint32_t shaderPermutationKey)
{
	macros.clear();

	if (shaderPermutationKey & NORMAL_MAP)
	{
		macros.push_back(shader_macro{ "NORMAL_MAP", "" });
	}

	if (shaderPermutationKey & BASE_TEXTURE)
	{
		macros.push_back(shader_macro{ "BASE_TEXTURE", "" });
	}

	if (shaderPermutationKey & SPECULAR_MAP)
	{
		macros.push_back(shader_macro{ "SPECULAR_MAP", "" });
	}
}

void forward_renderer::depth_prepass(ID3D11DeviceContext * deviceContext, ID3D11DepthStencilView * dsv, const extracted_scene & scene)
{
	ID3D11Device * device = m_apiContext->get_device();

	m_apiContext->profile_start("Depth Prepass");

	deviceContext->RSSetViewports(1, &CD3D11_VIEWPORT(0.f, 0.f, m_rvars.render_resolution.x, m_rvars.render_resolution.y));
	deviceContext->RSSetState(m_rasterizerState.get());

	deviceContext->OMSetBlendState(m_blendNoRTState.get(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, dsv);

	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);

	const camera * camera = scene.get_active_camera();

	const float4x4 & viewMatrix       = camera->get_directx_view_matrix();
	const float4x4 & projectionMatrix = camera->get_directx_projection_matrix();

	float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	auto & dynamicGeometry = scene.get_frustum_culling_dynamic();
	auto & staticGeometry  = scene.get_frustum_culling_static();

	deviceContext->VSSetConstantBuffers(1, 1, &m_cbPerObjectDepth);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gpu_mesh::load_data meshLoadData = { m_apiContext->get_device() };

	shader_program program = m_program.compile(device, "depth_prepass", { { "DEPTH_PREPASS", "" } });
	program.use(deviceContext);

	deviceContext->PSSetShader(nullptr, nullptr, 0);

	auto & renderables = scene.get_renderables();

	for (auto renderableIndex : boost::range::join(staticGeometry, dynamicGeometry))
	{
		auto & renderable = renderables[renderableIndex];

		gpu_mesh_ptr mesh = resource_factory::get_singleton()->create<gpu_mesh>("", resource_parameters(), resource_loader_ptr(), renderable.mesh);

		if (mesh && mesh->load(&meshLoadData))
		{
			// Fill constant buffer

			const float4x4 & modelMatrix = renderable.world;
			m_cbPerObjectDepth.write(deviceContext, &(viewProjectionMatrix * modelMatrix));

			// Draw

			UINT strides[] = { mesh->get_positional_data_stride() };
			UINT offsets[] = { mesh->get_positional_data_offset() };

			ID3D11Buffer * buffers[] = { mesh->get_positional_data() };

			deviceContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
			deviceContext->IASetIndexBuffer(mesh->get_indices_data(), mesh->get_indices_format(), mesh->get_indices_offset());

			deviceContext->DrawIndexed(mesh->get_num_indices(), 0, 0);
		}
	}

	m_apiContext->profile_complete("Depth Prepass");

	program.dispose(deviceContext);
}

void forward_renderer::render(
	ID3D11DeviceContext * deviceContext,
	ID3D11RenderTargetView * rtv,
	ID3D11DepthStencilView * dsv,
	bool renderDepth,
	const extracted_scene & scene)
{
	ID3D11Device * device = m_apiContext->get_device();

	m_apiContext->profile_start("Forward Pass");

	deviceContext->RSSetViewports(1, &CD3D11_VIEWPORT(0.f, 0.f, m_rvars.render_resolution.x, m_rvars.render_resolution.y));
	deviceContext->RSSetState(m_rasterizerState.get());

	if (renderDepth)
	{
		deviceContext->OMSetDepthStencilState(nullptr, 0);
		deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);
	}
	else
	{
		deviceContext->OMSetDepthStencilState(m_depthCompareState.get(), 0);
	}

	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	deviceContext->OMSetRenderTargets(1, &rtv, dsv);

	float clearColor[4] = { 0 };

	deviceContext->ClearRenderTargetView(rtv, clearColor);	

	const camera * camera = scene.get_active_camera();

	const float4x4 & viewMatrix       = camera->get_directx_view_matrix();
	const float4x4 & projectionMatrix = camera->get_directx_projection_matrix();

	float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	auto & dynamicGeometry = scene.get_frustum_culling_dynamic();
	auto & staticGeometry  = scene.get_frustum_culling_static();

	__cbPerFrame cbPerFrameData = {};

	cbPerFrameData.numLights = scene.get_lights().size();

	m_cbPerFrame.write(deviceContext, &cbPerFrameData);

	deviceContext->VSSetConstantBuffers(0, 2, &std::array<ID3D11Buffer*, 2>{ m_cbPerFrame.get(), m_cbPerObject.get() }[0]);
	deviceContext->PSSetConstantBuffers(0, 1, &m_cbPerFrame);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gpu_mesh::load_data meshLoadData = { m_apiContext->get_device() };

	// Sort objects by shader/material

	std::vector<shader_macro> shaderMacros;
	uint32_t shaderPermutationKey;

	struct geometry_key_index
	{
		uint64_t key;
		uint32_t index;
	};

	std::vector<geometry_key_index> geometry;

	geometry.reserve(dynamicGeometry.size() + staticGeometry.size());

	auto & renderables = scene.get_renderables();

	auto computeSortKey = [&](uint32_t geometryIndex)
	{
		uint32_t shaderProgram;

		const material_ptr & material = renderables[geometryIndex].material;

		uint64_t key = 0;

		if (material->load())
		{
			make_shader_permutation(shaderProgram, material);

			// TODO: Should use some hashing function

			uint64_t shaderProgram64 = shaderProgram;
			uint64_t baseTexture64   = material->get_base_texture() ? material->get_base_texture()->get_id() : 0;
			uint64_t normalMap64     = material->get_normal_map() ? material->get_normal_map()->get_id() : 0;
			uint64_t specularMap64   = material->get_specular_map() ? material->get_specular_map()->get_id() : 0;

			key =
				((shaderProgram64 & 0xFFFF) << 48) |
				((baseTexture64 & 0xFFFF) << 32) |
				((normalMap64 & 0xFFFF) << 16) |
				((specularMap64 & 0xFFFF));
		}

		return geometry_key_index{ key, geometryIndex };		
	};

	std::transform(dynamicGeometry.begin(), dynamicGeometry.end(), std::back_inserter(geometry), computeSortKey);
	std::transform(staticGeometry.begin(), staticGeometry.end(), std::back_inserter(geometry), computeSortKey);

	std::sort(geometry.begin(), geometry.end(),
		[](const geometry_key_index & lhs, const geometry_key_index & rhs)
	{
		return lhs.key < rhs.key;
	});


	// Setup lights

	auto & extractedLights = scene.get_lights();;
	uint32_t numLights = extractedLights.size();

	if (numLights > m_maxLights)
	{
		CD3D11_BUFFER_DESC bufferDesc(
			sizeof(__Light) * numLights,
			D3D11_BIND_SHADER_RESOURCE,
			D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE,
			D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
			sizeof(__Light));

		if (!XNG_HR_FAILED(device->CreateBuffer(&bufferDesc, nullptr, m_sbLights.reset_and_get_address())) &&
		    !XNG_HR_FAILED(device->CreateShaderResourceView(m_sbLights.get(), nullptr, m_srvLights.reset_and_get_address())))
		{
			m_maxLights = numLights;
		}
	}

	if (numLights > 0)
	{
		std::vector<__Light> lights(numLights);

		for (uint32_t i = 0; i < numLights; ++i)
		{
			auto & xLight = extractedLights[i];

			lights[i].ambient   = xLight.ambient;
			lights[i].type      = xLight.type;
			lights[i].direction = xLight.direction;
			lights[i].position  = xLight.position;
			lights[i].cutoff    = xLight.cutoff;
			lights[i].luminance = xLight.luminance;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (!XNG_HR_FAILED(deviceContext->Map(m_sbLights.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, &lights[0], sizeof(__Light) * numLights);
			deviceContext->Unmap(m_sbLights.get(), 0);
		}

		deviceContext->PSSetShaderResources(8, 1, &m_srvLights);
	}

	// Render

	uint32_t currentShader = 0xFFFFFFFF;

	for (auto renderableKeyIndex : geometry)
	{
		auto & renderable = renderables[renderableKeyIndex.index];

		material_ptr material = renderable.material;
		gpu_mesh_ptr mesh = resource_factory::get_singleton()->create<gpu_mesh>("", resource_parameters(), resource_loader_ptr(), renderable.mesh);

		if (material && material->load() &&
		    mesh && mesh->load(&meshLoadData))
		{
			make_shader_permutation(shaderPermutationKey, material);
			
			if (currentShader != shaderPermutationKey)
			{
				make_shader_permutation_macros(shaderMacros, shaderPermutationKey);

				shader_program program = m_program.compile(m_apiContext->get_device(), std::to_string(shaderPermutationKey).c_str(), shaderMacros);
				program.use(deviceContext);

				currentShader = shaderPermutationKey;
			}

			// Fill constant buffer

			const float4x4 & modelMatrix = renderable.world;

			__cbPerObject bufferPerObjectData;
			
			bufferPerObjectData.modelMatrix               = modelMatrix;
			bufferPerObjectData.modelViewMatrix           = viewMatrix * modelMatrix;
			bufferPerObjectData.modelViewProjectionMatrix = viewProjectionMatrix * modelMatrix;

			m_cbPerObject.write(deviceContext, &bufferPerObjectData);

			// Bind textures

			ID3D11ShaderResourceView * srvs[3] = { 0 };

			if (shaderPermutationKey & NORMAL_MAP)
			{
				texture_ptr normalMap = resource_factory::get_singleton()->create<texture>(
					"",
					resource_parameters(),
					resource_loader_ptr(),
					material->get_normal_map());
				
				if (normalMap && normalMap->load(&texture::load_data{ device, deviceContext }))
				{
					srvs[0] = normalMap->get_shader_resource_view();
				}
			}

			if (shaderPermutationKey & BASE_TEXTURE)
			{
				static const resource_parameters texParams = make_resource_parameters("generate_mipmaps", true);

				texture_ptr baseTexture = resource_factory::get_singleton()->create<texture>(
					"",
					texParams,
					resource_loader_ptr(),
					material->get_base_texture());

				if (baseTexture && baseTexture->load(&texture::load_data{ device, deviceContext }))
				{
					srvs[1] = baseTexture->get_shader_resource_view();
				}
			}

			if (shaderPermutationKey & SPECULAR_MAP)
			{
				texture_ptr specularMap = resource_factory::get_singleton()->create<texture>(
					"",
					resource_parameters(),
					resource_loader_ptr(),
					material->get_specular_map());

				if (specularMap && specularMap->load(&texture::load_data{ device, deviceContext }))
				{
					srvs[2] = specularMap->get_shader_resource_view();
				}
			}

			deviceContext->PSSetShaderResources(0, 3, srvs);

			// Draw

			UINT strides[] = { mesh->get_positional_data_stride(), mesh->get_non_positional_data_stride() };
			UINT offsets[] = { mesh->get_positional_data_offset(), mesh->get_non_positional_data_offset() };

			ID3D11Buffer * buffers[] = { mesh->get_positional_data(), mesh->get_non_positional_data() };

			deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
			deviceContext->IASetIndexBuffer(mesh->get_indices_data(), mesh->get_indices_format(), mesh->get_indices_offset());

			deviceContext->DrawIndexed(mesh->get_num_indices(), 0, 0);
		}
	}

	m_apiContext->profile_complete("Forward Pass");

	shader_program clearState;
	clearState.dispose(deviceContext);
}

void forward_renderer::update_render_variables(const render_variables & rvars, const render_variables_updates & updates)
{
	m_rvars = rvars;

	if (updates.find(XNG_RV_CULLING_MODE) != updates.end())
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		m_rasterizerState->GetDesc(&rasterizerDesc);

		switch (rvars.culling_mode)
		{
		case XNG_CULLING_MODE_NONE:
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			break;
		default:
		case XNG_CULLING_MODE_FRONT:
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			break;
		case XNG_CULLING_MODE_BACK:
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			break;
		}

		XNG_HR_CHECK(m_apiContext->get_device()->CreateRasterizerState(
			&rasterizerDesc, m_rasterizerState.reset_and_get_address()));
	}
}

void forward_renderer::reload_shaders(void)
{
	m_program.reload();
}
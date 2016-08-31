#pragma once

#include <xng/core.hpp>
#include <xng/dx11/dx11_headers.hpp>

#include <xng/graphics/shader_types.hpp>

#include <utility>
#include <vector>

enum xng_dx11_shader_model
{
	XNG_DX11_SHADER_MODEL_5_0
};

namespace xng
{
	namespace dx11
	{

		namespace detail
		{
			template <typename ShaderType>
			struct shader_helper {};

			DXGI_FORMAT get_dxgi_format(D3D_REGISTER_COMPONENT_TYPE componentType, BYTE mask);
		}

		template <typename ElementDescFunctor>
		XNG_INLINE bool create_input_layout(ID3D11Device * device, ID3D11VertexShader * shader, ID3DBlob * code, ID3D11InputLayout ** inputLayout, ElementDescFunctor f)
		{
			com_ptr<ID3D11ShaderReflection> reflection;

			//if (!XNG_HR_FAILED(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&reflection)))
			if (!XNG_HR_FAILED(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_PPV_ARGS(&reflection))))
			{
				D3D11_SHADER_DESC shaderDesc;
				reflection->GetDesc(&shaderDesc);

				std::vector<D3D11_INPUT_ELEMENT_DESC> elements;

				for (int i = 0; i < shaderDesc.InputParameters; i++)
				{
					D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc;
					reflection->GetInputParameterDesc(i, &inputParamDesc);

					D3D11_INPUT_ELEMENT_DESC elementDesc;

					elementDesc.SemanticName         = inputParamDesc.SemanticName;
					elementDesc.SemanticIndex        = inputParamDesc.SemanticIndex;
					elementDesc.InputSlot            = 0;
					elementDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
					elementDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
					elementDesc.InstanceDataStepRate = 0;
					elementDesc.Format               = detail::get_dxgi_format(inputParamDesc.ComponentType, inputParamDesc.Mask);

					f(&elementDesc);

					elements.push_back(elementDesc);
				}

				return !XNG_HR_FAILED(device->CreateInputLayout(&elements[0], elements.size(), code->GetBufferPointer(), code->GetBufferSize(), inputLayout));
			}

			return false;
		}
		
		template <typename ShaderType>
		XNG_INLINE bool compile_shader_from_string(
			ID3D11Device * device,
			const char * source,
			const char * sourceName,
			const char * entryPoint,
			xng_dx11_shader_model shaderModel,
			UINT flags,
			const D3D_SHADER_MACRO * defines,
			ShaderType ** shader)
		{
			using namespace detail;

			com_ptr<ID3DBlob> code, errors;

#ifdef XNG_DX11_GLOBAL_SHADER_FLAGS
			flags |= XNG_DX11_GLOBAL_SHADER_FLAGS;
#endif

			if (!XNG_HR_FAILED_BLOB(D3DCompile(source, strlen(source), sourceName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shader_helper<ShaderType>::get_target(shaderModel), flags, 0, &code, &errors), errors))
			{
				return !XNG_HR_FAILED(shader_helper<ShaderType>::create_shader(device, code->GetBufferPointer(), code->GetBufferSize(), nullptr, shader));
			}

			return false;
		}

		template <typename ElementDescFunctor>
		XNG_INLINE bool compile_shader_from_string(ID3D11Device * device,
			const char * source,
			const char * sourceName,
			const char * entryPoint,
			xng_dx11_shader_model shaderModel,
			UINT flags, 
			const D3D_SHADER_MACRO * defines,
			ID3D11VertexShader ** shader,
			ID3D11InputLayout ** inputLayout,
			ElementDescFunctor f)
		{
			using namespace detail;

			com_ptr<ID3DBlob> code, errors;

#ifdef XNG_DX11_GLOBAL_SHADER_FLAGS
			flags |= XNG_DX11_GLOBAL_SHADER_FLAGS;
#endif			

			if (!XNG_HR_FAILED_BLOB(D3DCompile(source, strlen(source), sourceName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shader_helper<ID3D11VertexShader>::get_target(shaderModel), flags, 0, &code, &errors), errors) &&
				!XNG_HR_FAILED(shader_helper<ID3D11VertexShader>::create_shader(device, code->GetBufferPointer(), code->GetBufferSize(), nullptr, shader)))
			{
				return create_input_layout(device, *shader, code.get(), inputLayout, f);
			}

			return false;
		}

		XNG_INLINE bool compile_shader_from_string(
			ID3D11Device * device,
			const char * source,
			const char * sourceName,
			const char * entryPoint,
			xng_dx11_shader_model shaderModel,
			UINT flags,
			const D3D_SHADER_MACRO * defines,
			ID3D11VertexShader ** shader,
			ID3D11InputLayout ** inputLayout)
		{
			return compile_shader_from_string(
				device,
				source,
				sourceName,
				entryPoint,
				shaderModel,
				flags,
				defines,
				shader,
				inputLayout,
				[](const D3D11_INPUT_ELEMENT_DESC*) {});
		}

		namespace detail
		{
			template <>
			struct shader_helper<ID3D11VertexShader>
			{
				template <typename ... Args>
				XNG_INLINE static HRESULT create_shader(ID3D11Device * device, Args && ... args)
				{
					return device->CreateVertexShader(std::forward<Args>(args) ...);
				}

				XNG_INLINE static const char * get_target(xng_dx11_shader_model shaderModel)
				{
					switch (shaderModel)
					{
					default:
						XNG_LOG("DX11 Shader Compiler", "Unrecognized shader model, defaulting to 5.0.");
					case XNG_DX11_SHADER_MODEL_5_0:
						return "vs_5_0";
					}
				}
			};

			template <>
			struct shader_helper<ID3D11PixelShader>
			{
				template <typename ... Args>
				XNG_INLINE static HRESULT create_shader(ID3D11Device * device, Args && ... args)
				{
					return device->CreatePixelShader(std::forward<Args>(args) ...);
				}

				XNG_INLINE static const char * get_target(xng_dx11_shader_model shaderModel)
				{
					switch (shaderModel)
					{
					default:
						XNG_LOG("DX11 Shader Compiler", "Unrecognized shader model, defaulting to 5.0.");
					case XNG_DX11_SHADER_MODEL_5_0:
						return "ps_5_0";
					}
				}
			};

			template <>
			struct shader_helper<ID3D11GeometryShader>
			{
				template <typename ... Args>
				XNG_INLINE static HRESULT create_shader(ID3D11Device * device, Args && ... args)
				{
					return device->CreateGeometryShader(std::forward<Args>(args) ...);
				}

				XNG_INLINE static const char * get_target(xng_dx11_shader_model shaderModel)
				{
					switch (shaderModel)
					{
					default:
						XNG_LOG("DX11 Shader Compiler", "Unrecognized shader model, defaulting to 5.0.");
					case XNG_DX11_SHADER_MODEL_5_0:
						return "gs_5_0";
					}
				}
			};

			template <>
			struct shader_helper<ID3D11ComputeShader>
			{
				template <typename ... Args>
				XNG_INLINE static HRESULT create_shader(ID3D11Device * device, Args && ... args)
				{
					return device->CreateComputeShader(std::forward<Args>(args) ...);
				}

				XNG_INLINE static const char * get_target(xng_dx11_shader_model shaderModel)
				{
					switch (shaderModel)
					{
					default:
						XNG_LOG("DX11 Shader Compiler", "Unrecognized shader model, defaulting to 5.0.");
					case XNG_DX11_SHADER_MODEL_5_0:
						return "cs_5_0";
					}
				}
			};
		}
	}
}
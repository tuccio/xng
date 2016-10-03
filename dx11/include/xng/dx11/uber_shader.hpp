#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/dx11/shader_program.hpp>
#include <xng/graphics/shader_preprocessor.hpp>
#include <xng/graphics/shader_types.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace xng
{
	namespace dx11
	{
		class uber_shader
		{

		public:

			uber_shader(void);

			void clear(void);
			bool reload(void);

			bool preprocess(const char * filename);
			bool is_preprocessed(void) const;

			shader_program compile(ID3D11Device * device, const char * name = "", const std::vector<graphics::shader_macro> & macros = {});
			void free(const char * name);

			XNG_INLINE void set_compile_flags(UINT flags)
			{
				m_flags = flags;
			}
			
			template <typename F>
			XNG_INLINE void set_ilv_functor(F f)
			{
				m_ilvFunctor = f;
			}

		private:

			std::string m_filename;

			std::unordered_map<std::string, shader_program> m_shaders;

			UINT m_flags;
			bool m_preprocessed;
			
			std::function<void(D3D11_INPUT_ELEMENT_DESC*)> m_ilvFunctor;

			graphics::shader_preprocessor::output_type m_preprocessedShader;

		};
	}
}
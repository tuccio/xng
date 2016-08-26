#pragma once

#include <xng/graphics/renderer.hpp>
#include <xng/gl/shader_program.hpp>
#include <xng/gl/vao_manager.hpp>
#include <xng/gl/ring_buffer.hpp>
#include <xng/gl/uber_shader.hpp>

namespace xng
{
	namespace gl
	{
		class forward_renderer :
			public graphics::renderer
		{

		public:

			bool init(graphics::api_context * context) override;
			void shutdown(void) override;

			void render(graphics::scene * scene, graphics::camera * camera, graphics::render_resource * target = nullptr) override;

		private:

			void process_rv_updates(graphics::render_variables * rvars);

			gl_api_context * m_context;

			uber_shader      m_program;
			vao_manager      m_vaos;
						     
			ring_buffer      m_bPerObject;
			
		};
	}
}
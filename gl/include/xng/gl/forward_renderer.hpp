#pragma once

#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/shader_program.hpp>
#include <xng/gl/ring_buffer.hpp>
#include <xng/gl/uber_shader.hpp>
#include <xng/graphics.hpp>

namespace xng
{
	namespace gl
	{
		class forward_renderer
		{

		public:

			bool init(gl_api_context * context);
			void shutdown(void);

			void update_render_variables(const graphics::render_variables & rvars, const graphics::render_variables_updates & update);
			void render(graphics::scene * scene, graphics::camera * camera);

		private:

			gl_api_context * m_context;

			graphics::render_variables m_rvars;

			uber_shader m_program;
			ring_buffer m_bPerObject;

		};
	}
}
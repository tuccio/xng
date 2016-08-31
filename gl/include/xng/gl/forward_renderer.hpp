#pragma once

#include <xng/graphics/renderer.hpp>
#include <xng/gl/shader_program.hpp>
#include <xng/gl/vertex_buffers_factory.hpp>
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

			void render(graphics::scene * scene, const graphics::camera * camera, graphics::render_resource * target = nullptr) override;

		private:

			void process_rv_updates(graphics::render_variables * rvars);

			gl_api_context * m_context;

			uber_shader m_program;
			ring_buffer m_bPerObject;

			vertex_buffers_factory m_vbFactory;
			
		};
	}
}
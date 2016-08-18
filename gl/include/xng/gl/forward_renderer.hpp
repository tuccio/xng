#pragma once

#include <xng/graphics/renderer.hpp>
#include <xng/gl/shader_program.hpp>

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

			shader_program m_program;

		};
	}
}
#pragma once

#include "camera.hpp"
#include "scene.hpp"

#include "render_resource.hpp"
#include "api_context.hpp"
#include "renderer_configuration.hpp"

namespace xng
{
	namespace graphics
	{
		struct renderer
		{
			virtual bool init(api_context * context) { return true; }
			virtual void shutdown(void)	{}

			virtual void render(scene * scene, const camera * camera, render_resource * target = nullptr) = 0;

			renderer_configuration * get_configuration(void)
			{
				return &m_configuration;
			}

			const renderer_configuration * get_configuration(void) const
			{
				return &m_configuration;
			}

		protected:

			void get_render_variables(render_variables * vars, std::set<xng_render_variable> * updates)
			{
				m_configuration.get_render_variables(vars, updates);
			}

		private:

			renderer_configuration m_configuration;

		};
	}
}
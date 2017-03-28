#pragma once

#include <xng/dx11/debug_renderer.hpp>
#include <boost/variant.hpp>

#include <unordered_map>
#include <string>

namespace xng
{
	namespace dx11
	{
		class visual_debugger
		{

		public:

			visual_debugger(void) = default;
			visual_debugger(const visual_debugger &) = delete;
			visual_debugger(visual_debugger &&) = default;

			bool init(dx11_api_context * context, const graphics::render_variables & rvars);
			void shutdown(void);

			void add_persistent(const char * name, const math::aabb & bb, const math::float4 & color);
			void add_persistent(const char * name, const math::frustum & f, const math::float4 & color);

			void remove_persistent(const char * name);

			void render(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const graphics::camera * camera);

		private:

			typedef boost::variant<math::aabb, math::frustum> geometry;

			struct draw_info
			{
				geometry     geometry;
				math::float4 color;
			};

			debug_renderer m_renderer;

			std::unordered_map<std::string, draw_info> m_persistentObjects;

			void draw_persistent_objects(void);
		};
	}

	

}
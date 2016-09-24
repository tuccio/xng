#include <xng/graphics/scene_graph_node.hpp>
#include <xng/graphics/camera.hpp>

namespace xng
{
	namespace graphics
	{
		class scene_graph_camera :
			public scene_graph_node
		{

		public:

			scene_graph_camera(scene_graph_node * parent);

			XNG_INLINE camera * get_camera(void)
			{
				return &m_camera;
			}

			XNG_INLINE const camera * get_camera(void) const
			{
				return &m_camera;
			}

		private:

			camera m_camera;

		};
	}
}
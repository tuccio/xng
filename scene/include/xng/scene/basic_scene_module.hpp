#include <xng/scene/basic_scene.hpp>
#include <xng/engine.hpp>
#include <unordered_map>

#include <memory>


namespace xng
{
	namespace scene
	{
		class basic_scene_module :
			public engine::scene_module
		{

		public:

			static const char *          module_name;
			static const char *          module_description;
			static const xng_module_type module_type;

			basic_scene_module(void);

			bool init(void) override;
			void shutdown(void) override;
			bool is_initialized(void) const override;

			graphics::scene_ptr create_scene(const char * name) override;
			bool destroy_scene(const char * name) override;
			graphics::scene_ptr find_scene(const char * name) override;

			graphics::scene_ptr get_active_scene(void) override;
			void set_active_scene(const graphics::scene_ptr & scene) override;

		private:

			std::unordered_map<std::string, graphics::scene_ptr> m_scenes;
			graphics::scene_ptr m_activeScene;
			bool m_initialized;


		};
	}
}
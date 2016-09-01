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

			graphics::scene * create_scene(const char * name) override;
			graphics::scene * find_scene(const char * name) override;

			graphics::scene * get_active_scene(void) override;
			void set_active_scene(graphics::scene * scene) override;

		private:

			std::unordered_map<std::string, std::unique_ptr<basic_scene>> m_scenes;
			basic_scene * m_activeScene;
			bool m_initialized;


		};
	}
}
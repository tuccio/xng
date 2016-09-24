#include <xng/engine.hpp>
#include "editor.hpp"

namespace xng
{
	namespace editor
	{
		class editor_module :
			public engine::runtime_module
		{

		public:

			static const char *          module_name;
			static const char *          module_description;
			static const xng_module_type module_type;

			editor_module(void) = default;

			bool init(void) override;
			void shutdown(void) override;
			bool is_initialized(void) const override;

			void update(float dt) override;

		private:

			std::unique_ptr<editor> m_editor;
			std::unique_ptr<os::native_window_observer> m_observer;

		};
	}
}
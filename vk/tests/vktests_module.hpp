#include <xng/engine.hpp>

#include "tests_window.hpp"

#include <memory>

namespace xng
{
    namespace vk
    {
		namespace test
		{
			class vktests_module :
				public engine::runtime_module
			{

			public:

				static const char *          module_name;
				static const char *          module_description;
				static const xng_module_type module_type;

				vktests_module(void) = default;

				bool init(void) override;
				void shutdown(void) override;
				bool is_initialized(void) const override;

				void update(float dt) override;

			private:

				std::unique_ptr<tests_window> m_testsWindow;

			};
		}
    }
}
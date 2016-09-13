#pragma once

#include <xng/gui/layout.hpp>
#include <xng/gui/widget.hpp>

namespace xng
{
	namespace gui
	{
		class vertical_layout :
			public layout
		{

		public:

			vertical_layout(void);

			void add(widget * widget, uint32_t flags, uint32_t proportion = 0, uint32_t padding = 0);

		protected:

			virtual void apply(void) override;

		private:

			struct layout_entry
			{
				widget * widget;
				uint32_t flags;
				uint32_t proportion;
				uint32_t padding;
			};

			typedef std::vector<layout_entry> entry_vector;

			entry_vector m_entries;
			uint32_t     m_weightSum;

		};
	}
}
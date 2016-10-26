#pragma once

#include "wx.hpp"

#include <wx/treectrl.h>

#include <xng/graphics.hpp>

namespace std
{
	template <>
	struct hash<wxTreeItemId>
	{
		size_t operator()(const wxTreeItemId & lhs) const
		{
			return std::hash<const void*>()(lhs.GetID());
		}
	};
}

namespace xng
{
	namespace editor
	{
		struct scene_graph_page_observer
		{
			virtual void on_node_select(graphics::scene_graph * sg, graphics::scene_graph_node * node) {}
		};

		class scene_graph_page :
			public wxWindow,
			public graphics::scene_graph_observer,
			public core::observable<scene_graph_page_observer>
		{

		public:

			scene_graph_page(graphics::scene_graph * g, wxWindow * parent);
			~scene_graph_page(void);

			void update_scene_graph(void);
			void update_selected_object(void);

			graphics::scene_graph_node * get_selected_node(void) const;
			void set_selected_node(graphics::scene_graph_node * node);

			void on_create(graphics::scene_graph * sg, graphics::scene_graph_node * node) override;
			void on_destroy(graphics::scene_graph * sg, graphics::scene_graph_node * node) override;
			void on_move(graphics::scene_graph * sg, graphics::scene_graph_node * node, const math::float4x4 & lastTransform, const math::float4x4 & newTransform) override;

		private:

			graphics::scene_graph      * m_sceneGraph;
			graphics::scene_graph_node * m_selectedNode;
			wxTreeCtrl                 * m_sgTree;
			wxTreeItemId                 m_draggedItem;

			std::unordered_map<graphics::scene_graph_node*, wxTreeItemId> m_nodeItemMap;
			std::unordered_map<wxTreeItemId, graphics::scene_graph_node*> m_itemNodeMap;

			void add_branch(graphics::scene_graph_node * root);
			void remove_branch(graphics::scene_graph_node * root);

		};
	}
}
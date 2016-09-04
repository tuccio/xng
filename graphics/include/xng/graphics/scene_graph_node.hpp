#pragma once

#include <xng/core.hpp>
#include <xng/math.hpp>
#include <xng/geometry/transform_hierarchy.hpp>

#include <string>
#include <vector>

enum xng_scene_graph_node_type
{
	XNG_SCENE_GRAPH_GROUP,
	XNG_SCENE_GRAPH_GEOMETRY,
	XNG_SCENE_GRAPH_CAMERA,
	XNG_SCENE_GRAPH_LIGHT
};

namespace xng
{
	namespace graphics
	{
		class scene_graph;

		class scene_graph_node :
			public math::transform_hierarchy<scene_graph_node>
		{

		public:

			using children_iterator = std::vector<scene_graph_node*>::const_iterator;

			virtual ~scene_graph_node(void);

			XNG_INLINE children_iterator begin(void) const
			{
				return m_children.cbegin();
			}

			XNG_INLINE children_iterator end(void) const
			{
				return m_children.cend();
			}

			XNG_INLINE bool is_root(void) const
			{
				return m_parent == nullptr;
			}

			XNG_INLINE bool is_leaf(void) const
			{
				return m_children.empty();
			}

			template <typename NodeType, typename ... Args>
			XNG_INLINE  NodeType * add_child(Args && ... args)
			{
				NodeType * child = new NodeType(this, std::forward<Args>(args) ...);
				m_children.push_back(child);
				return child;
			}

			void clear_children(void);
			void update(void);

			scene_graph      * get_scene_graph(void);
			scene_graph_node * get_root(void);

			scene_graph_node * find_child_by_name(const char * name);
			scene_graph_node * find_child_by_type(xng_scene_graph_node_type type);

			std::vector<scene_graph_node*> find_children_by_name(const char * name);
			std::vector<scene_graph_node*> find_children_by_type(xng_scene_graph_node_type type);

			scene_graph_node * get_parent(void) const;
			void set_parent(scene_graph_node * parent);

			const char * get_name(void) const;
			void set_name(const char * name);

			xng_scene_graph_node_type get_type(void) const;

			bool is_ancestor(scene_graph_node * node) const;
			bool is_descendant(scene_graph_node * node) const;

			virtual scene_graph_node * clone(scene_graph_node * newParent, scene_graph * newSceneGraph) = 0;

		protected:

			scene_graph_node(xng_scene_graph_node_type type, scene_graph_node * parent, scene_graph * graph) :
				m_graph(graph),
				m_type(type),
				m_parent(parent) {}

			scene_graph_node(const scene_graph_node &) = default;

			void on_parent_destruction(void);
			void on_child_destruction(scene_graph_node * child);

			virtual void update_impl(void) {}

			xng_scene_graph_node_type m_type;

			scene_graph_node * m_parent;
			scene_graph      * m_graph;

			std::vector<scene_graph_node*> m_children;
			std::string m_name;

			friend class scene_graph;

			void set_scene_graph(scene_graph * graph);
			void set_parent_internal(scene_graph_node * node);

			void on_parent_destruction_internal(void);
			void on_child_destruction_internal(scene_graph_node * child);

			// Transform hierarchy calls

			XNG_INLINE children_iterator get_transform_hierarchy_children_begin(void) const
			{
				return m_children.begin();
			}

			XNG_INLINE children_iterator get_transform_hierarchy_children_end(void) const
			{
				return m_children.end();
			}

			XNG_INLINE scene_graph_node * get_transform_hierarchy_parent(void) const
			{
				return m_parent;
			}

			XNG_INLINE void set_transform_hierarchy_parent(scene_graph_node * parent)
			{
				m_parent = parent;
			}

			friend class math::transform_hierarchy<scene_graph_node>;

		};
	}
}
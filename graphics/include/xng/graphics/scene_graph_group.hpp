#pragma once

#include <xng/graphics/scene_graph_node.hpp>

namespace xng
{
    namespace graphics
    {
        class scene_graph_group :
            public scene_graph_node
        {

        public:

            scene_graph_group(scene_graph * graph) :
                scene_graph_node(XNG_SCENE_GRAPH_GROUP, nullptr, graph) {}

            scene_graph_group(scene_graph_node * parent) :
                scene_graph_node(XNG_SCENE_GRAPH_GROUP, parent, parent->get_scene_graph()) {}

        };
    }
}
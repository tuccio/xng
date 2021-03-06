#pragma once

#include <xng/gl/gl_headers.hpp>
#include <xng/graphics.hpp>

#include <unordered_map>

namespace xng
{
    namespace gl
    {
        class vertex_buffers_factory
        {

        public:

            struct buffers
            {
                GLuint vao;
                GLuint positions;
                GLuint normals;
                GLuint indices;
            };

            buffers create(graphics::mesh_ptr m);

        private:

            std::unordered_map<res::resource_id, buffers> m_buffers;

        };
    }
}
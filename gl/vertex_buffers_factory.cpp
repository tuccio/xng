#include <xng/gl/vertex_buffers_factory.hpp>
#include <xng/gl/attributes.hpp>
#include <xng/gl/errors.hpp>

using namespace xng::graphics;
using namespace xng::gl;
using namespace xng::math;

vertex_buffers_factory::buffers vertex_buffers_factory::create(mesh_ptr m)
{
	buffers b = {};

	auto it = m_buffers.find(m->get_id());

	if (it == m_buffers.end() && m->load())
	{
		XNG_GL_CHECK(glGenVertexArrays(1, &b.vao));
		XNG_GL_CHECK(glBindVertexArray(b.vao));

		XNG_GL_CHECK(glGenBuffers(1, &b.positions));

		XNG_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, b.positions));
		XNG_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m->get_num_vertices() * sizeof(float3), m->get_vertices(), GL_STATIC_DRAW));

		XNG_GL_CHECK(glEnableVertexAttribArray(XNG_GL_ATTRIBUTE_POSITION));
		XNG_GL_CHECK(glVertexAttribPointer(XNG_GL_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float3), 0));

		XNG_GL_CHECK(glBindVertexArray(0));

		XNG_GL_CHECK(glGenBuffers(1, &b.indices));

		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.indices));
		XNG_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->get_num_indices() * sizeof(uint3), m->get_indices(), GL_STATIC_DRAW));

		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		m_buffers[m->get_id()] = b;
	}
	else
	{
		b = it->second;
	}

	return b;
}
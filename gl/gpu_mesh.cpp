#pragma once

#include <xng/gl/gpu_mesh.hpp>
#include <xng/gl/attributes.hpp>
#include <xng/gl/gpu_mesh_manager.hpp>

#include <xng/graphics/mesh.hpp>
#include <xng/res.hpp>

#include <string>

using namespace xng::gl;
using namespace xng::res;

gpu_mesh::gpu_mesh(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	resource(name, params, loader, owner),
	m_positionVAO(0), m_nonPositionalVAO(0),
	m_positionVBO(0),
	m_indicesEBO(0) {}

gpu_mesh::~gpu_mesh(void)
{
	clear();
}

void gpu_mesh::clear(void)
{

#define XNG_RELEASE_VAO(VAO) if (VAO) { XNG_GL_CHECK(glDeleteVertexArrays(1, &VAO)); VAO = 0; }
#define XNG_RELEASE_VBO(VBO) if (VBO) { XNG_GL_CHECK(glDeleteBuffers(1, &VBO)); VBO = 0; }
#define XNG_RELEASE_EBO(EBO) XNG_RELEASE_VBO(EBO)

	XNG_RELEASE_VBO(m_positionVAO);
	XNG_RELEASE_VBO(m_nonPositionalVAO);

	XNG_RELEASE_EBO(m_indicesEBO);
	XNG_RELEASE_VBO(m_positionVBO);

	m_numVertices = 0;
	m_numIndices  = 0;
}

bool gpu_mesh::load_impl(void)
{
	graphics::mesh_ptr m = res::resource_factory::get_singleton()->
		find_by_id<graphics::mesh>("mesh", std::atol(get_name()));

	if (m && m->load())
	{
		XNG_GL_CHECK(glGenVertexArrays(1, &m_positionVAO));
		XNG_GL_CHECK(glBindVertexArray(m_positionVAO));

		XNG_GL_CHECK(glGenBuffers(1, &m_positionVBO));

		XNG_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO));
		XNG_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m->get_num_vertices() * sizeof(float) * 3, m->get_vertices(), GL_STATIC_DRAW));

		XNG_GL_CHECK(glEnableVertexAttribArray(XNG_GL_ATTRIBUTE_POSITION));
		XNG_GL_CHECK(glVertexAttribPointer(XNG_GL_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0));

		XNG_GL_CHECK(glBindVertexArray(0));

		XNG_GL_CHECK(glGenBuffers(1, &m_indicesEBO));

		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesEBO));
		XNG_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->get_num_indices() * sizeof(uint32_t) * 3, m->get_indices(), GL_STATIC_DRAW));
		m_indicesFormat = GL_UNSIGNED_INT;

		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		m_numIndices  = m->get_num_indices();
		m_numVertices = m->get_num_vertices();

		return true;
	}

	return false;
}

void gpu_mesh::unload_impl(void)
{
	clear();
}

size_t gpu_mesh::calculate_size_impl(void)
{
	// TODO: Proper calculation
	return m_numVertices * sizeof(float) * 3 + m_numIndices * sizeof(uint32_t);
}

gpu_mesh_ptr xng::gl::make_gpu_mesh(graphics::mesh_ptr mesh)
{
	return res::resource_factory::get_singleton()->
		create<gpu_mesh>("glmesh", std::to_string(mesh->get_id()).c_str());
}
#include <xng/gl/forward_renderer.hpp>

#include <xng/gl.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

#include <xng/gl/attributes.hpp>

using namespace xng::gl;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

bool forward_renderer::init(api_context * context)
{
	m_context = dynamic_cast<gl_api_context*>(context);

	shader vs, fs;

	if (m_context &&
		m_bPerObject.init(m_context, GL_SHADER_STORAGE_BUFFER, 20 << 20) &&
		m_program.preprocess(XNG_GL_SHADER_FILE("test.xglsl")))
	{
		return true;
	}

	return false;
}

void forward_renderer::shutdown(void)
{
	m_program.clear();
}

void forward_renderer::render(scene * scene, camera * camera, render_resource * target)
{
	graphics::render_variables rvars;
	process_rv_updates(&rvars);

	XNG_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	XNG_GL_CHECK(glViewport(0, 0, rvars.render_resolution.x, rvars.render_resolution.y));

	const shader_program * program = m_program.compile();
	//m_program.use();
	program->use();

	mesh_ptr triangle = resource_factory::get_singleton()->create<mesh>(
		"mesh", "triangle", resource_parameters(),
		std::make_shared<dynamic_resource_loader>(
			[](resource * r)
	{
		mesh * m = static_cast<mesh*>(r);
		
		if (m->create(3, 1, XNG_MESH_STORAGE_NONE))
		{
			GLfloat vertices[] = {
				-1.f, -1.f, 0.f,
				1.f, -1.f, 0.f,
				0.f,  1.f, 0.f,
			};

			GLuint indices[] = {
				0, 1, 2
			};

			memcpy(m->get_vertices(), vertices, sizeof(vertices));
			memcpy(m->get_indices(), indices, sizeof(indices));

			return true;
		}

		return false;
	},
			[](resource * r)
	{
		static_cast<mesh*>(r)->clear();
	}
	));

	GLuint bPerObjectOffset, bPerObjectSize;

	float4 colors[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 }
	};
	
	float4 * buffer = m_bPerObject.allocate_buffer<float4>(&bPerObjectOffset, &bPerObjectSize);
	*buffer = colors[(m_context->get_frame_index() / 60) % 3];

	XNG_GL_CHECK(glBindBufferRange(m_bPerObject.get_target(), 0, m_bPerObject.get_buffer(), bPerObjectOffset, bPerObjectSize));
	auto b = m_vaos.create(triangle);

	XNG_GL_CHECK(glBindVertexArray(b.vao));
	XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.indices));
	XNG_GL_CHECK(glDrawElements(GL_TRIANGLES, triangle->get_num_indices(), GL_UNSIGNED_INT, 0));

	XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	XNG_GL_CHECK(glBindVertexArray(0));

	XNG_GL_CHECK(glDeleteBuffers(1, &bPerObjectOffset));

	program->dispose();
}

void forward_renderer::process_rv_updates(render_variables * rvars)
{
	std::set<xng_render_variable> updates;
	get_render_variables(rvars, &updates);
}
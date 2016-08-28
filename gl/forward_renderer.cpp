#include <xng/gl/forward_renderer.hpp>

#include <xng/gl.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

#include <xng/gl/gpu_mesh.hpp>

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

	program->use();

	mesh_ptr triangle = resource_factory::get_singleton()->create<mesh>(
		"mesh", "triangle", resource_parameters(),
		std::make_shared<dynamic_resource_loader>(
			[](resource * r)
	{
		mesh * m = static_cast<mesh*>(r);
		
		if (m->create(3, 1, XNG_MESH_STORAGE_NONE))
		{
			float vertices[] = {
				-1.f, -1.f, 0.f,
				1.f, -1.f, 0.f,
				0.f,  1.f, 0.f,
			};

			uint32_t indices[] = {
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

	float4 colors[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 }
	};

	gpu_mesh_ptr m = make_gpu_mesh(triangle);

	if (m && m->load())
	{
		struct __bufferPerObject
		{
			float4x4 viewMatrix;
			float4x4 viewProjectionMatrix;
			float4   color;
		};

		GLuint bPerObjectOffset, bPerObjectSize;
		void * buffer = m_bPerObject.allocate_buffer(sizeof(__bufferPerObject), &bPerObjectOffset, &bPerObjectSize);

		__bufferPerObject bufferPerObjectData = {
			float4x4(1),
			float4x4(1),
			colors[(m_context->get_frame_index() / 60) % 3]
		};

		memcpy(buffer, &bufferPerObjectData, bPerObjectSize);

		XNG_GL_CHECK(glBindBufferRange(m_bPerObject.get_target(), 0, m_bPerObject.get_buffer(), bPerObjectOffset, bPerObjectSize));

		XNG_GL_CHECK(glBindVertexArray(m->get_positional_vao()));
		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->get_indices_ebo()));
		XNG_GL_CHECK(glDrawElements(GL_TRIANGLES, m->get_num_indices(), m->get_indices_format(), nullptr));

		XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		XNG_GL_CHECK(glBindVertexArray(0));;
	}

	program->dispose();
}

void forward_renderer::process_rv_updates(render_variables * rvars)
{
	std::set<xng_render_variable> updates;
	get_render_variables(rvars, &updates);
}
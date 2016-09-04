#include <xng/gl/forward_renderer.hpp>

#include <xng/gl.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

#include <xng/gl/gpu_mesh.hpp>

using namespace xng::gl;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

bool forward_renderer::init(gl_api_context * context)
{
	m_context = context;

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

void forward_renderer::render(scene * scene, camera * camera)
{
	XNG_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	XNG_GL_CHECK(glViewport(0, 0, m_rvars.render_resolution.x, m_rvars.render_resolution.y));
	
	XNG_GL_CHECK(glFrontFace(GL_CCW));
	XNG_GL_CHECK(glCullFace(GL_BACK));
	XNG_GL_CHECK(glEnable(GL_CULL_FACE));

	if (scene && camera)
	{
		const shader_program * program = m_program.compile();

		program->use();

		float4 colors[] = {
			{ 1, 0, 0, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 0, 1, 1 }
		};

		scene::geometry_vector geometry = scene->get_geometry_nodes();

		const float4x4 & viewMatrix       = camera->get_gl_view_matrix();
		const float4x4 & projectionMatrix = camera->get_gl_projection_matrix();

		float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

		int ColorIndex = 0;

		for (scene_graph_geometry * gNode : geometry)
		{
			gpu_mesh_ptr m = make_gpu_mesh(gNode->get_mesh());

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

				const float4x4 & modelMatrix = gNode->get_global_matrix();

				__bufferPerObject bufferPerObjectData = {
					transpose(viewMatrix * modelMatrix),
					transpose(viewProjectionMatrix * modelMatrix),
					colors[ColorIndex++ % 3]
				};

				memcpy(buffer, &bufferPerObjectData, bPerObjectSize);

				XNG_GL_CHECK(glBindBufferRange(m_bPerObject.get_target(), 0, m_bPerObject.get_buffer(), bPerObjectOffset, bPerObjectSize));

				XNG_GL_CHECK(glBindVertexArray(m->get_positional_vao()));
				XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->get_indices_ebo()));
				XNG_GL_CHECK(glDrawElements(GL_TRIANGLES, m->get_num_indices(), m->get_indices_format(), nullptr));

				XNG_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
				XNG_GL_CHECK(glBindVertexArray(0));
			}
		}		
	}

	
}

void forward_renderer::update_render_variables(const render_variables & rvars, const render_variables_updates & update)
{
	m_rvars = rvars;
}
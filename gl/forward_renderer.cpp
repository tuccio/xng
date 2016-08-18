#include <xng/gl/forward_renderer.hpp>

#include <xng/gl.hpp>

using namespace xng;
using namespace xng::gl;

bool forward_renderer::init(graphics::api_context * context)
{
	shader vs, fs;

	if (vs.compile_from_file(XNG_GL_SHADER_VERTEX, XNG_GL_SHADER_FILE("test_shader.vert")) &&
		fs.compile_from_file(XNG_GL_SHADER_FRAGMENT, XNG_GL_SHADER_FILE("test_shader.frag")))
	{
		m_program.attach_shader(&vs);
		m_program.attach_shader(&fs);

		return m_program.link();
	}

	return false;
}

void forward_renderer::shutdown(void)
{
	m_program.clear();
}

void forward_renderer::render(graphics::scene * scene, graphics::camera * camera, graphics::render_resource * target)
{
	graphics::render_variables rvars;
	process_rv_updates(&rvars);

	XNG_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	XNG_GL_CHECK(glViewport(0, 0, rvars.render_resolution.x, rvars.render_resolution.y));

	
}

void forward_renderer::process_rv_updates(graphics::render_variables * rvars)
{
	std::set<xng_render_variable> updates;
	get_render_variables(rvars, &updates);
}
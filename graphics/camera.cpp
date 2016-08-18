#include <xng/graphics/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace xng;
using namespace xng::graphics;

camera::camera(void) :
	m_position(0),
	m_orientation(1, 0, 0, 0),
	m_fovy(glm::pi<float>()),
	m_ratio(1.f),
	m_znear(0.1f),
	m_zfar(1000.f) {}

void camera::look_at(const glm::vec3 & eye, const glm::vec3 & up, const glm::vec3 & target)
{
	m_viewMatrix      = glm::lookAtRH(eye, target, up);
	m_position        = eye;
	m_orientation     = glm::conjugate(glm::quat_cast(m_viewMatrix)),
	m_viewMatrixDirty = false;
}

void camera::move(const glm::vec3 & delta)
{
	m_position += delta.x * right() + delta.y * up() + delta.z * forward();
	m_viewMatrixDirty = true;
}

const glm::vec3 & camera::get_position(void) const
{
	return m_position;
}

void camera::set_position(const glm::vec3 & position)
{
	m_position        = position;
	m_viewMatrixDirty = true;
}

const glm::quat & camera::get_orientation(void) const
{
	return m_orientation;
}

void camera::set_orientation(const glm::quat & orientation)
{
	m_orientation     = orientation;
	m_viewMatrixDirty = true;
}

float camera::get_fovy(void) const
{
	return m_fovy;
}

void camera::set_fovy(float fovy)
{
	m_fovy                  = fovy;
	m_projectionMatrixDirty = true;
}

float camera::get_fovx(void) const
{
	return 2 * glm::atan(glm::tan(m_fovy * .5f) * m_ratio);
}

void camera::set_fovx(float fovx)
{
	m_fovy                  = 2 * glm::atan(glm::tan(fovx * .5f) / m_ratio);
	m_projectionMatrixDirty = true;
}

float camera::get_aspect_ratio(void) const
{
	return m_ratio;
}

void camera::set_aspect_ratio(float ratio)
{
	m_ratio                 = ratio;
	m_projectionMatrixDirty = true;
}

float camera::get_znear(void) const
{
	return m_znear;
}

void camera::set_znear(float znear)
{
	m_znear                 = znear;
	m_projectionMatrixDirty = true;
}

float camera::get_zfar(void) const
{
	return m_zfar;
}

void camera::set_zfar(float zfar)
{
	m_zfar                  = zfar;
	m_projectionMatrixDirty = true;
}

const glm::mat4 & camera::get_view_matrix(void) const
{
	if (m_viewMatrixDirty)
	{
		m_viewMatrix = glm::mat4_cast(glm::conjugate(m_orientation));
		m_viewMatrix[3][0] = -glm::dot(glm::vec3(m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2]), m_position);
		m_viewMatrix[3][1] = -glm::dot(glm::vec3(m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2]), m_position);
		m_viewMatrix[3][2] = -glm::dot(glm::vec3(m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2]), m_position);
		m_viewMatrixDirty = false;
	}
	return m_viewMatrix;
}

const glm::mat4 & camera::get_projection_matrix(void) const
{
	if (m_projectionMatrixDirty)
	{
		m_projectionMatrix = glm::perspectiveRH(m_fovy * .5f, m_ratio, m_znear, m_zfar);
		m_projectionMatrixDirty = false;
	}
	return m_projectionMatrix;
}

const glm::vec3 camera::forward(void)
{
	return m_orientation * glm::vec3(0, 0, -1);
}

const glm::vec3 camera::right(void)
{
	return m_orientation * glm::vec3(1, 0, 0);
}

const glm::vec3 camera::up(void)
{
	return m_orientation * glm::vec3(0, 1, 0);
}
#include <xng/graphics/camera.hpp>

using namespace xng;
using namespace xng::graphics;
using namespace xng::math;

camera::camera(void) :
	m_position(0),
	m_orientation(1, 0, 0, 0),
	m_fovy(to_radians(75.f)),
	m_ratio(1.f),
	m_znear(0.1f),
	m_zfar(1000.f),
	m_coordSystem(XNG_COORDINATE_SYSTEM_OPENGL) {}

void camera::look_at(const float3 & eye, const float3 & up, const float3 & target)
{
	m_viewMatrix      = look_at_rh(eye, up, target);
	m_position        = eye;
	m_orientation     = conjugate(to_quaternion(m_viewMatrix)),
	m_viewMatrixDirty = false;
}

void camera::move(const float3 & delta)
{
	m_position = m_position + delta.x * right() + delta.y * up() + delta.z * forward();
	m_viewMatrixDirty = true;
}

const float3 & camera::get_position(void) const
{
	return m_position;
}

void camera::set_position(const float3 & position)
{
	m_position        = position;
	m_viewMatrixDirty = true;
}

const quaternion & camera::get_orientation(void) const
{
	return m_orientation;
}

void camera::set_orientation(const quaternion & orientation)
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
	return 2 * std::atan(std::tan(m_fovy * .5f) * m_ratio);
}

void camera::set_fovx(float fovx)
{
	m_fovy                  = 2 * std::atan(std::tan(fovx * .5f) / m_ratio);
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

XNG_INLINE void gl_view_matrix(float4x4 & viewMatrix, const float3 & position, const quaternion & orientation)
{
	viewMatrix     = transpose(to_rotation4(orientation));
	viewMatrix._03 = -dot(to_float3(viewMatrix.r[0]), position);
	viewMatrix._13 = -dot(to_float3(viewMatrix.r[1]), position);
	viewMatrix._23 = -dot(to_float3(viewMatrix.r[2]), position);
}

XNG_INLINE void directx_view_matrix(float4x4 & viewMatrix, const float3 & position, const quaternion & orientation)
{
	viewMatrix     = transpose(to_rotation4(orientation));
	viewMatrix._03 = -dot(to_float3(viewMatrix.r[0]), position);
	viewMatrix._13 = -dot(to_float3(viewMatrix.r[1]), position);
	viewMatrix._23 = -dot(to_float3(viewMatrix.r[2]), position);
}

const math::float4x4 & camera::get_view_matrix(void) const
{
	switch (m_coordSystem)
	{
	case XNG_COORDINATE_SYSTEM_DIRECTX:
		return get_directx_view_matrix();

	case XNG_COORDINATE_SYSTEM_OPENGL:
		return get_gl_view_matrix();
	}

	return m_viewMatrix;
}

const math::float4x4 & camera::get_projection_matrix(void) const
{
	switch (m_coordSystem)
	{
	case XNG_COORDINATE_SYSTEM_DIRECTX:
		return get_directx_projection_matrix();

	case XNG_COORDINATE_SYSTEM_OPENGL:
		return get_gl_projection_matrix();
	}

	return m_projectionMatrix;
}

const float4x4 & camera::get_gl_view_matrix(void) const
{
	if (m_viewMatrixDirty || m_coordSystem != XNG_COORDINATE_SYSTEM_OPENGL)
	{
		gl_view_matrix(m_viewMatrix, m_position, m_orientation);;
		m_coordSystem     = XNG_COORDINATE_SYSTEM_OPENGL;
		m_viewMatrixDirty = false;
	}
	return m_viewMatrix;
}

const float4x4 & camera::get_directx_view_matrix(void) const
{
	if (m_viewMatrixDirty || m_coordSystem != XNG_COORDINATE_SYSTEM_DIRECTX)
	{
		directx_view_matrix(m_viewMatrix, m_position, m_orientation);;
		m_coordSystem     = XNG_COORDINATE_SYSTEM_DIRECTX;
		m_viewMatrixDirty = false;
	}
	return m_viewMatrix;
}

// TODO: Just do everything right handed

XNG_INLINE void gl_projection_matrix(float4x4 & projectionMatrix, float fovy, float ratio, float znear, float zfar)
{
	float h = 1.f / std::tan(fovy * .5f);
	float w = h / ratio;
	float invFmN = 1.f / (zfar - znear);

	projectionMatrix = float4x4 {
		w, 0.f, 0.f, 0.f,
		0.f, h, 0.f, 0.f,
		0.f, 0.f, -(zfar + znear) * invFmN, - 2.f * zfar * znear * invFmN,
		0.f, 0.f, -1.f, 0.f
	};
}

XNG_INLINE void directx_projection_matrix(float4x4 & projectionMatrix, float fovy, float ratio, float znear, float zfar)
{
	float h = 1.f / std::tan(fovy * .5f);
	float w = h / ratio;
	float invNmF = 1.f / (znear - zfar);
	float Q = zfar * invNmF;

	projectionMatrix = float4x4{
		w, 0.f, 0.f, 0.f,
		0.f, h, 0.f, 0.f,
		0.f, 0.f, Q, Q * znear,
		0.f, 0.f, -1.f, 0.f
	};
}

const float4x4 & camera::get_gl_projection_matrix(void) const
{
	if (m_projectionMatrixDirty || m_coordSystem != XNG_COORDINATE_SYSTEM_OPENGL)
	{
		gl_projection_matrix(m_projectionMatrix, m_fovy, m_ratio, m_znear, m_zfar);
		m_coordSystem           = XNG_COORDINATE_SYSTEM_OPENGL;
		m_projectionMatrixDirty = false;
	}
	return m_projectionMatrix;
}

const float4x4 & camera::get_directx_projection_matrix(void) const
{
	if (m_projectionMatrixDirty || m_coordSystem != XNG_COORDINATE_SYSTEM_DIRECTX)
	{
		directx_projection_matrix(m_projectionMatrix, m_fovy, m_ratio, m_znear, m_zfar);
		m_coordSystem           = XNG_COORDINATE_SYSTEM_DIRECTX;
		m_projectionMatrixDirty = false;
	}
	return m_projectionMatrix;
}

const float3 camera::forward(void)
{
	return transform(float3(0, 0, -1), m_orientation);
}

const float3 camera::back(void)
{
	return transform(float3(0, 0, 1), m_orientation);
}

const float3 camera::left(void)
{
	return transform(float3(-1, 0, 0), m_orientation);
}

const float3 camera::right(void)
{
	return transform(float3(1, 0, 0), m_orientation);
}

const float3 camera::up(void)
{
	return transform(float3(0, 1, 0), m_orientation);
}

const float3 camera::down(void)
{
	return transform(float3(0, -1, 0), m_orientation);
}

frustum camera::get_frustum(void) const
{
	return frustum(get_directx_projection_matrix() * get_directx_view_matrix(), m_coordSystem);
}

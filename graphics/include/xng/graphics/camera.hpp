#pragma once

#include <xng/core.hpp>
#include <xng/math.hpp>
#include <xng/geometry.hpp>

enum xng_coordinate_system
{
	XNG_COORDINATE_SYSTEM_OPENGL,
	XNG_COORDINATE_SYSTEM_DIRECTX
};

namespace xng
{
	namespace graphics
	{
		class camera
		{

		public:

			camera(void);

			void look_at(const math::float3 & eye, const math::float3 & up, const math::float3 & target);

			void move(const math::float3 & delta);

			const math::float3 & get_position(void) const;
			void set_position(const math::float3 & position);

			const math::quaternion & get_orientation(void) const;
			void set_orientation(const math::quaternion & orientation);

			float get_fovy(void) const;
			void set_fovy(float fovy);

			float get_fovx(void) const;
			void set_fovx(float fovx);

			float get_aspect_ratio(void) const;
			void set_aspect_ratio(float ratio);

			float get_znear(void) const;
			void set_znear(float znear);

			float get_zfar(void) const;
			void set_zfar(float zfar);

			xng_coordinate_system get_coordinate_system(void) const;
			void set_coordinate_system(xng_coordinate_system coordinateSystem);

			const math::float4x4 & get_view_matrix(void) const;
			const math::float4x4 & get_projection_matrix(void) const;

			const math::float3 forward(void);
			const math::float3 right(void);
			const math::float3 up(void);

		private:

			math::float3     m_position;
			math::quaternion m_orientation;
			
			float m_fovy;
			float m_ratio;
			float m_znear;
			float m_zfar;

			xng_coordinate_system m_coordSystem;

			mutable math::float4x4 m_viewMatrix;
			mutable math::float4x4 m_projectionMatrix;

			mutable bool m_viewMatrixDirty;
			mutable bool m_projectionMatrixDirty;

		};
	}
}
#pragma once

#include <xng/core.hpp>
#include <xng/math.hpp>
#include <xng/geometry.hpp>

namespace xng
{
    namespace graphics
    {
        class camera
        {

        public:

            camera(void);
            camera(const camera &) = default;
            camera(camera &&) = default;

            camera & operator= (const camera &) = default;
            camera & operator= (camera &&) = default;

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

            const math::float4x4 & get_view_matrix(void) const;
            const math::float4x4 & get_projection_matrix(void) const;
            const math::float4x4 & get_gl_view_matrix(void) const;
            const math::float4x4 & get_directx_view_matrix(void) const;
            const math::float4x4 & get_gl_projection_matrix(void) const;
            const math::float4x4 & get_directx_projection_matrix(void) const;

            const math::float3 forward(void);
            const math::float3 back(void);
            const math::float3 left(void);
            const math::float3 right(void);
            const math::float3 up(void);
            const math::float3 down(void);

            math::frustum get_frustum(void) const;

        private:

            math::float3     m_position;
            math::quaternion m_orientation;
            
            float m_fovy;
            float m_ratio;
            float m_znear;
            float m_zfar;

            mutable xng_coordinate_system m_coordSystem;

            mutable math::float4x4 m_viewMatrix;
            mutable math::float4x4 m_projectionMatrix;

            mutable bool m_viewMatrixDirty;
            mutable bool m_projectionMatrixDirty;

        };
    }
}
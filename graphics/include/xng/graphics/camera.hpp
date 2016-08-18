#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace xng
{
	namespace graphics
	{
		class camera
		{

		public:

			camera(void);

			void look_at(const glm::vec3 & eye, const glm::vec3 & up, const glm::vec3 & target);

			void move(const glm::vec3 & delta);

			const glm::vec3 & get_position(void) const;
			void set_position(const glm::vec3 & position);

			const glm::quat & get_orientation(void) const;
			void set_orientation(const glm::quat & orientation);

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

			const glm::mat4 & get_view_matrix(void) const;
			const glm::mat4 & get_projection_matrix(void) const;

			const glm::vec3 forward(void);
			const glm::vec3 right(void);
			const glm::vec3 up(void);

		private:

			glm::vec3 m_position;
			glm::quat m_orientation;
			
			float m_fovy;
			float m_ratio;
			float m_znear;
			float m_zfar;

			mutable glm::mat4 m_viewMatrix;
			mutable glm::mat4 m_projectionMatrix;

			mutable bool m_viewMatrixDirty;
			mutable bool m_projectionMatrixDirty;

		};
	}
}
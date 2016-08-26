#pragma once

#include <xng/math.hpp>
#include <xng/geometry.hpp>

namespace xng
{
	namespace math
	{
		template <typename T>
		class transform_hierarchy
		{

		public:

			transform_hierarchy(void) :
				m_globalMatrixUptodate(false),
				m_globalTranslationRotationMatrixUptodate(false),
				m_localMatricesUptodate(false),
				m_globalSRTUptodate(false),
				m_localRotation(1, 0, 0, 0),
				m_localScale(1),
				m_localTranslation(0),
				m_localMatrix(1),
				m_globalMatrix(1),
				m_globalScale(1),
				m_globalRotation(1, 0, 0, 0),
				m_globalTranslation(0),
				m_globalTranslationRotationMatrix(1),
				m_moved(true) {}

		public:

			XNG_INLINE const float4x4 & get_global_matrix(void);
			XNG_INLINE const float4x4 & get_local_matrix(void);
			XNG_INLINE const float4x4 & get_local_translation_rotation_matrix(void);

			XNG_INLINE void set_local_translation(const float3 & t);
			XNG_INLINE void set_local_rotation(const quaternion & q);
			XNG_INLINE void set_local_scale(const float3 & s);

			XNG_INLINE float3     get_local_translation(void) const;
			XNG_INLINE quaternion get_local_rotation(void) const;
			XNG_INLINE float3     get_local_scale(void) const;

			XNG_INLINE float3     get_global_translation(void);
			XNG_INLINE quaternion get_global_rotation(void);
			XNG_INLINE float3     get_global_scale(void);

		protected:

			XNG_INLINE const float4x4 & get_global_translation_rotation_matrix(void);

			XNG_INLINE void notify_local_change(void);
			XNG_INLINE void notify_global_change(void);

			XNG_INLINE bool is_global_matrix_uptodate(void) const;
			XNG_INLINE bool is_local_matrix_uptodate(void) const;

			XNG_INLINE bool was_moved(void) const;
			XNG_INLINE void set_moved(bool moved);

			XNG_INLINE bool update_local_matrices(void);
			XNG_INLINE bool update_global_matrix(void);
			XNG_INLINE bool update_global_rotation_translation_matrix(void);

			XNG_INLINE static void extract_srt(const float4x4 & SRT, const float4x4 & RT, float3 * scale, quaternion * rotation, float3 * translation);

			XNG_INLINE bool update_global_srt(void);
			XNG_INLINE void switch_parent(transform_hierarchy * parent);

			XNG_INLINE const float4x4 & get_global_matrix_internal(void) const;
			XNG_INLINE const float4x4 & get_global_translation_rotation_matrix_internal(void) const;
			XNG_INLINE const float4x4 & get_local_matrix_internal(void) const;
			XNG_INLINE const float4x4 & get_local_translation_rotation_matrix_internal(void) const;
			
		private:

			float4x4   m_globalMatrix;
			float4x4   m_globalTranslationRotationMatrix;
			float4x4   m_localMatrix;
			float4x4   m_localTranslationRotationMatrix;

			float3     m_localTranslation;
			quaternion m_localRotation;
			float3     m_localScale;

			float3     m_globalTranslation;
			quaternion m_globalRotation;
			float3     m_globalScale;

			bool m_globalMatrixUptodate;
			bool m_localMatricesUptodate;
			bool m_globalTranslationRotationMatrixUptodate;
			bool m_globalSRTUptodate;
			bool m_moved;

		};
	}
}

#include "transform_hierarchy.inl"
namespace xng
{
	namespace math
	{
		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_global_matrix(void)
		{
			update_global_matrix();
			return m_globalMatrix;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_local_matrix(void)
		{
			update_local_matrices();
			return m_localMatrix;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_local_translation_rotation_matrix(void)
		{
			update_local_matrices();
			return m_localTranslationRotationMatrix;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_local_translation(const float3 & t)
		{
			m_localTranslation = t;
			notify_local_change();
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_local_rotation(const quaternion & r)
		{
			m_localRotation = r;
			notify_local_change();
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_local_scale(const float3 & s)
		{
			m_localScale = s;
			notify_local_change();
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_local_matrix(const float4x4 & m)
		{
			decompose_affine(m, &m_localScale, &m_localRotation, m_localTranslation);
			notify_local_change();
		}

		template <typename T>
		XNG_INLINE float3 transform_hierarchy<T>::get_local_translation(void) const
		{
			return m_localTranslation;
		}

		template <typename T>
		XNG_INLINE quaternion transform_hierarchy<T>::get_local_rotation(void) const
		{
			return m_localRotation;
		}

		template <typename T>
		XNG_INLINE float3 transform_hierarchy<T>::get_local_scale(void) const
		{
			return m_localScale;
		}

		template <typename T>
		XNG_INLINE float3 transform_hierarchy<T>::get_global_translation(void)
		{
			update_global_srt();
			return m_globalTranslation;
		}

		template <typename T>
		XNG_INLINE quaternion transform_hierarchy<T>::get_global_rotation(void)
		{
			update_global_srt();
			return m_globalRotation;
		}

		template <typename T>
		XNG_INLINE float3 transform_hierarchy<T>::get_global_scale(void)
		{
			update_global_srt();
			return m_globalScale;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_global_translation_rotation_matrix(void)
		{
			update_global_rotation_translation_matrix();
			return m_globalTranslationRotationMatrix;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::notify_local_change(void)
		{
			m_localMatricesUptodate = false;
			m_moved = false;
			notify_global_change();
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::notify_global_change(void)
		{
			if (!m_moved)
			{
				m_moved                                   = true;
				m_globalMatrixUptodate                    = false;
				m_globalTranslationRotationMatrixUptodate = false;
				m_globalSRTUptodate                       = false;

				for (auto it = ((T*)this)->get_transform_hierarchy_children_begin(); it != ((T*)this)->get_transform_hierarchy_children_end(); ++it)
				{
					(*it)->notify_global_change();
				}
			}
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::is_global_matrix_uptodate(void) const
		{
			return m_globalMatrixUptodate;
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::is_local_matrix_uptodate(void) const
		{
			return m_localMatricesUptodate;
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::was_moved(void) const
		{
			return m_moved;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_moved(bool moved)
		{
			m_moved = moved;
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::update_local_matrices(void)
		{
			bool update = !m_localMatricesUptodate;

			if (update)
			{
				m_localTranslationRotationMatrix = to_translation4(m_localTranslation) * to_rotation4(m_localRotation);
				m_localMatrix                    = m_localTranslationRotationMatrix * to_scale4(m_localScale);
				m_localMatricesUptodate          = true;
			}

			return update;
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::update_global_matrix(void)
		{
			bool update = !m_globalMatrixUptodate;
			if (update)
			{
				auto parent = ((T*)this)->get_transform_hierarchy_parent();
				const float4x4 & local = get_local_matrix();
				m_globalMatrix = parent ? parent->get_global_matrix() * local : local;
				m_globalMatrixUptodate = true;
			}
			return update;
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::update_global_rotation_translation_matrix(void)
		{
			bool update = !m_globalTranslationRotationMatrixUptodate;

			if (update)
			{
				auto parent = ((T*)this)->get_transform_hierarchy_parent();
				const float4x4 & local = get_local_translation_rotation_matrix();
				m_globalTranslationRotationMatrix = parent ? parent->get_global_translation_rotation_matrix() * local : local;
				m_globalTranslationRotationMatrixUptodate = true;
			}

			return update;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::extract_srt(const float4x4 & SRT, const float4x4 & RT, float3 * scale, quaternion * rotation, float3 * translation)
		{
			*translation = float3(RT._03, RT._13, RT._23);

			*rotation = to_quaternion(RT);

			float3x3 ss = to_float3x3(SRT) * to_rotation3(inverse(*rotation));
			*scale = float3(ss._00, ss._11, ss._22);
		}

		template <typename T>
		XNG_INLINE bool transform_hierarchy<T>::update_global_srt(void)
		{
			bool update = !m_globalSRTUptodate;

			if (update)
			{
				extract_srt(get_global_matrix(), get_global_translation_rotation_matrix(), &m_globalScale, &m_globalRotation, &m_globalTranslation);
				m_globalSRTUptodate = true;
			}

			return update;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::switch_parent(transform_hierarchy * parent)
		{
			float4x4 invParentSRT = parent ? inverse(parent->get_global_matrix()) : float4x4(1);
			float4x4 invParentRT  = parent ? inverse(parent->get_global_translation_rotation_matrix()) : float4x4(1);

			float4x4 newSRT = invParentSRT * get_global_matrix();
			float4x4 newRT  = invParentRT * get_global_translation_rotation_matrix();

			extract_srt(newSRT, newRT, &m_localScale, &m_localRotation, &m_localTranslation);

			((T*)this)->set_transform_hierarchy_parent((T*)parent);
			notify_local_change();
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_global_matrix_internal(void) const
		{
			return m_globalMatrix;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_global_translation_rotation_matrix_internal(void) const
		{
			return m_globalTranslationRotationMatrix;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_local_matrix_internal(void) const
		{
			return m_localMatrix;
		}

		template <typename T>
		XNG_INLINE const float4x4 & transform_hierarchy<T>::get_local_translation_rotation_matrix_internal(void) const
		{
			return m_localTranslationRotationMatrix;
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_global_translation(const float3 & t)
		{
			transform_hierarchy<T> * parent = ((T*)this)->get_transform_hierarchy_parent();
			float3 globalTranslation = parent ? parent->get_global_translation() : float3(0);
			set_local_translation(t - globalTranslation);
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_global_rotation(const quaternion & r)
		{
			transform_hierarchy<T> * parent = ((T*)this)->get_transform_hierarchy_parent();
			quaternion globalRotation = parent ? parent->get_global_rotation() : quaternion(1, 0, 0, 0);
			set_local_rotation(r * inverse(globalRotation));
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_global_scale(const float3 & s)
		{
			transform_hierarchy<T> * parent = ((T*)this)->get_transform_hierarchy_parent();
			float3 globalScale = parent ? parent->get_global_scale() : float3(1);
			set_local_scale(s / globalScale);
		}

		template <typename T>
		XNG_INLINE void transform_hierarchy<T>::set_global_matrix(const float4x4 & m)
		{
			transform_hierarchy<T> * parent = ((T*)this)->get_transform_hierarchy_parent();
			const float4x4 & globalMatrix = parent ? get_global_matrix() : float4x4(1);
			set_local_matrix(inverse(globalMatrix) * m);
		}

	}
}
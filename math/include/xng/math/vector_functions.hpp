#pragma once

namespace xng
{
    namespace math
    {
        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, typename VectorType::scalar> dot(const VectorType & a, const VectorType & b)
        {
            return detail::vector_dot_impl<VectorType, VectorType::size::value>::dot(a, b);
        }

        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, float> length(const VectorType & a)
        {
            return detail::vector_length_impl<VectorType, VectorType::size::value>::length(a);
        }

        template <typename VectorType>
        VectorType normalize(const VectorType & a)
        {
            return a / length(a);
        }

        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, VectorType> min(const VectorType & a, const VectorType & b)
        {
            return detail::vector_min_max_impl<VectorType, VectorType::size::value>::min(a, b);
        }

        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, VectorType> max(const VectorType & a, const VectorType & b)
        {
            return detail::vector_min_max_impl<VectorType, VectorType::size::value>::max(a, b);
        }

        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, VectorType> inverse_square_root(const VectorType & a)
        {
            return detail::vector_square_root_impl<VectorType, VectorType::size::value>::inverse_square_root(a);
        }

        template <typename VectorType>
        std::enable_if_t<detail::is_vector<VectorType>::value, VectorType> square_root(const VectorType & a)
        {
            return detail::vector_square_root_impl<VectorType, VectorType::size::value>::square_root(a);
        }

    }
}
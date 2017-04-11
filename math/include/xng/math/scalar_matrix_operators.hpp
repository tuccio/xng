//#pragma once
//
//namespace fuse
//{
//
//    /* Binary operators among matrices */
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator+ (const VectorType & a, const VectorType & b)
//    {
//        return add(a, b);
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator- (const VectorType & a, const VectorType & b)
//    {
//        return subtract(a, b);
//    }
//
//    /* Binary operators among matrices/scalars */
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator+ (const VectorType & a, typename VectorType::scalar b)
//    {
//        return add(a, VectorType(b, b));
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator+ (typename VectorType::scalar a, const VectorType & b)
//    {
//        return add(VectorType(a, a), b);
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator- (const VectorType & a, typename VectorType::scalar b)
//    {
//        return subtract(a, VectorType(b, b));
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator- (typename VectorType::scalar a, const VectorType & b)
//    {
//        return subtract(VectorType(a, a), b);
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator* (const VectorType & a, typename VectorType::scalar b)
//    {
//        return scale(a, VectorType(b, b));
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator* (typename VectorType::scalar a, const VectorType & b)
//    {
//        return scale(VectorType(a, a), b);
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_float_matrix<VectorType>::value, VectorType>
//        operator/ (const VectorType & a, typename VectorType::scalar b)
//    {
//        typename VectorType::scalar s = 1.f / b;
//        return scale(a, VectorType(s, s));
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_float_matrix<VectorType>::value, VectorType>
//        operator/ (typename VectorType::scalar a, const VectorType & b)
//    {
//        return divide(VectorType(a, a), b);
//    }
//
//    /* Unary operators */
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator- (const VectorType & a)
//    {
//        return negate(a);
//    }
//
//    /* Comparison operators */
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, bool>
//        operator== (const VectorType & a, const VectorType & b)
//    {
//        return equals(a, b);
//    }
//
//    /* Matrix multiplication */
//
//    namespace detail
//    {
//
//        template <typename VectorType, int I, int J, int K>
//        struct row_column_multiplier
//        {
//            XNG_INLINE static void multiply(typename VectorType::scalar & r, const VectorType & a, const VectorType & b)
//            {
//                row_column_multiplier<VectorType, I, J, K - 1>::multiply(r, a, b);
//                r += get<I, K - 1>(a) * get<K - 1, J>(b);
//            }
//        };
//
//        template <typename VectorType, int I, int J>
//        struct row_column_multiplier<VectorType, I, J, 0>
//        {
//            XNG_INLINE static void multiply(typename VectorType::scalar & r, const VectorType & a, const VectorType & b)
//            {
//                // Initialize to 0, then sum a(i, k) * a(k, j) for each k
//                r = 0;
//            }
//        };
//
//        template <typename VectorType, int I, int J>
//        struct row_column_operator
//        {
//            XNG_INLINE static void multiply(VectorType & r, const VectorType & a, const VectorType & b)
//            {
//                row_column_operator<VectorType, I, J - 1>::multiply(r, a, b);
//                //std::cout << "R/C:" << I << ", " << J << std::endl;
//                row_column_multiplier<VectorType, I - 1, J - 1, VectorType::rows::value>::multiply(get<I - 1, J - 1>(r), a, b);
//            }
//        };
//
//        template <typename VectorType, int I>
//        struct row_column_operator<VectorType, I, 0>
//        {
//            XNG_INLINE static void multiply(VectorType & r, const VectorType & a, const VectorType & b)
//            {
//                row_column_operator<VectorType, I - 1, VectorType::cols::value>::multiply(r, a, b);
//                //std::cout << "R/C:" << I << ", " << 0 << " (Do nothing)" << std::endl;
//            }
//        };
//
//        template <typename VectorType, int J>
//        struct row_column_operator<VectorType, 0, J>
//        {
//            XNG_INLINE static void multiply(VectorType & r, const VectorType & a, const VectorType & b)
//            {
//                //std::cout << "R/C:" << 0 << ", " << J << " (Do nothing)" << std::endl;
//            }
//        };
//
//    }
//
//    template <typename VectorType>
//    XNG_INLINE VectorType multiply(const VectorType & a, const VectorType & b)
//    {
//        VectorType r;
//        detail::row_column_operator<VectorType, VectorType::rows::value, VectorType::cols::value>::multiply(r, a, b);
//        return r;
//    }
//
//    template <typename VectorType>
//    XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, VectorType>
//        operator* (const VectorType & a, const VectorType & b)
//    {
//        return multiply(a, b);
//    }
//
//}
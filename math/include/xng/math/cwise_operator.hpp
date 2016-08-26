//#pragma once
//
//namespace fuse
//{
//
//	// get
//
//	template <int N, typename VectorType>
//	XNG_INLINE std::enable_if_t<is_scalar_vector<VectorType>::value, typename VectorType::scalar &> get(VectorType & x)
//	{
//		return x.m[N];
//	}
//
//	template <int N, typename VectorType>
//	XNG_INLINE std::enable_if_t<is_scalar_vector<VectorType>::value, const typename VectorType::scalar &> get(const VectorType & x)
//	{
//		return x.m[N];
//	}
//
//	template <int N, int M, typename VectorType>
//	XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, typename VectorType::scalar &> get(VectorType & x)
//	{
//		return x.m[VectorType::rows::value * M + N];
//	}
//
//	template <int N, int M, typename VectorType>
//	XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, const typename VectorType::scalar &> get(const VectorType & x)
//	{
//		return x.m[VectorType::rows::value * M + N];
//	}
//
//	// operators
//
//	namespace detail
//	{
//
//		template <int N, typename VectorType>
//		XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, typename VectorType::scalar &> get(VectorType & x)
//		{
//			return x.m[N];
//		}
//
//		template <int N, typename VectorType>
//		XNG_INLINE std::enable_if_t<is_scalar_matrix<VectorType>::value, const typename VectorType::scalar &> get(const VectorType & x)
//		{
//			return x.m[N];
//		}
//
//		template <typename VectorType, int N = VectorType::size::value>
//		struct cwise_operator
//		{
//
//			XNG_INLINE static void add(VectorType & r, const VectorType & a, const VectorType & b)
//			{
//				cwise_operator<VectorType, N - 1>::add(r, a, b);
//				get<N - 1>(r) = get<N - 1>(a) + get<N - 1>(b);
//			}
//
//			XNG_INLINE static void subtract(VectorType & r, const VectorType & a, const VectorType & b)
//			{
//				cwise_operator<VectorType, N - 1>::subtract(r, a, b);
//				get<N - 1>(r) = get<N - 1>(a) - get<N - 1>(b);
//			}
//
//			XNG_INLINE static void multiply(VectorType & r, const VectorType & a, const VectorType & b)
//			{
//				cwise_operator<VectorType, N - 1>::multiply(r, a, b);
//				get<N - 1>(r) = get<N - 1>(a) * get<N - 1>(b);
//			}
//
//			XNG_INLINE static void divide(VectorType & r, const VectorType & a, const VectorType & b)
//			{
//				cwise_operator<VectorType, N - 1>::divide(r, a, b);
//				get<N - 1>(r) = get<N - 1>(a) / get<N - 1>(b);
//			}
//
//			XNG_INLINE static void negate(VectorType & r, const VectorType & a)
//			{
//				cwise_operator<VectorType, N - 1>::negate(r, a);
//				get<N - 1>(r) = -get<N - 1>(a);
//			}
//
//			XNG_INLINE static void hadd(typename VectorType::scalar & r, const VectorType & a)
//			{
//				cwise_operator<VectorType, N - 1>::hadd(r, a);
//				r += get<N - 1>(a);
//			}
//
//			XNG_INLINE static void absolute(VectorType & r, const VectorType & a)
//			{
//				cwise_operator<VectorType, N - 1>::absolute(r, a);
//				get<N - 1>(r) = xng::absolute(get<N - 1>(a));
//			}
//
//		};
//
//		template <typename VectorType>
//		struct cwise_operator<VectorType, 0>
//		{
//			XNG_INLINE static void add(VectorType & r, const VectorType & a, const VectorType & b) {}
//			XNG_INLINE static void subtract(VectorType & r, const VectorType & a, const VectorType & b) {}
//			XNG_INLINE static void multiply(VectorType & r, const VectorType & a, const VectorType & b) {}
//			XNG_INLINE static void divide(VectorType & r, const VectorType & a, const VectorType & b) {}
//			XNG_INLINE static void negate(VectorType & r, const VectorType & a) {}
//			XNG_INLINE static void absolute(VectorType & r, const VectorType & a) {}
//		};
//
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType add(const VectorType & a, const VectorType & b)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::add(r, a, b);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType subtract(const VectorType & a, const VectorType & b)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::subtract(r, a, b);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType scale(const VectorType & a, const VectorType & b)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::multiply(r, a, b);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType divide(const VectorType & a, const VectorType & b)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::divide(r, a, b);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType negate(const VectorType & a)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::negate(r, a);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE VectorType absolute(const VectorType & a)
//	{
//		VectorType r;
//		detail::cwise_operator<VectorType>::absolute(r, a);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE typename VectorType::scalar hadd(const VectorType & a)
//	{
//		VectorType::scalar r = 0;
//		detail::cwise_operator<VectorType>::hadd(r, a);
//		return r;
//	}
//
//	template <typename VectorType>
//	XNG_INLINE bool equals(typename VectorType::scalar a, const VectorType & b)
//	{
//		assert(sizeof(VectorType) == sizeof(VectorType::scalar) * sizeof(VectorType::size::value) &&
//			"Padding detected, can't use memcmp to compare objects.");
//		return memcmp(&a, &b, sizeof(VectorType)) == 0;
//	}
//
//}
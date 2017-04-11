#pragma once

namespace fuse
{

    namespace detail
    {

        template <int N>
        struct newton_raphson_simd
        {

            static XNG_INLINE vec128 XNG_VECTOR_CALL square_root_ps(vec128 x)
            {
                return _mm_mul_ps(x, reciprocal_square_root_ps(x));
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL square_root_ss(vec128 x)
            {
                return _mm_mul_ss(x, reciprocal_square_root_ss(x));
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_ps(vec128 x)
            {
                return reciprocal_ps_step(x, _mm_rcp_ps(x));
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_ss(vec128 x)
            {
                return reciprocal_ss_step(x, _mm_rcp_ss(x));
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ps(vec128 x)
            {
                return reciprocal_square_root_ps_step(x, _mm_rsqrt_ps(x));
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ss(vec128 x)
            {
                return reciprocal_square_root_ss_step(x, _mm_rsqrt_ss(x));
            }

        private:

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_ps_step(vec128 x, vec128 yi)
            {
                vec128 t0 = _mm_mul_ps(_mm_mul_ps(x, yi), yi);
                vec128 t1 = _mm_add_ps(yi, yi);
                return _mm_sub_ps(t1, t0);
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_ss_step(vec128 x, vec128 yi)
            {
                vec128 t0 = _mm_mul_ss(_mm_mul_ss(x, yi), yi);
                vec128 t1 = _mm_add_ss(yi, yi);
                return _mm_sub_ss(t1, t0);
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ps_step(vec128 x, vec128 yi)
            {
                vec128 t0 = _mm_mul_ps(_mm_mul_ps(x, yi), yi);
                vec128 yn = _mm_mul_ps(_mm_sub_ps(_mm_set1_ps(3.0f), t0), _mm_mul_ps(_mm_set1_ps(0.5f), yi));
                return newton_raphson_simd<N - 1>::reciprocal_square_root_ss_step(x, yn);
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ss_step(vec128 x, vec128 yi)
            {
                vec128 t0 = _mm_mul_ss(_mm_mul_ss(x, yi), yi);
                vec128 yn = _mm_mul_ss(_mm_sub_ss(_mm_set_ss(3.0f), t0), _mm_mul_ss(_mm_set_ss(0.5f), yi));
                return newton_raphson_simd<N - 1>::reciprocal_square_root_ss_step(x, yn);
            }

            template <int K> friend struct newton_raphson_simd;

        };

        template <>
        struct newton_raphson_simd<0>
        {

        private:

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ps_step(vec128 x, vec128 yi)
            {
                return yi;
            }

            static XNG_INLINE vec128 XNG_VECTOR_CALL reciprocal_square_root_ss_step(vec128 x, vec128 yi)
            {
                return yi;
            }

            template <int K> friend struct newton_raphson_simd;

        };

    }

    

}
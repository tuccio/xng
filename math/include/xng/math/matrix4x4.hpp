#pragma once

#include "forward.hpp"
#include "matrix.hpp"
#include "matrix3x3.hpp"

namespace xng
{
    namespace math
    {
        template <typename T>
        struct matrix<T, 4, 4> :
            matrix_traits<T, 4, 4>
        {

            matrix(void) = default;
            matrix(const matrix &) = default;

            matrix(
                scalar m00, scalar m01, scalar m02, scalar m03,
                scalar m10, scalar m11, scalar m12, scalar m13,
                scalar m20, scalar m21, scalar m22, scalar m23,
                scalar m30, scalar m31, scalar m32, scalar m33) :
                _00(m00), _10(m10), _20(m20), _30(m30),
                _01(m01), _11(m11), _21(m21), _31(m31),
                _02(m02), _12(m12), _22(m22), _32(m32),
                _03(m03), _13(m13), _23(m23), _33(m33) {}

            matrix(T d) :
                matrix(
                    d, 0, 0, 0,
                    0, d, 0, 0,
                    0, 0, d, 0,
                    0, 0, 0, d) {}

            matrix(const matrix<T, 3, 3> & m) :
                _00(m._00), _10(m._10), _20(m._20), _30(T(0)),
                _01(m._01), _11(m._11), _21(m._21), _31(T(0)),
                _02(m._02), _12(m._12), _22(m._22), _32(T(0)),
                _03(T(0)), _13(T(0)), _23(T(0)), _33(T(1)) {}

            union
            {

                struct
                {
                    matrix<T, 1, 4> r[4];
                };

                scalar m[16];

                struct
                {
                    scalar _00;
                    scalar _01;
                    scalar _02;
                    scalar _03;

                    scalar _10;
                    scalar _11;
                    scalar _12;
                    scalar _13;

                    scalar _20;
                    scalar _21;
                    scalar _22;
                    scalar _23;

                    scalar _30;
                    scalar _31;
                    scalar _32;
                    scalar _33;
                };
            };
        };
    }
}
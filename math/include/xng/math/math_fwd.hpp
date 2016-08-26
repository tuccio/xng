#pragma once

namespace xng
{
	namespace math
	{
		template <typename T, int N, int M> struct matrix;

		typedef matrix<float, 2, 2> float2x2;
		typedef matrix<float, 3, 3> float3x3;
		typedef matrix<float, 4, 4> float4x4;

		typedef matrix<float, 1, 2> float2row;
		typedef matrix<float, 1, 3> float3row;
		typedef matrix<float, 1, 4> float4row;

		typedef matrix<double, 1, 2> double2row;
		typedef matrix<double, 1, 3> double3row;
		typedef matrix<double, 1, 4> double4row;

		typedef matrix<int, 1, 2> int2row;
		typedef matrix<int, 1, 3> int3row;
		typedef matrix<int, 1, 4> int4row;

		typedef matrix<unsigned int, 1, 2> uint2row;
		typedef matrix<unsigned int, 1, 3> uint3row;
		typedef matrix<unsigned int, 1, 4> uint4row;

		typedef matrix<float, 2, 1> float2col;
		typedef matrix<float, 3, 1> float3col;
		typedef matrix<float, 4, 1> float4col;

		typedef matrix<double, 2, 1> double2col;
		typedef matrix<double, 3, 1> double3col;
		typedef matrix<double, 4, 1> double4col;

		typedef matrix<int, 2, 1> int2col;
		typedef matrix<int, 3, 1> int3col;
		typedef matrix<int, 4, 1> int4col;

		typedef matrix<unsigned int, 2, 1> uint2col;
		typedef matrix<unsigned int, 3, 1> uint3col;
		typedef matrix<unsigned int, 4, 1> uint4col;

		typedef float2col float2;
		typedef float3col float3;
		typedef float4col float4;

		typedef double2col double2;
		typedef double3col double3;
		typedef double4col double4;

		typedef int2col int2;
		typedef int3col int3;
		typedef int4col int4;

		typedef uint2col uint2;
		typedef uint3col uint3;
		typedef uint4col uint4;
	}
}
#include <xng/math.hpp>
#include <xng/geometry.hpp>
#include <xng/math/to_string.hpp>

#include <Eigen/Eigen>

#include <DirectXMath.h>

#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <tuple>

using namespace xng;
using namespace xng::math;

static std::ofstream g_log;

std::ostream & operator<< (std::ostream & os, const Eigen::Quaternionf & q)
{
	quaternion r(q.w(), q.x(), q.y(), q.z());
	return os << r;
}

void test_print_all(std::ostream & os) {}

template <typename Arg1, typename ... Args>
void test_print_all(std::ostream & os, Arg1 && arg1, Args && ... args)
{
	os << "\t" << arg1 << std::endl;
	test_print_all(os, args ...);
}

#define TEST_SUCCESS_LOG(OutputStream)\
{\
	OutputStream << "Test " << __FUNCSIG__ << " OK" << std::endl;\
}

#define TEST_FAIL_LOG(OutputStream, Iteration, ...)\
{\
	OutputStream << "Test failed at " << __FUNCSIG__ << "@" << __FILE__ << ":" << __LINE__ << " Iteration #" << Iteration << std::endl;\
	test_print_all(OutputStream, __VA_ARGS__);\
}

#define TEST_MIN_FLOAT 0
#define TEST_MAX_FLOAT 10000

bool test_eigen_equals(float a, float b, float tolerance)
{
	float err = std::abs(a - b);
	float tol = std::abs(tolerance * b);
	return err <= tol;
}

template <typename T, int N, int M>
bool test_eigen_equals(const matrix<T, N, M> & m, const Eigen::Matrix<T, N, M> & e, T tolerance)
{
	for (int j = 0; j < M; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			if (!test_eigen_equals(m(i, j), e(i, j), tolerance))
			{
				return false;
			}
		}
	}
	return true;
}

bool test_eigen_equals(const quaternion & q, const Eigen::Quaternionf & e, float tolerance)
{
	return (test_eigen_equals(q.w, e.w(), tolerance) &&
		test_eigen_equals(q.x, e.x(), tolerance) &&
		test_eigen_equals(q.y, e.y(), tolerance) &&
		test_eigen_equals(q.z, e.z(), tolerance));
}

template <typename T, int N, int M>
void test_eigen_load(const matrix<T, N, M> & m, Eigen::Matrix<T, N, M> & e)
{
	for (int j = 0; j < M; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			e(i, j) = m(i, j);
		}
	}
}

void test_eigen_load(const quaternion & q, Eigen::Quaternionf & e)
{
	e.w() = q.w;
	e.x() = q.x;
	e.y() = q.y;
	e.z() = q.z;
}

template <typename T, int N, int M>
bool test_eigen_add(
	const matrix<T, N, M> & a,
	const matrix<T, N, M> & b,
	matrix<T, N, M> & r,
	Eigen::Matrix<T, N, M> & e,
	T tolerance)
{
	Eigen::Matrix<T, N, M> e1, e2;
	test_eigen_load(a, e1);
	test_eigen_load(b, e2);
	r = a + b;
	e = e1 + e2;
	return test_eigen_equals(r, e, tolerance);
}

template <typename T, int N, int M>
bool test_eigen_sub(
	const matrix<T, N, M> & a,
	const matrix<T, N, M> & b,
	matrix<T, N, M> & r,
	Eigen::Matrix<T, N, M> & e,
	T tolerance)
{
	Eigen::Matrix<T, N, M> e1, e2;
	test_eigen_load(a, e1);
	test_eigen_load(b, e2);
	r = a - b;
	e = e1 - e2;
	return test_eigen_equals(r, e, tolerance);
}

template <typename T, int N, int M>
bool test_eigen_transpose(
	const matrix<T, N, M> & a,
	matrix<T, M, N> & r,
	Eigen::Matrix<T, M, N> & e,
	T tolerance)
{
	Eigen::Matrix<T, N, M> e1;
	test_eigen_load<T, N, M>(a, e1);
	r = transpose(a);
	e = e1.transpose();
	return test_eigen_equals<T, M, N>(r, e, tolerance);
}

template <typename T, int N, int M, int K>
bool test_eigen_multiply(
	const matrix<T, N, K> & a,
	const matrix<T, K, M> & b,
	matrix<T, N, M> & r,
	Eigen::Matrix<T, N, M> & e,
	T tolerance)
{
	Eigen::Matrix<T, N, K> e1;
	Eigen::Matrix<T, K, M> e2;
	test_eigen_load<T, N, K>(a, e1);
	test_eigen_load<T, K, M>(b, e2);
	r = a * b;
	e = e1 * e2;
	return test_eigen_equals<T, N, M>(r, e, tolerance);
}

template <typename T, int N, int M>
bool test_eigen_dot(
	const matrix<T, N, M> & a,
	const matrix<T, N, M> & b,
	float & r,
	float & e,
	T tolerance)
{
	Eigen::Matrix<T, N, M> e1, e2;
	test_eigen_load(a, e1);
	test_eigen_load(b, e2);
	r = dot(a, b);
	e = e1.dot(e2);
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_det3(
	const float3x3 & a,
	float & r,
	float & e,
	float tolerance)
{
	Eigen::Matrix<float, 3, 3> e1;
	test_eigen_load(a, e1);
	r = determinant(a);
	e = e1.determinant();
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_det4(
	const float4x4 & a,
	float & r,
	float & e,
	float tolerance)
{
	Eigen::Matrix<float, 4, 4> e1;
	test_eigen_load(a, e1);
	r = determinant(a);
	e = e1.determinant();
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_inv3(
	const float3x3 & a,
	float3x3 & r,
	Eigen::Matrix<float, 3, 3> & e,
	float tolerance)
{
	Eigen::Matrix<float, 3, 3> e1;
	test_eigen_load(a, e1);
	r = inverse(a);
	e = e1.inverse();
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_inv4(
	const float4x4 & a,
	float4x4 & r,
	Eigen::Matrix<float, 4, 4> & e,
	float tolerance)
{
	Eigen::Matrix<float, 4, 4> e1;
	test_eigen_load(a, e1);
	r = inverse(a);
	e = e1.inverse();
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_quaternion_mul(
	const quaternion & a,
	const quaternion & b,
	quaternion & r,
	Eigen::Quaternionf & e,
	float tolerance)
{
	Eigen::Quaternionf e1, e2;
	test_eigen_load(a, e1);
	test_eigen_load(b, e2);
	r = b * a;
	e = e2 * e1;
	return test_eigen_equals(r, e, tolerance);
}

bool test_eigen_quaternion_transform(
	const quaternion & q,
	const float3 & x,
	float3 & r,
	Eigen::Matrix<float, 3, 1> & e,
	float tolerance)
{
	Eigen::Quaternionf e1;
	Eigen::Matrix<float, 3, 1> ex;
	test_eigen_load(q, e1);
	test_eigen_load(x, ex);
	r = transform(x, q);
	e = (Eigen::Matrix3f(e1) * ex);
	return test_eigen_equals(r, e, tolerance);
}

template <typename T, int N, int M>
bool test_eigen_length(
	const matrix<T, N, M> & a,
	float & r,
	float & e,
	T tolerance)
{
	Eigen::Matrix<T, N, M> e1;
	test_eigen_load(a, e1);
	r = length(a);
	e = e1.norm();
	return test_eigen_equals(r, e, tolerance);
}

template <typename Matrix, typename Distribution, typename Generator>
void test_load_random_matrix(Matrix & m, const Distribution & d, Generator & g)
{
	for (int i = 0; i < Matrix::size::value; ++i)
	{
		m[i] = d(g);
	}
}

template <typename Distribution, typename Generator>
void test_load_random_quaternion(quaternion & q, const Distribution & d, Generator & g)
{
	q.w = d(g);
	q.x = d(g);
	q.y = d(g);
	q.z = d(g);
}

template <typename T, int N, int M>
bool test_batch_eigen_add(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{

		matrix<T, N, M> a;
		matrix<T, N, M> b;

		test_load_random_matrix(a, dist, generator);
		test_load_random_matrix(b, dist, generator);

		matrix<T, N, M> r;
		Eigen::Matrix<T, N, M> e;
		
		if (!test_eigen_add(a, b, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

template <typename T, int N, int M>
bool test_batch_eigen_sub(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{

		matrix<T, N, M> a;
		matrix<T, N, M> b;

		test_load_random_matrix(a, dist, generator);
		test_load_random_matrix(b, dist, generator);

		matrix<T, N, M> r;
		Eigen::Matrix<T, N, M> e;

		if (!test_eigen_sub(a, b, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

template <typename T, int N, int K, int M>
bool test_batch_eigen_multiply(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		matrix<T, N, K> a;
		matrix<T, K, M> b;

		test_load_random_matrix(a, dist, generator);
		test_load_random_matrix(b, dist, generator);

		matrix<T, N, M> r;
		Eigen::Matrix<T, N, M> e;

		if (!test_eigen_multiply(a, b, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

template <typename T, int N, int M>
bool test_batch_eigen_dot(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{

		matrix<T, N, M> a;
		matrix<T, N, M> b;

		test_load_random_matrix(a, dist, generator);
		test_load_random_matrix(b, dist, generator);

		float r;
		float e;

		if (!test_eigen_dot(a, b, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

template <typename T, int N, int M>
bool test_batch_eigen_transpose(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		matrix<T, N, M> a;

		test_load_random_matrix(a, dist, generator);

		matrix<T, M, N> r;
		Eigen::Matrix<T, M, N> e;

		if (!test_eigen_transpose<T, N, M>(a, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_quaternion_mul(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{

		quaternion a;
		quaternion b;

		test_load_random_quaternion(a, dist, generator);
		test_load_random_quaternion(b, dist, generator);

		quaternion r;
		Eigen::Quaternionf e;

		if (!test_eigen_quaternion_mul(a, b, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, b, "Result:", r, "Eigen result:", e);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_quaternion_transform(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		quaternion q;
		float3 x;

		test_load_random_quaternion(q, dist, generator);
		test_load_random_matrix(x, dist, generator);

		q = normalize(q);

		float3 r;
		Eigen::Matrix<float, 3, 1> e;

		if (!test_eigen_quaternion_transform(q, x, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", q, x, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", q, x, "Result:", r, "Eigen result:", e);
			return false;
		}
	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_det3(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		float3x3 a;

		test_load_random_matrix(a, dist, generator);

		float r, e;

		if (!test_eigen_det3(a, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			return false;
		}
	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_det4(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		float4x4 a;

		test_load_random_matrix(a, dist, generator);

		float r, e;

		if (!test_eigen_det4(a, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			return false;
		}
	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_inv3(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{

		float3x3 a;

		test_load_random_matrix(a, dist, generator);

		float3x3 r;
		Eigen::Matrix<float, 3, 3> e;

		if (!test_eigen_inv3(a, r, e, tolerance))
		{
			float4x4 I = a * r;
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e, "Identity:", I);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e, "Identity:", I);
			return false;
		}

	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

bool test_batch_eigen_inv4(int iterations, float tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<float> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		float4x4 a;

		test_load_random_matrix(a, dist, generator);

		float4x4 r;
		Eigen::Matrix<float, 4, 4> e;

		if (!test_eigen_inv4(a, r, e, tolerance))
		{
			float4x4 I = a * r;
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e, "Identity:", I);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e, "Identity:", I);
			return false;
		}
	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

template <typename T, int N, int M>
bool test_batch_eigen_length(int iterations, T tolerance)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<T> dist(TEST_MIN_FLOAT, TEST_MAX_FLOAT);

	for (int i = 0; i < iterations; i++)
	{
		matrix<T, N, M> a;

		test_load_random_matrix(a, dist, generator);

		float r;
		float e;

		if (!test_eigen_length(a, r, e, tolerance))
		{
			TEST_FAIL_LOG(std::cout, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			TEST_FAIL_LOG(g_log, i, "Arguments:", a, "Result:", r, "Eigen result:", e);
			return false;
		}
	}

	TEST_SUCCESS_LOG(g_log);
	TEST_SUCCESS_LOG(std::cout);

	return true;
}

int main(int argc, char * argv[])
{
	const int   Iterations = 30;
	const float Tolerance  = 1e-05;
	const float Tolerance2 = 1e-03;

	g_log.open("math_test.log");

#include "test_batch_eigen_add.inl"
#include "test_batch_eigen_sub.inl"
#include "test_batch_eigen_multiply.inl"
#include "test_batch_eigen_transpose.inl"
#include "test_batch_eigen_dot.inl"
#include "test_batch_eigen_length.inl"

	test_batch_eigen_quaternion_mul(Iterations, Tolerance);
	test_batch_eigen_quaternion_transform(Iterations, Tolerance);

	test_batch_eigen_det3(Iterations, Tolerance2);
	test_batch_eigen_det4(Iterations, Tolerance2);

	test_batch_eigen_inv3(Iterations, Tolerance2);
	test_batch_eigen_inv4(Iterations, Tolerance2);
	
	// Decomposition

	using namespace DirectX;

	XMVECTOR originalScale       = XMVectorSet(5.f, -2.f, 3.f, 1.f);
	XMVECTOR originalRotation    = XMQuaternionRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 1.f), XM_PIDIV4);
	XMVECTOR originalTranslation = XMVectorSet(200, 105, -55, 1.f);

	XMVECTOR decomposedScale, decomposedRotation, decomposedTranslation;

	XMMATRIX dxm = XMMatrixAffineTransformation(originalScale, XMVectorZero(), originalRotation, originalTranslation);
	float4x4 m = reinterpret_cast<const float4x4&>((dxm));

	quaternion r;
	float3 s, t;

	decompose_affine(m, &s, &r, &t);
	XMMatrixDecompose(&decomposedScale, &decomposedRotation, &decomposedTranslation, dxm);

	TEST_FAIL_LOG(g_log, 1,
		"Original scale:", reinterpret_cast<const float3&>(originalScale),
		"Original rotation:", reinterpret_cast<const quaternion&>(originalRotation),
		"Original translation", reinterpret_cast<const float3&>(originalTranslation),
		"Decomposed scale:", s,
		"Decomposed rotation:", r,
		"Decomposed translation", t,
		"DX Decomposed scale:", reinterpret_cast<const float3&>(decomposedScale),
		"DX Decomposed rotation:", reinterpret_cast<const quaternion&>(decomposedRotation),
		"DX Decomposed translation:", reinterpret_cast<const float3&>(decomposedTranslation));

	float3     e1 = to_radians(float3(25.f, 59.f, 47.f));
	quaternion q1 = to_quaternion(e1);
	float3x3   m1 = to_rotation3(q1);
	quaternion q2 = to_quaternion(m1);

	TEST_FAIL_LOG(g_log, 1,
		"Original rotation:", q1,
		"Original euler:", to_degrees(e1),
		"Converted euler:", to_degrees(to_euler(q1)),
		"Matrix to euler:", to_degrees(to_euler(m1)),
		"Transformed rotation:", q2,
		"Transformed euler:", to_degrees(to_euler(q2)));

	return 0;
}
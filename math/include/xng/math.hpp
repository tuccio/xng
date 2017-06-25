#pragma once

#include <algorithm>
#include <cassert>
#include <type_traits>

#include "math/forward.hpp"
#include "math/utility.hpp"

/* Template matrices */

#include "math/matrix.hpp"

#include "math/math_type_traits.hpp"
#include "math/scalar_functions.hpp"

#include "math/cwise_operator.hpp"

#include "math/scalar_matrix_operators.hpp"
#include "math/scalar_matrix_functions.hpp"

#include "math/matrix_functions.hpp"
#include "math/matrix_operators.hpp"

#include "math/vector_functions.hpp"

#include "math/matrix_base_impl.inl"
#include "math/vector_base_impl.inl"

/* Specializations */

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include "math/matrix2x2.hpp"
#include "math/matrix3x3.hpp"
#include "math/matrix4x4.hpp"

#include "math/quaternion.hpp"

/* Implementations */

#include "math/vector3_impl.inl"
#include "math/vector4_impl.inl"

#include "math/matrix3x3_impl.inl"
#include "math/matrix4x4_impl.inl"

#include "math/quaternion_impl.inl"

#include "math/convertions.hpp"

#include "math/constants.hpp"
#include "math/trigonometry.hpp"

/* Utility */

#include "math/to_string.hpp"
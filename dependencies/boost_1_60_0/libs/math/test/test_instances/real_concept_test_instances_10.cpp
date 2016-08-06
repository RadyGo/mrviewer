// Copyright John Maddock 2011.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "pch.hpp"

#ifndef BOOST_BUILD_PCH_ENABLED
#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error
#include <boost/math/special_functions/jacobi_elliptic.hpp>
#endif

#include <boost/math/tools/config.hpp>
#include <boost/math/concepts/real_concept.hpp>

#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS

#define BOOST_MATH_TEST_TYPE boost::math::concepts::real_concept
#define TEST_GROUP_10
#include "test_instances.hpp"

#endif

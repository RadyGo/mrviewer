//  (C) Copyright John Maddock 2000. 
//  (C) Copyright Ion Gaztanaga 2014.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//#define TEST_STD
#include "test.hpp"
#include "check_integral_constant.hpp"

#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_copy_assignable.hpp>
#endif


#include <boost/move/core.hpp>

struct has {
    has(){}
    has &operator=(const has&){ return *this; }
};

// MSVC can not generate neither default constructor, nor assignment operator, 
// nor copy constructor for `has2` type. Suppressing those warnings is essential, 
// because we treat warnings as errors in those tests
#if (defined _MSC_VER)
# pragma warning( push )
# pragma warning( disable : 4510 4512 4610)
#endif
struct has2 {
    int i;
    has2 &operator=(const int& val) { i = val; return *this; }
};
#if (defined _MSC_VER)
# pragma warning( pop )
#endif

struct has3 { // Copy assignment must be generated by compiler
    has3(has3*){}
};

struct has_not: public boost::noncopyable {
    typedef boost::noncopyable base_t;
    has_not() : base_t() {}
};

#if defined(BOOST_TT_CXX11_IS_COPY_ASSIGNABLE)

struct has_not2 {
    has_not2() {}
    has_not2& operator=(has_not2&) = delete;
};

struct has_not3 {
    has_not3() {}
    has_not3& operator=(const has_not3&) = delete;
};

#endif // BOOST_TT_CXX11_IS_COPY_ASSIGNABLE

struct has_not4: private boost::noncopyable {
    typedef boost::noncopyable base_t;
    has_not4() : base_t() {}
private:
    has_not4& operator=(const has_not4&);
};

struct has_not5 {
private:
    BOOST_MOVABLE_BUT_NOT_COPYABLE(has_not5)
};

struct has_not6 {
    has_not6& operator=(has_not6&){ return *this; }
};


TT_TEST_BEGIN(is_copy_assignable)

// Main part of the test
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not>::value, false);
#if defined(BOOST_TT_CXX11_IS_COPY_ASSIGNABLE)
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not2>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not3>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not6>::value, false);
#endif
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not4>::value, false);

// Requires some basic support from Boost.Move in C++03
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<has_not5>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<bool>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<bool const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<bool volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<bool const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<signed char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<signed char const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<signed char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<signed char const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned char const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<char>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<char const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned char const volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<char volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<char const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned short const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<short>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<short const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned short const volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<short volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<short const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned int const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned int const volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned long const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<unsigned long const volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long const volatile>::value, false);

#ifdef BOOST_HAS_LONG_LONG

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::ulong_long_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::ulong_long_type const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::long_long_type>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::long_long_type const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::ulong_long_type volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::ulong_long_type const volatile>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::long_long_type volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable< ::boost::long_long_type const volatile>::value, false);

#endif

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<float>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<float const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<float volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<float const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<double const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<double const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long double>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long double const>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long double volatile>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<long double const volatile>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<void*>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int*const>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<f1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<f2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<f3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<mf1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<mf2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<mf3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<mp>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<cmf>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<enum_UDT>::value, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<const int&>::value, false);

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int&&>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<const int&&>::value, false);
#endif

// Following three tests may give different results because of compiler and C++03/C++11.
// On C++11 compiler following code:
//      int c[2][4][5][6][3];
//      int b[2][4][5][6][3] = std::move(c);
// does not compile, so we expect `false` to be the result of those three tests.
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int[2]>::value, false, true);
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int[3][2]>::value, false, true);
BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_copy_assignable<int[2][4][5][6][3]>::value, false, true);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<void>::value, false);

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<empty_POD_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<POD_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<POD_union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<empty_POD_union_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<nothrow_copy_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<nothrow_assign_UDT>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_copy_assignable<nothrow_construct_UDT>::value, true);


TT_TEST_END


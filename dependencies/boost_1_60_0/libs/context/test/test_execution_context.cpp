
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/context/all.hpp>
#include <boost/context/detail/config.hpp>

namespace ctx = boost::context;

int value1 = 0;
std::string value2;
double value3 = 0.;

void fn1( void * vp) {
    value1 = 3;
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

void fn2( int i, void * vp) {
    value1 = i;
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

void fn3( const char * what, void * vp) {
    try {
        throw std::runtime_error( what);
    } catch ( std::runtime_error const& e) {
        value2 = e.what();
    }
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

void fn4( double d, void * vp) {
    d += 3.45;
    value3 = d;
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

void fn6( void * vp) {
    value1 = 3;
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

void fn5( void * vp) {
    std::cout << "fn5: entered" << std::endl;
    ctx::execution_context ectx( fn6);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    value3 = 3.14;
    ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
    ( * mctx)();
}

struct X {
    int foo( int i, void * vp) {
        value1 = i;
        ctx::execution_context * mctx = static_cast< ctx::execution_context * >( vp);
        ( * mctx)();
        return i;
    }
};

void test_ectx() {
    value1 = 0;
    ctx::execution_context ectx( fn1);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 3, value1);
}

void test_variadric() {
    value1 = 0;
    ctx::execution_context ectx( fn2, 5);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 5, value1);
}

void test_memfn() {
    value1 = 0;
    X x;
    ctx::execution_context ectx( & X::foo, x, 7);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 7, value1);
}

void test_exception() {
    const char * what = "hello world";
    ctx::execution_context ectx( fn3, what);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( std::string( what), value2);
}

void test_fp() {
    double d = 7.13;
    ctx::execution_context ectx( fn4, d);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 10.58, value3);
}

void test_stacked() {
    value1 = 0;
    value3 = 0.;
    ctx::execution_context ectx( fn5);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 3, value1);
    BOOST_CHECK_EQUAL( 3.14, value3);
}

void test_prealloc() {
    value1 = 0;
    ctx::default_stack alloc;
    ctx::stack_context sctx( alloc.allocate() );
    void * sp = static_cast< char * >( sctx.sp) - 10;
    std::size_t size = sctx.size - 10;
    ctx::execution_context ectx( std::allocator_arg, ctx::preallocated( sp, size, sctx), alloc, fn2, 7);
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    ectx( & ctx);
    BOOST_CHECK_EQUAL( 7, value1);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.Context: execution_context test suite");

    test->add( BOOST_TEST_CASE( & test_ectx) );
#if 0
    test->add( BOOST_TEST_CASE( & test_variadric) );
    test->add( BOOST_TEST_CASE( & test_memfn) );
    test->add( BOOST_TEST_CASE( & test_exception) );
    test->add( BOOST_TEST_CASE( & test_fp) );
    test->add( BOOST_TEST_CASE( & test_stacked) );
    test->add( BOOST_TEST_CASE( & test_prealloc) );
#endif

    return test;
}

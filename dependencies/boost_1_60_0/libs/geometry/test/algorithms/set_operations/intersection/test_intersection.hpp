// Boost.Geometry (aka GGL, Generic Geometry Library)
// Unit Test

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_TEST_INTERSECTION_HPP
#define BOOST_GEOMETRY_TEST_INTERSECTION_HPP

#include <fstream>
#include <iomanip>

#include <boost/foreach.hpp>
#include <boost/variant/variant.hpp>

#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/length.hpp>
#include <boost/geometry/algorithms/num_points.hpp>

#include <boost/geometry/geometries/geometries.hpp>

#include <boost/geometry/strategies/strategies.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>


#if defined(TEST_WITH_SVG)
#  include <boost/geometry/io/svg/svg_mapper.hpp>
#endif

#include <geometry_test_common.hpp>

template <typename G1, typename G2, typename OutputType>
typename bg::default_area_result<G1>::type
check_result(
    std::vector<OutputType> const& intersection_output,
    std::string const& caseid,
    std::size_t expected_count = 0, int expected_point_count = 0,
    double expected_length_or_area = 0,
    double percentage = 0.0001,
    bool debug = false)
{
    bool const is_line = bg::geometry_id<OutputType>::type::value == 2;

    typename bg::default_area_result<G1>::type length_or_area = 0;
    int n = 0;
    for (typename std::vector<OutputType>::const_iterator it = intersection_output.begin();
            it != intersection_output.end();
            ++it)
    {
        if (expected_point_count > 0)
        {
            // here n should rather be of type std::size_t, but expected_point_count
            // is set to -1 in some test cases so type int was left for now
            n += static_cast<int>(bg::num_points(*it, true));
        }

        // instead of specialization we check it run-time here
        length_or_area += is_line
            ? bg::length(*it)
            : bg::area(*it);

        if (debug)
        {
            std::cout << std::setprecision(20) << bg::wkt(*it) << std::endl;
        }
    }

#if ! defined(BOOST_GEOMETRY_NO_BOOST_TEST)
#if ! defined(BOOST_GEOMETRY_NO_ROBUSTNESS)
    if (expected_point_count > 0)
    {
        BOOST_CHECK_MESSAGE(bg::math::abs(n - expected_point_count) < 3,
                "intersection: " << caseid
                << " #points expected: " << expected_point_count
                << " detected: " << n
                << " type: " << (type_for_assert_message<G1, G2>())
                );
    }
#endif

    if (expected_count > 0)
    {
        BOOST_CHECK_MESSAGE(intersection_output.size() == expected_count,
                "intersection: " << caseid
                << " #outputs expected: " << expected_count
                << " detected: " << intersection_output.size()
                << " type: " << (type_for_assert_message<G1, G2>())
                );
    }

    double const detected_length_or_area = boost::numeric_cast<double>(length_or_area);
    if (percentage > 0.0)
    {
        BOOST_CHECK_CLOSE(detected_length_or_area, expected_length_or_area, percentage);
    }
    else
    {
        // In some cases (geos_2) the intersection is either 0, or a tiny rectangle,
        // depending on compiler/settings. That cannot be tested by CLOSE
        BOOST_CHECK_LE(detected_length_or_area, expected_length_or_area);
    }
#endif

    return length_or_area;
}


template <typename OutputType, typename CalculationType, typename G1, typename G2>
typename bg::default_area_result<G1>::type test_intersection(std::string const& caseid,
        G1 const& g1, G2 const& g2,
        std::size_t expected_count = 0, int expected_point_count = 0,
        double expected_length_or_area = 0,
        double percentage = 0.0001,
        bool debug = false)
{
    if (debug)
    {
        std::cout << std::endl << "case " << caseid << std::endl;
    }

    typedef typename bg::point_type<G1>::type point_type;

    if (! debug)
    {
        // Check _inserter behaviour with stratey
        typedef bg::strategy_intersection
            <
                typename bg::cs_tag<point_type>::type,
                G1,
                G2,
                point_type,
                typename bg::rescale_policy_type<point_type>::type,
                CalculationType
            > strategy;
        std::vector<OutputType> clip;
        bg::detail::intersection::intersection_insert<OutputType>(g1, g2, std::back_inserter(clip), strategy());
    }

    typename bg::default_area_result<G1>::type length_or_area = 0;

    // Check normal behaviour
    std::vector<OutputType> intersection_output;
    bg::intersection(g1, g2, intersection_output);

    check_result<G1, G2>(intersection_output, caseid, expected_count, expected_point_count,
        expected_length_or_area, percentage, debug);

    // Check variant behaviour
    intersection_output.clear();
    bg::intersection(boost::variant<G1>(g1), g2, intersection_output);

    check_result<G1, G2>(intersection_output, caseid, expected_count, expected_point_count,
        expected_length_or_area, percentage, debug);

    intersection_output.clear();
    bg::intersection(g1, boost::variant<G2>(g2), intersection_output);

    check_result<G1, G2>(intersection_output, caseid, expected_count, expected_point_count,
        expected_length_or_area, percentage, debug);

    intersection_output.clear();
    bg::intersection(boost::variant<G1>(g1), boost::variant<G2>(g2), intersection_output);

    check_result<G1, G2>(intersection_output, caseid, expected_count, expected_point_count,
        expected_length_or_area, percentage, debug);

#if defined(TEST_WITH_SVG)
    {
        bool const is_line = bg::geometry_id<OutputType>::type::value == 2;
        typedef typename bg::coordinate_type<G1>::type coordinate_type;

        bool const ccw =
            bg::point_order<G1>::value == bg::counterclockwise
            || bg::point_order<G2>::value == bg::counterclockwise;
        bool const open =
            bg::closure<G1>::value == bg::open
            || bg::closure<G2>::value == bg::open;

        std::ostringstream filename;
        filename << "intersection_"
            << caseid << "_"
            << string_from_type<coordinate_type>::name()
            << string_from_type<CalculationType>::name()
            << (ccw ? "_ccw" : "")
            << (open ? "_open" : "")
#if defined(BOOST_GEOMETRY_NO_ROBUSTNESS)
            << "_no_rob"
#endif
            << ".svg";

        std::ofstream svg(filename.str().c_str());

        bg::svg_mapper<point_type> mapper(svg, 500, 500);

        mapper.add(g1);
        mapper.add(g2);

        mapper.map(g1, is_line
            ? "opacity:0.6;stroke:rgb(0,255,0);stroke-width:5"
            : "fill-opacity:0.5;fill:rgb(153,204,0);"
                    "stroke:rgb(153,204,0);stroke-width:3");
        mapper.map(g2, "fill-opacity:0.3;fill:rgb(51,51,153);"
                    "stroke:rgb(51,51,153);stroke-width:3");

        for (typename std::vector<OutputType>::const_iterator it = intersection_output.begin();
                it != intersection_output.end(); ++it)
        {
            mapper.map(*it, "fill-opacity:0.2;stroke-opacity:0.4;fill:rgb(255,0,0);"
                        "stroke:rgb(255,0,255);stroke-width:8");
        }
    }
#endif


    if (debug)
    {
        std::cout << "end case " << caseid << std::endl;
    }

    return length_or_area;
}

template <typename OutputType, typename G1, typename G2>
typename bg::default_area_result<G1>::type test_one(std::string const& caseid,
        std::string const& wkt1, std::string const& wkt2,
        std::size_t expected_count = 0, int expected_point_count = 0,
        double expected_length_or_area = 0,
        double percentage = 0.0001,
        bool debug = false)
{
    G1 g1;
    bg::read_wkt(wkt1, g1);

    G2 g2;
    bg::read_wkt(wkt2, g2);

    // Reverse if necessary
    bg::correct(g1);
    bg::correct(g2);

    return test_intersection<OutputType, void>(caseid, g1, g2,
        expected_count, expected_point_count,
        expected_length_or_area, percentage,
        debug);
}

template <typename OutputType, typename Areal, typename Linear>
void test_one_lp(std::string const& caseid,
        std::string const& wkt_areal, std::string const& wkt_linear,
        std::size_t expected_count = 0, int expected_point_count = 0,
        double expected_length = 0,
        double percentage = 0.0001,
        bool debug1 = false, bool debug2 = false)
{
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << caseid << " -- start" << std::endl;
#endif
    Areal areal;
    bg::read_wkt(wkt_areal, areal);
    bg::correct(areal);

    Linear linear;
    bg::read_wkt(wkt_linear, linear);

    test_intersection<OutputType, void>(caseid, areal, linear,
        expected_count, expected_point_count,
        expected_length, percentage, debug1);

    // A linestring reversed should deliver exactly the same.
    bg::reverse(linear);

    test_intersection<OutputType, void>(caseid + "_rev", areal, linear,
        expected_count, expected_point_count,
        expected_length, percentage, debug2);
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << caseid << " -- end" << std::endl;
#endif
}

template <typename Geometry1, typename Geometry2>
void test_point_output(std::string const& wkt1, std::string const& wkt2, unsigned int expected_count)
{
    Geometry1 g1;
    bg::read_wkt(wkt1, g1);
    bg::correct(g1);

    Geometry2 g2;
    bg::read_wkt(wkt2, g2);
    bg::correct(g2);

    std::vector<typename bg::point_type<Geometry1>::type> points;
    bg::intersection(g1, g2, points);
    BOOST_CHECK_EQUAL(points.size(), expected_count);
}


#endif

// Boost.Geometry (aka GGL, Generic Geometry Library)
// Unit Test

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_TEST_OVERLAY_HPP
#define BOOST_GEOMETRY_TEST_OVERLAY_HPP

#include <string>

// 1. (Example) testcases
static std::string example_box = "box(1.5 1.5, 4.5 2.5)";

static std::string example_polygon =
    "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3)"
        "(4.0 2.0, 4.2 1.4, 4.8 1.9, 4.4 2.2, 4.0 2.0))";

static std::string example_ring =
    "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3))";

static std::string example_star =
    "POLYGON((4.0 -0.5 , 3.5 1.0 , 2.0 1.5 , 3.5 2.0 , 4.0 3.5 , 4.5 2.0 , 6.0 1.5 , 4.5 1.0 , 4.0 -0.5))";

static std::string polygon_empty = "POLYGON EMPTY";

// 2. Alphabetically ordered testcase pairs

static std::string crossed[2] =
    {"POLYGON((0 0,0 5,5 5,5 0,0 0),(4 1,3 4,1 2,4 1))",
    "POLYGON((2 1,1 4,4 3,2 1))"};

static std::string disjoint[2] =
    {"POLYGON((3 0,3 1,4 1,4 0,3 0))",
    "POLYGON((3 4,3 5,4 5,4 4,3 4))"};

static std::string distance_zero[2] =
    {"POLYGON((1 1,1 4,4 4,4 1,1 1))",
    "POLYGON((1.9 0.9,2.0 4.000001,2.1 1.0,1.9 0.9))"};

// e-45 gives 'convenient' IEEE-single-FP-error,
static std::string epsilon[2] =
    {"POLYGON((0.0 0.0"
        ",3.0e-45 4.0e-45"
        ",4.0e-45 1.0e-45"
        ",0.0 0.0))",
    "POLYGON((2.0e-45 2.0e-45"
        ",6.0e-45 4.0e-45"
        ",4.0e-45 -1.0e-45"
        ",2.0e-45 2.0e-45))"};

static std::string epsilon_multi_ip[2] =
    {
    "POLYGON("
        "(0.0e-44 2.0e-44,0.5e-44 2.5e-44,1.2e-44 2.0e-44,1.7e-44 2.5e-44,2.5e-44 2.0e-44,2.0e-44 1.5e-44"
        ",2.5e-44 1.0e-44,2.0e-44 0.5e-44,1.7e-44 0.0e-44,1.5e-44 0.5e-44,1.2e-44 0.0e-44,1.0e-44 0.5e-44"
        ",0.7e-44 0.0e-44,0.5e-44 1.7e-44,0.12e-44 1.5e-44,0.5e-44 1.2e-44,0.0e-44 1.0e-44,0.0e-44 2.0e-44))",
    "POLYGON("
        "(0.2e-44 0.2e-44,0.2e-44 2.2e-44,2.2e-44 2.2e-44,2.2e-44 0.2e-44,0.2e-44 0.2e-44))"
    };

static std::string equal_holes_disjoint[2] =
    {"POLYGON((0 0,0 9,9 9,9 0,0 0),(1 1,4 1,4 8,1 8,1 1),(5 1,8 1,8 4,5 4,5 1))",
    "POLYGON((0 0,0 9,9 9,9 0,0 0),(1 1,4 1,4 8,1 8,1 1),(5 5,8 5,8 8,5 8,5 5))"};

static std::string first_within_second[2] =
    {"POLYGON((2 2,2 3,3 3,3 2,2 2))",
    "POLYGON((0 0, 0 5, 5 5, 5 0, 0 0))"};

static std::string first_within_hole_of_second[2] =
    {"POLYGON((2 2,2 3,3 3,3 2,2 2))",
    "POLYGON((0 0, 0 5, 5 5, 5 0, 0 0),(1 1,4 1,4 4,1 4,1 1))"};

// == case 52
static std::string fitting[2] =
    {"POLYGON((0 0,0 5,5 5,5 0,0 0),(4 1,3 4,1 2,4 1))",
    "POLYGON((1 2,3 4,4 1,1 2))"};

static std::string identical[2] =
    {"POLYGON((0 0,0 1,1 1,1 0,0 0))",
    "POLYGON((1 1,1 0,0 0,0 1,1 1))"};

// case 2102 from "algorithms/detail/overlay/robustness/assemble.hpp"
static std::string intersect_exterior_and_interiors_winded[2] =
    {"POLYGON((2 0.5,0.5 2,0.5 8,2 9.5,6 9.5,8.5 8,8.5 2,7 0.5,2 0.5),(2 2,7 2,7 8,2 8,2 2))",
    "POLYGON((1 1,1 9,8 9,8 1,1 1),(4 4,5 4,5 5,4 5,4 4))"};

static std::string intersect_holes_disjoint[2] =
    {"POLYGON((0 0,0 7,5 7,5 0,0 0),(2 2,3 2,3 3,2 3,2 2))",
    "POLYGON((1 1,1 6,6 6,6 1,1 1),(2 4,3 4,3 5,2 5,2 4))"};

static std::string intersect_holes_intersect[2] =
    {"POLYGON((0 0,0 7,5 7,5 0,0 0),(2 2,3 2,3 3,2 3,2 2))",
            "POLYGON((1 1,1 6,6 6,6 1,1 1),(2.5 2.5,3.5 2.5,3.5 3.5,2.5 3.5,2.5 2.5))"};

static std::string intersect_holes_intersect_and_disjoint[2] =
    {"POLYGON((0 0,0 7,5 7,5 0,0 0),(2 2,3 2,3 3,2 3,2 2),(2 4,3 4,3 5,2 5,2 4))",
    "POLYGON((1 1,1 6,6 6,6 1,1 1),(2.5 2.5,3.5 2.5,3.5 3.5,2.5 3.5,2.5 2.5))"};

static std::string intersect_holes_intersect_and_touch[2] =
    {"POLYGON((0 0,0 7,5 7,5 0,0 0),(2 2,3 2,3 3,2 3,2 2),(2.5 4,3 4.5,2.5 5,2 4.5,2.5 4))",
    "POLYGON((1 1,1 6,6 6,6 1,1 1),(2.5 2.5,3.5 2.5,3.5 3.5,2.5 3.5,2.5 2.5),(3.5 4,4 4.5,3.5 5,3 4.5,3.5 4))"};

static std::string intersect_holes_new_ring[2] =
    {"POLYGON((4 4,4 16,16 16,16 4,4 4),(7 6,14 10,7 14,11 10,7 6))",
    "POLYGON((2 2,2 18,18 18,18 2,2 2),(13 6,9 10,13 14,6 10,13 6))"};

static std::string isovist1[2] =
    {
    "POLYGON((37.29449462890625 1.7902572154998779,  46.296027072709599 -2.4984308554828116,  45.389434814453125 -4.5143837928771973,  47.585065917176543 -6.1314922196594779,  46.523914387974358 -8.5152102535033496,  42.699958801269531 -4.4278755187988281,  42.577877044677734 -4.4900407791137695,  42.577911376953125 -4.4901103973388672,  40.758884429931641 -5.418975830078125,  40.6978759765625 -5.4500408172607422,  41.590042114257813 -7.2021245956420898,  57.297810222148939 -37.546793343968417,  50.974888957147442 -30.277285722290763,  37.140213012695313 1.3446992635726929,  37.000419616699219 1.664225697517395,  37.29449462890625 1.7902572154998779))",
    "POLYGON((43.644271850585938 0.96149998903274536,43.764598846435547 0.93951499462127686,49.071769542946825 0.61489892713413252,48.43512638981781 -0.81299959072453376,47.830955505371094 -0.69758313894271851,47.263670054709685 -1.784876824891044,46.695858001708984 -1.6093428134918213,45.389434814453125 -4.5143837928771973,47.604561877161387 -6.087697464505224,46.559533858616469 -8.435196445683264,42.699958801269531 -4.4278755187988281,42.577877044677734 -4.4900407791137695,42.577911376953125 -4.4901103973388672,40.758884429931641 -5.418975830078125,40.6978759765625 -5.4500408172607422,41.590042114257813 -7.2021245956420898,57.524304765518266 -37.807195733984784,41.988733475572282 -19.945838749437218,41.821544647216797 -19.211688995361328,40.800632476806641 -17.208097457885742,39.966808319091797 -17.625011444091797,38.823680877685547 -16.296066284179688,37.326129913330078 -17.190576553344727,35.963497161865234 -15.476018905639648,35.656356811523438 -15.66030216217041,34.931102752685547 -16.223842620849609,34.634240447128811 -15.85007183479255,34.886280059814453 -14.120697975158691,34.658355712890625 -13.81736946105957,34.328716278076172 -13.992490768432617,33.598796844482422 -14.546377182006836,33.164891643669634 -14.000060288415174,33.566280364990234 -12.450697898864746,33.339523315429688 -12.147735595703125,32.998821258544922 -12.323249816894531,32.274600982666016 -12.879127502441406,31.682494778186321 -12.133624901803865,32.226280212402344 -10.790698051452637,32.000633239746094 -10.488097190856934,31.669155120849609 -10.653837203979492,30.947774887084961 -11.208560943603516,30.207040612748258 -10.275926149505661,30.896280288696289 -9.1206979751586914,30.670633316040039 -8.8180980682373047,30.339155197143555 -8.9838371276855469,29.619997024536133 -9.5368013381958008,29.135100397190627 -8.9262827849488211,32.718830108642578 -4.3281683921813965,32.708168029785156 -2.3611698150634766,32.708126068115234 -2.3611700534820557,32.708126068115234 -2.3611266613006592,30.501169204711914 -2.3718316555023193,27.069889344709196 -4.2926591211028242,26.472516656201325 -3.5380830513658776,36.954700469970703 1.2597870826721191,37.140213012695313 1.3446992635726929,37.000419616699219 1.664225697517395,37.29449462890625 1.7902572154998779,37.43402099609375 1.470055103302002,51.370888500897557 7.4163459734570729,51.20102152843122 7.1738039562841562,42.721500396728516 3.6584999561309814,42.721500396728516 2.2342472076416016,42.399410247802734 1.4956772327423096,43.644271850585938 0.96149998903274536))"
    };

static std::string new_hole[2] =
    {"POLYGON((2 2,2 5,5 5,5 2,2 2))",
    "POLYGON((0 0,0 6,3 6,3 4,1 4,1 3,3 3,3 0,0 0))"};

static std::string only_hole_intersections[3] =
    {"POLYGON((0 0,0 10,20 10,20 0,0 0),(1 1,7 5,5 7,1 1),(11 1,17 5,15 7,11 1))",
    "POLYGON((0 0,0 10,20 10,20 0,0 0),(1 1,7 6,6 7,1 1),(11 1,17 6,16 7,11 1))",
    "POLYGON((0.5 0.5,0.5 9.5,19.5 9.5,19.5 0.5,0.5 0.5),(1 1,7 6,6 7,1 1),(11 1,17 6,16 7,11 1))"};

static std::string side_side[2] =
    {"POLYGON((0 0,0 1,1 1,1 0,0 0))",
    "POLYGON((1 0,1 1,2 1,2 0,1 0))"};

static std::string simplex_normal[2] =
    {"POLYGON((0 1,2 5,5 3,0 1))",
    "POLYGON((3 0,0 3,4 5,3 0))"};

static std::string simplex_reversed[2] =
    {"POLYGON((0 1,5 3,2 5,0 1))",
    "POLYGON((3 0,4 5,0 3,3 0))"};

static std::string star_comb_15[2] =
    {"POLYGON((25 52.5,27.1694 29.5048,46.5004 42.146,29.8746 26.1126,51.8105 18.8807,28.9092 21.8826,36.9318 0.223356,25 20,13.0682 0.223356,21.0908 21.8826,-1.81052 18.8807,20.1254 26.1126,3.49963 42.146,22.8306 29.5048,25 52.5))",
    "POLYGON((25 0,0 25,25 50,50 25,49.0741 24.0741,25 48.1481,24.0741 47.2222,48.1481 23.1481,47.2222 22.2222,23.1481 46.2963,22.2222 45.3704,46.2963 21.2963,45.3704 20.3704,21.2963 44.4444,20.3704 43.5185,44.4444 19.4444,43.5185 18.5185,19.4444 42.5926,18.5185 41.6667,42.5926 17.5926,41.6667 16.6667,17.5926 40.7407,16.6667 39.8148,40.7407 15.7407,39.8148 14.8148,15.7407 38.8889,14.8148 37.963,38.8889 13.8889,37.963 12.963,13.8889 37.037,12.963 36.1111,37.037 12.037,36.1111 11.1111,12.037 35.1852,11.1111 34.2593,35.1852 10.1852,34.2593 9.25926,10.1852 33.3333,9.25926 32.4074,33.3333 8.33333,32.4074 7.40741,8.33333 31.4815,7.40741 30.5556,31.4815 6.48148,30.5556 5.55556,6.48148 29.6296,5.55556 28.7037,29.6296 4.62963,28.7037 3.7037,4.62963 27.7778,3.7037 26.8519,27.7778 2.77778,26.8519 1.85185,2.77778 25.9259,1.85185 25,25.9259 0.925926,25 0))"};

static std::string two_bends[2] =
    {"POLYGON((0 4,4 8,7 7,8 4,5 3,4 0,0 4))",
    "POLYGON((0 4,4 8,5 5,8 4,7 1,4 0,0 4))"};

// within each other, having no intersections but many holes within each other
static std::string winded[2] =
    {"POLYGON((0 0,0 11,11 11,11 0,0 0),(3 3,4 3,4 4,3 4,3 3),(5 3,6 3,6 4,5 4,5 3),(2 6,7 6,7 9,2 9,2 6),(9 2,10 2,10 5,9 5,9 2))",
    "POLYGON((1 1,1 10,10 10,10 6,8 6,8 1,1 1),(2 2,7 2,7 5,2 5,2 2),(3 7,4 7,4 8,3 8,3 7),(5 7,6 7,6 8,5 8,5 7),(8 7,9 7,9 8,8 8,8 7))"};

static std::string within_holes_disjoint[2] =
    {"POLYGON((0 0,0 7,7 7,7 0,0 0),(2 2,3 2,3 3,2 3,2 2))",
    "POLYGON((1 1,1 6,6 6,6 1,1 1),(2 4,3 4,3 5,2 5,2 4))"};

// == case 53
static std::string wrapped[3] = {
        "POLYGON((2 2,2 3,3 3,3 2,2 2))",
        /*a:*/ "POLYGON((0 2,0 5,5 5,5 0,2 0,2 2,3 2,3 1,4 1,4 4,1 4,1 3,2 3,2 2,0 2))", // NOT st_isvalid
        /*b:*/ "POLYGON((0 2,0 5,5 5,5 0,2 0,2 2,0 2),(1 3,2 3,2 2,3 2,3 1,4 1,4 4,1 4,1 3))" // st_isvalid
    };

#endif

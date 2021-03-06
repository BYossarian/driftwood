
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <chrono>

#include "./computational-geometry/vector-2d.h"
#include "./computational-geometry/convex-hull.h"
#include "./computational-geometry/sat.h"
#include "./computational-geometry/gjk.h"
#include "./computational-geometry/epa.h"
#include "./helpers/timer.h"

struct convex_hull_test {
    std::vector<vector_2d<double>> input_points, expected_result;
};

std::vector<convex_hull_test> convex_hull_tests = {
    {
        // convex hull test #1
        { { 0.3215348546593775, 0.03629583077160248 }, { 0.02402358131857918, -0.2356728797179394 }, { 0.04590851212470659, -0.4156409924995536 }, 
            { 0.3218384001607433, 0.1379850698988746 }, { 0.11506479756447, -0.1059521474930943 }, { 0.2622539999543261, -0.29702873322836 }, 
            { -0.161920957418085, -0.4055339716426413 }, { 0.1905378631228002, 0.3698601009043493 }, { 0.2387090918968516, -0.01629827079949742 }, 
            { 0.07495888748668034, -0.1659825110491202 }, { 0.3319341836794598, -0.1821814101954749 }, { 0.07703635755650362, -0.2499430638271785 }, 
            { 0.2069242999022122, -0.2232970760420869 }, { 0.04604079532068295, -0.1923573186549892 }, { 0.05054295812784038, 0.4754929463150845 }, 
            { -0.3900589168910486, 0.2797829520700341 }, { 0.3120693385713448, -0.0506329867529059 }, { 0.01138812723698857, 0.4002504701728471 }, 
            { 0.009645149586391732, 0.1060251100976254 }, { -0.03597933197019559, 0.2953639456959105 }, { 0.1818290866742182, 0.001454397571696298 }, 
            { 0.444056063372694, 0.2502497166863175 }, { -0.05301752458607545, -0.06553921621808712 }, { 0.4823896228171788, -0.4776170002088109 }, 
            { -0.3089226845734964, -0.06356112199235814 }, { -0.271780741188471, 0.1810810595574612 }, { 0.4293626522918815, 0.2980897964891882 }, 
            { -0.004796652127799228, 0.382663812844701 }, { 0.430695573269106, -0.2995073500084759 }, { 0.1799668387323309, -0.2973467472915973 }, 
            { 0.4932166845474547, 0.4928094162538735 }, { -0.3521487911717489, 0.4352656197131292 }, { -0.4907368011686362, 0.1865826865533206 }, 
            { -0.1047924716070224, -0.247073392148198 }, { 0.4374961861758457, -0.001606279519951237 }, { 0.003256207800708899, -0.2729194320486108 }, 
            { 0.04310378203457577, 0.4452604050238248 }, { 0.4916198379282093, -0.345391701297268 }, { 0.001675087028811806, 0.1531837672490476 }, 
            { -0.4404289572876217, -0.2894855991839297 }
        },
        // expected result #1
        { { 0.4823896228171788, -0.4776170002088109 }, { 0.4916198379282093, -0.345391701297268 }, { 0.4932166845474547, 0.4928094162538735 }, 
            { 0.05054295812784038, 0.4754929463150845 }, { -0.3521487911717489, 0.4352656197131292 }, { -0.4907368011686362, 0.1865826865533206 }, 
            { -0.4404289572876217, -0.2894855991839297 }, { -0.161920957418085, -0.4055339716426413 } 
        }
    },
    {
        // convex hull test #2
        { { 0, 0 }, { 1, 0 }, { 0, 1 } },
        // expected result #2
        { { 0, 0 }, { 1, 0 }, { 0, 1 } }
    },
    {
        // convex hull test #3
        { { 0, 0 }, { 0, 1 }, { 1, 0 } },
        // expected result #3
        { { 0, 0 }, { 1, 0 }, { 0, 1 } }
    },
    {
        // convex hull test #4
        { { 1.482459, 3.015825 }, { -1.751458, 1.604398 }, { 2.683672, 0.960661 }, { 3.937593, 5.318933 }, { 3.925634, 1.233532 }, 
            { 4.377908, 2.631490 }, { 2.247293, 1.593499 }, { 2.380997, 1.127417 }, { 3.186213, 2.990795 }, { 3.531792, 2.343877 } 
        },
        // expected result #4
        { { 2.683672, 0.960661 }, { 3.925634, 1.233532 }, { 4.377908, 2.631490 }, { 3.937593, 5.318933 }, { -1.751458, 1.604398 } }
    },
    {
        // convex hull test #5
        { { 0.676379, -7.354404 }, { 1.833986, -5.196411 }, { -4.863173, 10.020462 }, { -1.318370, -5.675507 }, { -1.321741, 0.927964 }, 
            { -2.599505, 3.764413 }, { -1.163348, -6.802671 }, { 5.534011, 10.398596 }, { -3.375691, 2.166727 }, { -3.252004, -8.470309 } 
        },
        // expected result #5
        { { -3.252004, -8.470309 }, { 0.676379, -7.354404 }, { 1.833986, -5.196411 }, { 5.534011, 10.398596 }, { -4.863173, 10.020462 } }
    },
    {
        // convex hull test #6
        { { 4.4, 14 }, { 6.7, 15.25 }, { 6.9, 12.8 }, { 2.1, 11.1 }, { 9.5, 14.9 }, { 13.2, 11.9 }, { 10.3, 12.3 }, { 6.8, 9.5 }, { 3.3, 7.7 }, 
            { 0.6, 5.1 }, { 5.3, 2.4 }, { 8.45, 4.7 }, { 11.5, 9.6 }, { 13.8, 7.3 }, { 12.9, 3.1 }, { 11, 1.1 } 
        },
        // expected result #6
        { { 11, 1.1 }, { 12.9, 3.1 }, { 13.8, 7.3 }, { 13.2, 11.9 }, { 9.5, 14.9 }, { 6.7, 15.25 }, { 4.4, 14 }, { 2.1, 11.1 }, { 0.6, 5.1 }, 
            { 5.3, 2.4 } }
    },
    {
        // convex hull test #7
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
        // expected result #7
        { { 0, 0 }, { 1, 0 }, { 0, 1 } }
    },
    {
        // convex hull test #8
        { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 0 }},
        // expected result #8
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }
    }
};

struct intersection_test {
    std::vector<vector_2d<double>> shape_a, shape_b;
    bool are_intersecting;
    vector_2d<double> penetration_vector;
};

// TODO: add cases where origin is on corner/edges of A-B
// TODO: think about < vs <= for dot products

std::vector<intersection_test> intersection_tests = {
    {   
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        true,
        { 1, 0 }  // NB: this test has multiple correct penetration_vectors
    },
    {   
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        { { 0.5, -0.9 }, { 1.5, -0.9 }, { 0.5, 0.1 }, { 1.5, 0.1 } }, 
        true,
        { 0, -0.1 }
    },
    {   
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        { { 0.5, 0 }, { 1.5, 0 }, { 0.5, 1 }, { 1.5, 1 } }, 
        true,
        { 0.5, 0 }
    },
    {   
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        { { 0.5, 0.5 }, { 1.5, 0.5 }, { 0.5, 1.5 }, { 1.5, 1.5 } }, 
        true,
        { 0.5, 0 }  // NB: this test has multiple correct penetration_vectors
    },
    {   
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, 
        { { 0, 0 }, { 1, 1 }, { 0, 1 } }, 
        true,
        { -0.5 , 0.5 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 3, 1 }, { 1, 1 }, { 0, 1 } },
        { { 2, 0 }, { 3, 0 }, { 3, 1 }, { 2, 1 } },
        true,
        { 0.2, -0.4 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 4, 0.5 }, { 1, 1 }, { 0, 1 } },
        { { 2, 0 }, { 3, 0 }, { 3, 1 }, { 2, 1 } },
        true,
        { 5.0 / 37.0, 30.0 / 37.0 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 10, 1 }, { 0, 1 } },
        { { 2, 0 }, { 3, 0 }, { 12, 1 } },
        false,
        { 0, 0 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { 0, 0 }, { 1, 0 }, { 0, -1 } },
        true,
        { 0, 0 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { 1.1, 0 }, { 2, 0 }, { 2, 1 } },
        false,
        { 0, 0 }
    },
    {
        { { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { 1, 0 }, { 2, 0 }, { 2, 1 } },
        true,
        { 0, 0 }
    },
    {
        { { 2, 2 }, { 4, 2 }, { 2, 4 } },
        { { 3, 3 }, { 4, 3 }, { 4, 4 }, { 3, 4 } },
        true,
        { 0, 0 }
    },
    {
        { { 2, 2 }, { 4, 2 }, { 2, 4 } },
        { { 2.9, 2.9 }, { 3.9, 2.9 }, { 3.9, 3.9 }, { 2.9, 3.9 } },
        true,
        { 0.1, 0.1 }
    },
    {
        { { 2, 2 }, { 4, 2 }, { 2, 4 } },
        { { 3.01, 3 }, { 4, 3 }, { 3, 4 }, { 4, 4 } },
        false,
        { 0, 0 }
    },
    {
        { { -4, -4 }, { -5, -4 }, { -4, -5 }, { -5, -5 } },
        { { -3, -3 }, { -4, -3 }, { -3, -4 }, { -4, -4 } },
        true,
        { 0, 0 }
    },
    {   
        { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }, 
        { { 1, 0.5 }, { 2, 0.5 }, { 2, 1.5 }, { 1, 1.5 } }, 
        true,
        { 0, 0 }
    },
    {   
        { { 1.21, 1.001 }, { 3.77, 0.9 }, { 3.76, 4.5 }, { 2.42, 4.5 }, { 1.21, 4.3 } }, 
        { { -5.7, -3.42 }, { 0.1, -3 }, { 1.21, 0.999 }, { -2.3, 0.999 } }, 
        false,
        { 0, 0 }
    },
    {   
        { { 1.21, 1.001 }, { 3.77, 0.9 }, { 3.76, 4.5 }, { 2.42, 4.5 }, { 1.21, 4.3 } }, 
        { { -5.7, -3.42 }, { 0.1, -3 }, { 1.23, 0.999 }, { -2.3, 0.999 } }, 
        false,
        { 0, 0 }
    },
    {   
        { { 1.21, 1.001 }, { 3.77, 0.9 }, { 3.76, 4.5 }, { 2.42, 4.5 }, { 1.21, 4.3 } }, 
        { { -5.7, -3.42 }, { 0.1, -3 }, { 1.21, 1.001 }, { -2.3, 0.999 } }, 
        true,
        { 0, 0 }
    },
    {   
        { { 1.21, 1.001 }, { 3.77, 0.9 }, { 3.76, 4.5 }, { 2.42, 4.5 }, { 1.21, 4.3 } }, 
        { { -5.7, -3.42 }, { 0.1, -3 }, { 1.22, 1 }, { 1.2, 1.002 }, { -2.3, 0.999 } }, 
        true,
        { 0, 0 }
    },
    {   
        { { 1.21, 1.001 }, { 3.77, 0.9 }, { 3.76, 4.5 }, { 2.42, 4.5 }, { 1.21, 4.3 } }, 
        { { -5.7, -3.42 }, { 0.1, -3 }, { 1.22, 1 }, { 1.2, 1.0019 }, { -2.3, 0.999 } }, 
        false,
        { 0, 0 }
    }
};

template <typename T>
bool are_equal_point_lists(const std::vector<vector_2d<T>> &points_a, const std::vector<vector_2d<T>> &points_b) {

    if (points_a.size() != points_b.size()) {
        return false;
    }

    for (int i = 0, l = points_a.size(); i < l; i++) {
        if (points_a[i].x != points_b[i].x || points_a[i].y != points_b[i].y) {
            return false;
        }
    }

    return true;

}

template <typename T>
bool almost_equal(const vector_2d<T> &vector_a, const vector_2d<T> &vector_b) {

    T x_diff = vector_a.x == 0 ? vector_b.x : (vector_b.x - vector_a.x) / vector_a.x;
    T y_diff = vector_a.y == 0 ? vector_b.y : (vector_b.y - vector_a.y) / vector_a.y;

    return std::abs(x_diff) < 0.0001 && std::abs(y_diff) < 0.0001;

}

// TODO:
// 1) think about degenerate/edge/corner cases
// 2) organise api a bit better; put in namespace
// 3) account for floating point inaccuracies
// 4) speed up EPA and convex hull ops
// 5) add v-clip
// 6) add/think about escapes for while(true) loops
// 7) will GJK be faster if we maintain the ccw winding of simplex?

int main() {

    // make sure all intersection_tests shapes are convex:
    for (int i = 0, l = intersection_tests.size(); i < l; i++) {
        make_convex_hull(intersection_tests[i].shape_a);
        make_convex_hull(intersection_tests[i].shape_b);
    }

    std::cout << "CONVEX HULL:\n    ";
    for (int i = 0, l = convex_hull_tests.size(); i < l; i ++) {
        std::vector<vector_2d<double>> points(convex_hull_tests[i].input_points);
        make_convex_hull(points);
        std::cout << i << " ";
        if (!are_equal_point_lists(points, convex_hull_tests[i].expected_result)) {
            std::cout << "FAILED\n";
            print_points(convex_hull_tests[i].input_points);
            print_points(points);
            print_points(convex_hull_tests[i].expected_result);
            throw;
        }
    }
    std::cout << "PASSED\n";


    std::cout << "SAT:\n    ";
    for (int i = 0, l = intersection_tests.size(); i < l; i++) {
        std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
        std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
        std::cout << i << " ";
        if (sat_intersects(shape_a, shape_b) != intersection_tests[i].are_intersecting) {
            std::cout << "FAILED\n";
            throw;
        }
    }
    std::cout << "PASSED\n";

    {
        std::array<vector_2d<double>, 3> simplex;
        std::cout << "GJK:\n    ";
        for (int i = 0, l = intersection_tests.size(); i < l; i++) {
            std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
            std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
            std::cout << i << " ";
            if (intersection_tests[i].are_intersecting != (gjk_intersects(shape_a, shape_b, simplex) != gjk_result::no_intersection)) {
                std::cout << "FAILED\n";
                throw;
            }
        }
        std::cout << "PASSED\n";
    }

    {
        std::array<vector_2d<double>, 3> simplex;
        std::cout << "GJK + EPA:\n    ";
        for (int i = 0, l = intersection_tests.size(); i < l; i++) {
            std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
            std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
            std::cout << i << " ";
            gjk_result result = gjk_intersects(shape_a, shape_b, simplex);
            if (!almost_equal(intersection_tests[i].penetration_vector, epa_get_penetration_vector(shape_a, shape_b, simplex, result))) {
                std::cout << "FAILED\n";
                throw;
            }
        }
        std::cout << "PASSED\n";
    }

    std::cout << "\nALL GOOD... :)\n\n";


    timer<std::chrono::microseconds> test_timer{};

    // NB: can only run through convex_hull_tests once, because 
    // make_convex_hull mutates it's input:
    for (int i = 0, l = convex_hull_tests.size(); i < l; i ++) {
        make_convex_hull(convex_hull_tests[i].input_points);
    }
    std::cout << "convex hull time: " << test_timer.get_ticks() / 1000.0 << " ms (" 
                << static_cast<double>(test_timer.get_ticks()) / convex_hull_tests.size() << " ms/1k ops)\n";

    test_timer.reset();

    for (int j = 0; j < 1000; j++) {
        for (int i = 0, l = intersection_tests.size(); i < l; i++) {
            std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
            std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
            sat_intersects(shape_a, shape_b);
        }
    }
    std::cout << "sat time: " << test_timer.get_ticks() / 1000.0 << " ms (" 
                << test_timer.get_ticks() / (1000.0 * intersection_tests.size()) << " ms/1k ops)\n";

    test_timer.reset();

    {
        std::array<vector_2d<double>, 3> simplex;
        for (int j = 0; j < 1000; j++) {
            for (int i = 0, l = intersection_tests.size(); i < l; i++) {
                std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
                std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
                gjk_intersects(shape_a, shape_b, simplex);
            }
        }
        std::cout << "gjk time: " << test_timer.get_ticks() / 1000.0 << " ms (" 
                    << test_timer.get_ticks() / (1000.0 * intersection_tests.size()) << " ms/1k ops)\n";
    }

    test_timer.reset();

    {
        std::array<vector_2d<double>, 3> simplex;
        for (int j = 0; j < 1000; j++) {
            for (int i = 0, l = intersection_tests.size(); i < l; i++) {
                std::vector<vector_2d<double>> &shape_a = intersection_tests[i].shape_a;
                std::vector<vector_2d<double>> &shape_b = intersection_tests[i].shape_b;
                gjk_result result = gjk_intersects(shape_a, shape_b, simplex);
                if (result == gjk_result::no_intersection || result == gjk_result::origin_at_vertex) {
                    continue;
                }
                epa_get_penetration_vector(shape_a, shape_b, simplex, result);
            }
        }
        std::cout << "gjk + epa time: " << test_timer.get_ticks() / 1000.0 << " ms (" 
                    << test_timer.get_ticks() / (1000.0 * intersection_tests.size()) << " ms/1k ops)\n";
    }

}

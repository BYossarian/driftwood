
#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <utility>
#include <functional>

#include "./vector-2d.h"

// support function for a convex hull:
template <typename T>
vector_2d<T> convex_hull_support(const std::vector<vector_2d<T>> &convex_hull, const vector_2d<T> &direction) {

    T point_projection_max = direction.dot(convex_hull[0]);
    size_t support_index = 0;
    for (size_t i = 1, l = convex_hull.size(); i < l; i++) {
        T point_projection = direction.dot(convex_hull[i]);
        if (point_projection > point_projection_max) {
            point_projection_max = point_projection;
            support_index = i;
        }
    }

    return convex_hull[support_index];

}

// 2D vector triple product  a x (b x c) = b(a.c) - c(a.b)
template <typename T>
vector_2d<T> vector_triple_product(const vector_2d<T> &vector_a, const vector_2d<T> &vector_b, const vector_2d<T> &vector_c) {

    T a_dot_c = vector_a.dot(vector_c);
    T a_dot_b = vector_a.dot(vector_b);

    return {
        vector_b.x * a_dot_c - vector_c.x * a_dot_b,
        vector_b.y * a_dot_c - vector_c.y * a_dot_b
    };

}

enum class gjk_result {
    no_intersection = 0,
    origin_at_vertex = 1,
    origin_on_edge = 2,
    origin_inside = 3
};

// checks if the two convex_hulls overlap using the GJK algorithm
// The simplex input doesn't have to contain any particular values (i.e. doesn't have to be all 0's)
// the return values are:
// no_intersection = a and b aren't overlapping
// origin_at_vertex = one of simplex's vertices is the origin => a and b are touching (boundaries overlap, not interiors)
// origin_on_edge = the origin lies on an edge of simplex, specifically the simplex[0] -> simplex[1] edge
//                      => a and b are overlapping (possibly just touching if the simplex edge is also an edge 
//                          for the Minkowski difference, but GJK doesn't specify this)
// origin_inside = the origin lies within the interior of simplex => a and b are fully overlapping (i.e. not just touching)
template <typename T, size_t N = 10>
gjk_result gjk_intersects(const std::vector<vector_2d<T>> &convex_hull_a, const std::vector<vector_2d<T>> &convex_hull_b, std::array<vector_2d<T>, 3> &simplex, vector_2d<T> direction = { 1, 0 }) {

    simplex[0] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);
    if (simplex[0].is_zero()) {
        // is our first point is the origin, then we're all good:
        return gjk_result::origin_at_vertex;
    }
    // for the next point in our simplex, we move in the 
    // direction first point -> origin in order to maximumise 
    // the probability of enclosing the origin as soon as possible:
    direction = static_cast<T>(-1) * simplex[0];
    simplex[1] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);
    if (simplex[1].is_zero()) {
        // is our second point is the origin, then we're all good:
        return gjk_result::origin_at_vertex;
    }
    // if our second point doesn't move past the origin then there won't be a simplex 
    // that contains the origin, and the shapes don't intersect
    if (direction.dot(simplex[1]) < 0) {
        return gjk_result::no_intersection;
    }
    const vector_2d<T> ab = simplex[1] - simplex[0];
    // now set the direction to be perpendicular to ab, pointing in 
    // the direction of the origin:
    direction = vector_triple_product(ab, ab, simplex[0]);
    size_t cursor = 2;

    int loops = N;
    while (--loops) {

        if (direction.is_zero()) {
            // if the direction vector here is the zero vector then the origin lays 
            // on the edge of the simplex (the edge that doesn't include the cursor point):
            if (cursor != 2) {
                std::swap(simplex[cursor], simplex[2]);
            }
            return gjk_result::origin_on_edge;
        }

        simplex[cursor] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);

        if (simplex[cursor].is_zero()) {
            return gjk_result::origin_at_vertex;
        }

        // if newly added point doesn't sit on the other side of the origin then 
        // the origin must be outside, and therefore our shapes don't intersect
        if (direction.dot(simplex[cursor]) < 0) {
            return gjk_result::no_intersection;
        }

        T alignment = calc_alignment(simplex[0], simplex[1], simplex[2]);

        // REVIEW: think about alignment == 0 case

        // if winding of simplex is clockwise, then swap elements to 
        // ensure counter-clockwise winding:
        if (alignment < 0) {
            std::swap(simplex[(cursor + 1) % 3], simplex[(cursor + 2) % 3]);
        }

        // a is our most recently added point and because we've passed the return condition above 
        // then the origin must be between the lines bc and the line parrallel to bc that passed 
        // through a:
        const vector_2d<T> &a = simplex[cursor];
        const vector_2d<T> &b = simplex[(cursor + 1) % 3];
        const vector_2d<T> &c = simplex[(cursor + 2) % 3];

        // ab_perpendicular will be perpendicular to ab in the direction that 
        // points away from the inside of the simplex:
        // (direction is guaranteed by the ccw simplex winding)
        const vector_2d<T> ab_perpendicular = { b.y - a.y, a.x - b.x };

        if (ab_perpendicular.dot(a) < 0) {

            // the origin sits outside ab
            cursor = (cursor + 2) % 3;
            direction = ab_perpendicular;
            continue;
            
        }

        // ac_perpendicular will be perpendicular to ac in the direction that 
        // points away from the inside of the simplex:
        // (direction is guaranteed by the ccw simplex winding)
        const vector_2d<T> ac_perpendicular = { a.y - c.y, c.x - a.x };

        if (ac_perpendicular.dot(a) < 0) {

            // the origin sits outside ac
            cursor = (cursor + 1) % 3;
            direction = ac_perpendicular;
            continue;

        }

        // ok, so the origin is on the inside of all three lines ab, bc, ac 
        // and therefore must be within the simplex:

        return gjk_result::origin_inside;

    }

    // unable to find intersection promptly, so assume it doesn't 
    // exist:
    return gjk_result::no_intersection;

}


#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>
#include <functional>

#include "./vector-2d.h"

// support function for a convex hull:
template <typename T>
vector_2D<T> convex_hull_support(const std::vector<vector_2D<T>> &convex_hull, const vector_2D<T> &direction) {

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
vector_2D<T> vector_triple_product(const vector_2D<T> &vector_a, const vector_2D<T> &vector_b, const vector_2D<T> &vector_c) {

    T a_dot_c = vector_a.dot(vector_c);
    T a_dot_b = vector_a.dot(vector_b);

    return {
        vector_b.x * a_dot_c - vector_c.x * a_dot_b,
        vector_b.y * a_dot_c - vector_c.y * a_dot_b
    };

}

template <typename T>
void print_points(const std::vector<vector_2D<T>> &points) {

    for (int i = 0, l = points.size(); i < l; i++) {

        std::cout << "(" << points[i].x << ", " << points[i].y << ") ";

    }

    std::cout << "\n";

}

// checks if the two convex_hulls overlap using the GJK algorithm
template <typename T>
bool gjk_intersects(const std::vector<vector_2D<T>> &convex_hull_a, const std::vector<vector_2D<T>> &convex_hull_b, vector_2D<T> direction = { 1, 0 }) {

    std::vector<vector_2D<T>> simplex(3);
    simplex[0] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);
    if (simplex[0].is_zero()) {
        // is our first point is the origin, then we're all good:
        return true;
    }
    // for the next point in our simplex, we move in the 
    // direction first point -> origin in order to maximumise 
    // the probability of enclosing the origin as soon as possible:
    direction = static_cast<T>(-1) * simplex[0];
    simplex[1] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);
    if (simplex[1].is_zero()) {
        // is our second point is the origin, then we're all good:
        return true;
    }
    // if our second point doesn't move past the origin then there won't be a simplex 
    // that contains the origin, and the shapes don't intersect
    if (direction.dot(simplex[1]) < 0) {
        return false;
    }
    const vector_2D<T> ab = simplex[1] - simplex[0];
    // now set the direction to be perpendicular to ab, pointing in 
    // the direction of the origin:
    direction = vector_triple_product(ab, ab, simplex[0]);
    int cursor = 2;

    while (true) {

        if (direction.is_zero()) {
            // if the direction vector here is the zero vector then the origin lays 
            // on the edge of the simplex:
            return true;
        }

        simplex[cursor] = convex_hull_support(convex_hull_a, direction) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * direction);

        if (simplex[cursor].is_zero()) {
            return true;
        }

        // if newly added point doesn't sit on the other side of the origin then 
        // the origin must be outside, and therefore our shapes don't intersect
        if (direction.dot(simplex[cursor]) < 0) {
            return false;
        }

        // a is our most recently added point and because we've passed the return condition above 
        // then the origin must be between the lines bc and the line parrallel to bc that passed 
        // through a:
        const vector_2D<T> &a = simplex[cursor];
        const vector_2D<T> &b = simplex[(cursor + 1) % 3];
        const vector_2D<T> &c = simplex[(cursor + 2) % 3];

        const vector_2D<T> ab = b - a;
        const vector_2D<T> ca = a - c;
        // ab_perpendicular will be perpendicular to ab in the direction that 
        // points away from the inside of the simplex:
        const vector_2D<T> ab_perpendicular = vector_triple_product(ab, ca, ab);

        if (ab_perpendicular.dot(a) < 0) {

            // the origin sits outside ab
            cursor = (cursor + 2) % 3;
            direction = ab_perpendicular;
            continue;
            
        }

        // ac_perpendicular will be perpendicular to ac in the direction that 
        // points away from the inside of the simplex:
        const vector_2D<T> ac_perpendicular = vector_triple_product(ca, ca, ab);

        if (ac_perpendicular.dot(a) < 0) {

            // the origin sits outside ac
            cursor = (cursor + 1) % 3;
            direction = ac_perpendicular;
            continue;

        }

        // ok, so the origin is on the inside of all three lines ab, bc, ac 
        // and therefore must be within the simplex:

        return true;

    }

}


#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>

#include "./vector-2d.h"

// determines if axis is a seperating axis for convex_hull_a and convex_hull_b by projecting each shape onto axis 
// and seeing if they overlap:
template <typename T>
bool is_seperating_axis(const std::vector<vector_2D<T>> &convex_hull_a, const std::vector<vector_2D<T>> &convex_hull_b, const vector_2D<T> &axis) {

    // NB: axis doesn't need normalising since that would just multiple the projections by a constant factor (|axis|)
    // and thus doesn't affect whether or not the projections of convex_hull_a and convex_hull_b overlap.

    // project convex_hull_a onto axis:
    T point_projection = axis.dot(convex_hull_a[0]);
    std::pair<T, T> projection_a = std::make_pair(point_projection, point_projection);
    for (size_t i = 1, l = convex_hull_a.size(); i < l; i++) {
        point_projection = axis.dot(convex_hull_a[i]);
        projection_a.first = std::min(projection_a.first, point_projection);
        projection_a.second = std::max(projection_a.second, point_projection);
    }

    // project convex_hull_b onto, checking for overlap as each point is considered:
    point_projection = axis.dot(convex_hull_b[0]);
    std::pair<T, T> projection_b = std::make_pair(point_projection, point_projection);
    for (size_t i = 1, l = convex_hull_b.size(); i < l; i++) {
        point_projection = axis.dot(convex_hull_b[i]);
        projection_b.first = std::min(projection_b.first, point_projection);
        projection_b.second = std::max(projection_b.second, point_projection);
        if (projection_a.second >= projection_b.first && projection_a.first <= projection_b.second) {
            return false;
        }
    }

    return true;

}

// checks if the two convex_hulls overlap using the Seperating Axis Theorem
// NB: assumes that the points in convex_hull_a and convex_hull_b are ordered (i.e. that they list the 
// vertices in clockwise or counter-clockwise order)
template <typename T>
bool sat_intersects(const std::vector<vector_2D<T>> &convex_hull_a, const std::vector<vector_2D<T>> &convex_hull_b) {

    // TODO: currently, if there are any parrallel sides between the two convex hulls, then 
    // we'll check that axis multiple times. can this be avoided cheaply?

    if (convex_hull_a.size() < 3 || convex_hull_b.size() < 3) {
        return false;
    }

    // checks the axises perpendicular to the sides of convex_hull_a:
    for (size_t i = 0, l = convex_hull_a.size(); i < l; i++) {
        const vector_2D<T> &point_a = convex_hull_a[i];
        const vector_2D<T> &point_b = convex_hull_a[(i + 1) % l];
        if (is_seperating_axis(convex_hull_a, convex_hull_b, { point_a.y - point_b.y, point_b.x - point_a.x })) {
            return false;
        }
    }

    // checks the axises perpendicular to the sides of convex_hull_b:
    for (size_t i = 0, l = convex_hull_b.size(); i < l; i++) {
        const vector_2D<T> &point_a = convex_hull_b[i];
        const vector_2D<T> &point_b = convex_hull_b[(i + 1) % l];
        if (is_seperating_axis(convex_hull_a, convex_hull_b, { point_a.y - point_b.y, point_b.x - point_a.x })) {
            return false;
        }
    }

    return true;

}

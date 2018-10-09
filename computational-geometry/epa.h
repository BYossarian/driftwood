
#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>
#include <functional>

#include "./vector-2d.h"

// calculates the shortest distance from ab to the origin
// NB: if angle 0ab is less than 180 deg, then this will return -ve distance
template <typename T>
T calc_edge_to_origin_distance(const vector_2d<T> &point_a, const vector_2d<T> &point_b) {

    vector_2d<T> normal = { point_b.y - point_a.y, point_a.x - point_b.x };
    normal.normalise();
    
    return normal.dot(point_a);

}

// calculates the penetration vector for intersecting shapes using the Expanding Polytope Algorithm:
// polytope should be a 2-simplex whose points lay on the boundary of convex_hull_a - convex_hull_b
template <typename T>
vector_2d<T> epa_get_penetration_vector(const std::vector<vector_2d<T>> &convex_hull_a, const std::vector<vector_2d<T>> &convex_hull_b, std::vector<vector_2d<T>> polytope) {

    if (polytope.size() != 3) {
        throw;
    }

    // check for zero vectors in polytope
    for (size_t i = 0; i < 3; i++) {
        // if the origin is in the polytope, then the origin 
        // is on the boundary of the Minkowski difference, so 
        // the shapes are just touching...
        if (polytope[i].is_zero()) {
            return { 0, 0 };
        }
    }

    T alignment = calc_alignment(polytope[0], polytope[1], polytope[2]);

    if (alignment == 0) {
        // co-linear...
        throw;
    }

    // if winding of polytope is clockwise, then swap elements to 
    // ensure counter-clockwise winding:
    if (alignment < 0) {
        std::swap(polytope[1], polytope[2]);
    }

    // so we're going to maintain the ccw winding in polytope as 
    // we add elements to it. We'll define edge_to_origin_dist such 
    // that the edge polytope[i] -> polytope[i + 1] will have a 
    // (shortest) distance to the origin of edge_to_origin_dist[i]:
    std::vector<T> edge_to_origin_dist(3);

    // calc edge_to_origin_dist for starting polytope:
    for (size_t i = 0; i < 3; i++) {

        // because we know that the winding of polytope is 
        // ccw, this will return the +ve distance:
        edge_to_origin_dist[i] = calc_edge_to_origin_distance(polytope[i], polytope[(i + 1) % 3]);

    }

    int loops = 10;

    while (--loops) {

        // find smallest edge_to_origin_dist
        T smallest_dist = edge_to_origin_dist[0];
        size_t smallest_dist_index = 0;

        for (size_t i = 1, l = edge_to_origin_dist.size(); i < l; i++) {

            if (edge_to_origin_dist[i] < smallest_dist) {
                smallest_dist = edge_to_origin_dist[i];
                smallest_dist_index = i;
            }

        }

        // expand the polytope by removing the edge closest to the origin 
        // and adding in a point:
        vector_2d<T> &point_a = polytope[smallest_dist_index];
        vector_2d<T> &point_b = polytope[(smallest_dist_index + 1) % polytope.size()];
        vector_2d<T> orthogonal = { point_b.y - point_a.y, point_a.x - point_b.x };
        vector_2d<T> new_point = convex_hull_support(convex_hull_a, orthogonal) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * orthogonal);

        // TODO: if insertion_index == 0 then we can actually, push the new elements to the end 
        // rather than inserting at the beginning...
        size_t insertion_index = (smallest_dist_index + 1) % polytope.size();

        if (polytope[insertion_index] == new_point) {
            // new_point already exists in polytope => haven't expanded polytope 
            orthogonal.normalise();
            return smallest_dist * orthogonal;
        }

        polytope.insert(polytope.begin() + insertion_index, new_point);
        edge_to_origin_dist.insert(edge_to_origin_dist.begin() + insertion_index, 
                calc_edge_to_origin_distance(polytope[insertion_index], polytope[(insertion_index + 1) % polytope.size()]));
        size_t precedes_insertion_index = (insertion_index == 0) ? polytope.size() - 1 : insertion_index - 1;
        if (polytope[precedes_insertion_index] == new_point) {
            // new_point already exists in polytope => haven't expanded polytope 
            orthogonal.normalise();
            return smallest_dist * orthogonal;
        }
        edge_to_origin_dist[precedes_insertion_index] = 
                calc_edge_to_origin_distance(polytope[precedes_insertion_index], polytope[insertion_index]);

        if (edge_to_origin_dist[insertion_index] <= smallest_dist && edge_to_origin_dist[precedes_insertion_index] <= smallest_dist) {
            // haven't actually extended the polytope => have hit the edge
            orthogonal.normalise();
            return smallest_dist * orthogonal;
        }
    }

    throw;

}

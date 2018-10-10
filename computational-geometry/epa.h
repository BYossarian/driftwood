
#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <utility>
#include <functional>

#include "./gjk.h"
#include "./vector-2d.h"

// calculates the shortest distance from ab to the origin
// NB: if angle 0ab is less than 180 deg, then this will return -ve distance
template <typename T>
T calc_edge_to_origin_distance(const vector_2d<T> &point_a, const vector_2d<T> &point_b) {

    vector_2d<T> normal = { point_b.y - point_a.y, point_a.x - point_b.x };
    normal.normalise();
    
    return normal.dot(point_a);

}

// an edge for the expanding polytope using in the EPA algorithm:
template <typename T>
struct epa_edge {
    vector_2d<T> start, end;
    T dist_to_origin;
};

template <typename T>
void print_edges(const std::vector<epa_edge<T>> &edges) {
    
    for (size_t i = 1, l = edges.size(); i < l; i++) {

        print_point(edges[i].start);
        std::cout << "-> ";
        print_point(edges[i].end);
        std::cout << ": " << edges[i].dist_to_origin << "\n";

    }

}

// calculates the penetration vector for intersecting shapes using the Expanding Polytope Algorithm:
// simplex and result should be straight from the GJK implementation.
template <typename T>
vector_2d<T> epa_get_penetration_vector(const std::vector<vector_2d<T>> &convex_hull_a, const std::vector<vector_2d<T>> &convex_hull_b, std::array<vector_2d<T>, 3> simplex, gjk_result result) {

    if (result == gjk_result::no_intersection || result == gjk_result::origin_at_vertex) {
        return { 0, 0 };
    } else if (result == gjk_result::origin_on_edge) {

        const vector_2d<T> &point_a = simplex[0];
        const vector_2d<T> &point_b = simplex[1];
        // this orthogonal points ccw when going from a to b, such that the newly added 
        // point will ensure that simplex has ccw winding:
        vector_2d<T> orthogonal = { point_a.y - point_b.y, point_b.x - point_a.x };
        simplex[2] = convex_hull_support(convex_hull_a, orthogonal) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * orthogonal);

        if (calc_edge_to_origin_distance(point_b, simplex[2]) == 0) {
            return { 0, 0 };
        }

    } else {

        T alignment = calc_alignment(simplex[0], simplex[1], simplex[2]);

        if (alignment == 0) {
            // co-linear...
            throw;
        }

        // if winding of simplex is clockwise, then swap elements to 
        // ensure counter-clockwise winding:
        if (alignment < 0) {
            std::swap(simplex[1], simplex[2]);
        }

    }

    // ok, so at this point, simplex should be three vertices, all on the boundary 
    // of the Minkowski difference of a and b; in ccw winding order

    // the edges of our expanding polytope:
    std::vector<epa_edge<T>> edges;
    edges.reserve(8);

    // calc edges for starting simplex:
    for (size_t i = 0; i < 3; i++) {

        edges.push_back({
            simplex[i],
            simplex[(i + 1) % 3],
            // because we know that the winding of simplex is 
            // ccw, this will return the +ve distance:
            calc_edge_to_origin_distance(simplex[i], simplex[(i + 1) % 3])
        });

    }

    int loops = 10;
    while (--loops) {

        // find smallest edge_to_origin_dist
        T smallest_dist = edges[0].dist_to_origin;
        size_t smallest_dist_index = 0;

        for (size_t i = 1, l = edges.size(); i < l; i++) {

            if (edges[i].dist_to_origin < smallest_dist) {
                smallest_dist = edges[i].dist_to_origin;
                smallest_dist_index = i;
            }

        }

        // expand the polytope by removing the edge closest to the origin 
        // and adding in a point:
        const vector_2d<T> &point_a = edges[smallest_dist_index].start;
        const vector_2d<T> &point_b = edges[smallest_dist_index].end;
        vector_2d<T> orthogonal = { point_b.y - point_a.y, point_a.x - point_b.x };
        vector_2d<T> new_point = convex_hull_support(convex_hull_a, orthogonal) - convex_hull_support(convex_hull_b, static_cast<T>(-1) * orthogonal);

        if (new_point == point_a || new_point == point_b) {
            // new_point already exists in polytope => haven't expanded polytope 
            orthogonal.normalise();
            return smallest_dist * orthogonal;
        }

        edges.push_back({
            new_point,
            point_b,
            calc_edge_to_origin_distance(new_point, point_b)
        });

        edges[smallest_dist_index].end = new_point;
        edges[smallest_dist_index].dist_to_origin = calc_edge_to_origin_distance(point_a, new_point);

        if (edges[smallest_dist_index].dist_to_origin <= smallest_dist && edges.back().dist_to_origin <= smallest_dist) {
            // haven't actually extended the polytope => have hit the edge
            orthogonal.normalise();
            return smallest_dist * orthogonal;
        }
    }

    throw;

}

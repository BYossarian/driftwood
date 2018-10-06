
#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

#include "./vector-2d.h"

// given three points - p1, p2, p3 - this returns:
// 0 if they are colinear;
// < 0 if going from p1->p2 to p2->p3 is a clockwise turn (i.e. the angle 
//      formed from p1-p2-p3 is less than 180 deg);
// > 0 if going from p1->p2 to p2->p3 is a counter-clockwise turn (i.e. the angle 
//      formed from p1-p2-p3 is more than 180 deg).
// NB: it's just calculating the 2D cross-product (p2 - p1) X (p3 - p1):
template <typename T>
T calc_alignment(const vector_2D<T> &p1, const vector_2D<T> &p2, const vector_2D<T> &p3) {

    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);

}

// takes a list of points and mutates the list such that it forms a convex hull 
// using Graham's scan algorithm.
// the mutated list of points will be the vertices of the convex hull listed 
// in counter-clockwise order, starting from the point with the smallest y 
// value. (in case of multiple such points, use one with smallest x as the tie-breaker)
template <typename T>
void make_convex_hull(std::vector<vector_2D<T>> &points) {

    if (points.size() < 3) {
        throw;
    }

    // 1) swap points[0] with the point with the smallest y
    // (in case of multiple such points, use one with smallest x)

    {

        size_t index_of_lowest = 0;
        T lowest_y = points[0].y;

        for (size_t i = 1, l = points.size(); i < l; i++) {

            if (points[i].y < lowest_y) {
                index_of_lowest = i;
                lowest_y = points[i].y;
            } else if (points[i].y == lowest_y) {
                if (points[i].x < points[index_of_lowest].x) {
                    index_of_lowest = i;
                    lowest_y = points[i].y;
                }
            }

        }

        std::swap(points[0], points[index_of_lowest]);

    }

    // 2) sort rest of points by polar angle with points[0]
    {

        // TODO: atan2 is presumably a relatively expensive operation, so cache values:
        std::sort(points.begin() + 1, points.end(), [&points](vector_2D<T> a, vector_2D<T> b) {
            return atan2(a.y - points[0].y, a.x - points[0].x) < atan2(b.y - points[0].y, b.x - points[0].x);
        });

    }

    // 3) iterate over points and add them to the convex hull if appropriate.
    // NB: we're doing this within points itself to avoid copying anything. top  
    // is the index of the current top of the 'stack' of points such that 
    // points[0 ... top] will always be a convex hull.
    // our stack begins with points[0 .. 2] hence, top starts at 2:
    size_t top = 2;

    // Since i >= top in our loop, we don't have to worry about the fact that we're 
    // mutating an array that's being iterated over:
    for (size_t i = 3, l = points.size(); i < l; i++) {

        while (calc_alignment(points[top - 1], points[top], points[i]) <= 0) {
            top--;
            if (top < 1) {
                // this should only happen if points begins with a sequence of 
                // colinear points (such that calc_alignment will return 0).
                // but as long as points[0...2] aren't colinear then this 
                // shouldn't be an issue:
                throw;
            }
        }
        top++;
        points[top] = points[i];

    }

    // deallocate memory for points not in the convex hull:
    points.resize(top + 1);
    points.shrink_to_fit();

}

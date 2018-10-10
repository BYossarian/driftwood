
#pragma once

#include <cmath>
#include <iostream>

template <typename T>
struct vector_2d {
    T x, y;
    inline T dot(const vector_2d<T> &other) const {
        return x * other.x + y * other.y;
    }
    inline bool is_zero() const {
        return x == 0 && y == 0;
    }
    inline void normalise() {
        T length = std::sqrt(x * x + y * y);
        // TODO: consider cases when / by 0 is not cool
        // ref: https://en.cppreference.com/w/cpp/types/numeric_limits/is_iec559
        x /= length;
        y /= length;
    }
};

template <typename T>
inline vector_2d<T> operator*(T scalar, const vector_2d<T>& vector) {
    return { scalar * vector.x, scalar * vector.y };
}

template <typename T>
inline vector_2d<T> operator+(const vector_2d<T>& vector_a, const vector_2d<T>& vector_b) {
    return { vector_a.x + vector_b.x, vector_a.y + vector_b.y };
}

template <typename T>
inline vector_2d<T> operator-(const vector_2d<T>& vector_a, const vector_2d<T>& vector_b) {
    return { vector_a.x - vector_b.x, vector_a.y - vector_b.y };
}

template <typename T>
inline bool operator==(const vector_2d<T>& vector_a, const vector_2d<T>& vector_b) {
    return vector_a.x == vector_b.x && vector_a.y == vector_b.y;
}

template <typename T>
void print_point(const vector_2d<T> &point) {

    std::cout << "(" << point.x << ", " << point.y << ") ";

}

template <typename T>
void print_points(const std::vector<vector_2d<T>> &points) {

    for (size_t i = 0, l = points.size(); i < l; i++) {

        std::cout << "(" << points[i].x << ", " << points[i].y << ") ";

    }

    std::cout << "\n";

}

template <typename T, std::size_t N>
void print_points(const std::array<vector_2d<T>, N> &points) {

    for (size_t i = 0; i < N; i++) {

        std::cout << "(" << points[i].x << ", " << points[i].y << ") ";

    }

    std::cout << "\n";

}

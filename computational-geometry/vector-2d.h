
#pragma once

template <typename T>
struct vector_2D {
    T x, y;
    inline T dot(const vector_2D<T> &other) const {
        return x * other.x + y * other.y;
    }
    inline bool is_zero() const {
        return x == 0 && y == 0;
    }
};

template <typename T>
inline vector_2D<T> operator*(T scalar, const vector_2D<T>& vector) {
    return { scalar * vector.x, scalar * vector.y };
}

template <typename T>
inline vector_2D<T> operator+(const vector_2D<T>& vector_a, const vector_2D<T>& vector_b) {
    return { vector_a.x + vector_b.x, vector_a.y + vector_b.y };
}

template <typename T>
inline vector_2D<T> operator-(const vector_2D<T>& vector_a, const vector_2D<T>& vector_b) {
    return { vector_a.x - vector_b.x, vector_a.y - vector_b.y };
}

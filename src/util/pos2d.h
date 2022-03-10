#pragma once

#include <stdint.h>

template <typename T, uint32_t GRID_SIZE = 0>
struct Pos2D
{
    T x, y;

    Pos2D() :x(0), y(0) {}
    Pos2D(T x, T y) : x(x), y(y) {}
    Pos2D(const Pos2D& v) : x(v.x), y(v.y) {}
    Pos2D(T id) : x(GRID_SIZE > 0 ? id % GRID_SIZE : 0), y(GRID_SIZE > 0 ? id / GRID_SIZE : 0) {}

    Pos2D& operator=(const Pos2D& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    Pos2D operator+(Pos2D& v)
    {
        return Pos2D(x + v.x, y + v.y);
    }
    Pos2D operator-(Pos2D& v)
    {
        return Pos2D(x - v.x, y - v.y);
    }

    Pos2D& operator+=(Pos2D& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Pos2D& operator-=(Pos2D& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    T& operator[](T i)
    {
        return *(reinterpret_cast<T*>(this) + i);
    }

    const T& operator[](T i) const
    {
        return *(reinterpret_cast<const T*>(this) + i);
    }

    inline void Set(T a_x, T a_y) { x = a_x; y = a_y; }
    inline uint8_t ToIndex() const { return GRID_SIZE > 0 ? (y * GRID_SIZE + x) : 0; }
};

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

    operator int8_t() const
    {
        return GRID_SIZE > 0 ? (y * GRID_SIZE + x) : 0;
    }

    T& operator[](T i)
    {
        return *(reinterpret_cast<T*>(this) + i);
    }

    const T& operator[](T i) const
    {
        return *(reinterpret_cast<const T*>(this) + i);
    }

    void Set(T x, T y)
    {
        this->x = x;
        this->y = y;
    }
};

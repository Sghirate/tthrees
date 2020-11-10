#pragma once

#include <stdint.h>

template <typename T, uint32_t GRID_SIZE = 0>
struct pos2d
{
    T x, y;

    pos2d() :x(0), y(0) {}
    pos2d(T x, T y) : x(x), y(y) {}
    pos2d(const pos2d& v) : x(v.x), y(v.y) {}
    pos2d(T id) : x(GRID_SIZE > 0 ? id % GRID_SIZE : 0), y(GRID_SIZE > 0 ? id / GRID_SIZE : 0) {}

    pos2d& operator=(const pos2d& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    pos2d operator+(pos2d& v)
    {
        return pos2d(x + v.x, y + v.y);
    }
    pos2d operator-(pos2d& v)
    {
        return pos2d(x - v.x, y - v.y);
    }

    pos2d& operator+=(pos2d& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    pos2d& operator-=(pos2d& v)
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

    void set(T x, T y)
    {
        this->x = x;
        this->y = y;
    }
};
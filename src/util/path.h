#pragma once

#include <stdio.h>

struct path
{
    static const char* get_home();
    static bool make_abs(char* out, int size_out, const char* base, const char* rel);
    template<int SIZE_OUT>
    static inline bool make_abs(char(&out)[SIZE_OUT], const char* base, const char* rel)
    {
        return make_abs(out, SIZE_OUT, base, rel);
    }
    static int file_exists(const char* path);
    static int dir_exists(const char* path);
};

template<int SIZE>
struct path_instance
{
    path_instance(const char* path) { snprintf(buffer, SIZE, "%s", path); }
    path_instance(const char* base, const char* rel) { path::make_abs(buffer, base, rel); }
    const char* c_str() const { return buffer; }
private:
    char buffer[SIZE];
};

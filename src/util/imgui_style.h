#pragma once

struct imgui_style
{
    static int load(const char* const path);
    static int save(const char* const path);
};

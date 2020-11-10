#pragma once

#include <imgui/imgui.h>

struct imgui_keys
{
    inline static bool any_mod(const ImGuiIO& io) { return io.KeyCtrl || io.KeyShift || io.KeyAlt || io.KeySuper; }
    inline static bool no_mod(const ImGuiIO& io) { return !io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper; }
    inline static bool only_ctrl(const ImGuiIO& io) { return io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper; }
    inline static bool only_alt(const ImGuiIO& io) { return !io.KeyCtrl && !io.KeyShift && io.KeyAlt && !io.KeySuper; }
    inline static bool only_shift(const ImGuiIO& io) { return !io.KeyCtrl && io.KeyShift && !io.KeyAlt && !io.KeySuper; }
    inline static bool only_ctrl_alt(const ImGuiIO& io) { return io.KeyCtrl && !io.KeyShift && io.KeyAlt && !io.KeySuper; }
    inline static bool only_ctrl_shift(const ImGuiIO& io) { return io.KeyCtrl && io.KeyShift && !io.KeyAlt && !io.KeySuper; }
    inline static bool only_alt_shift(const ImGuiIO& io) { return !io.KeyCtrl && io.KeyShift && io.KeyAlt && !io.KeySuper; }
};

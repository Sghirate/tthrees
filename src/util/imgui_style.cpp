#include <util/imgui_style.h>
#include <util/path.h>
#include <Yaml.hpp>
#include <imgui/imgui.h>

namespace
{
    void try_read_style_float(Yaml::Node &parent, const char *name, float &v, bool print_err = true)
    {
        Yaml::Node &node = parent[name];
        if (!node.IsNone())
        {
            std::string value = node.As<std::string>();
            float x = 0.0f;
            if (sscanf(value.c_str(), "%f", &x) == 1)
            {
                v = x;
            }
            else if (print_err)
            {
                fprintf(stderr, "Could not parse float '%s=%s'\n", name, value.c_str());
            }
        }
    }

    void write_style_float(Yaml::Node &parent, const char *name, const float &v)
    {
        enum { MAX_OUT_STRING = 32 };
        char buffer[MAX_OUT_STRING];
        snprintf(buffer, MAX_OUT_STRING, "%f", v);
    }

    void try_read_style_float2(Yaml::Node &parent, const char *name, ImVec2 &v, bool print_err = true)
    {
        Yaml::Node &node = parent[name];
        if (!node.IsNone())
        {
            std::string value = node.As<std::string>();
            float x = 0.0f;
            float y = 0.0f;
            if (sscanf(value.c_str(), "%f,%f", &x, &y) == 2)
            {
                v.x = x;
                v.y = y;
            }
            else if (print_err)
            {
                fprintf(stderr, "Could not parse float2 '%s=%s'\n", name, value.c_str());
            }
        }
    }

    void write_style_float2(Yaml::Node &parent, const char *name, const ImVec2 &v)
    {
        enum { MAX_OUT_STRING = 64 };
        char buffer[MAX_OUT_STRING];
        snprintf(buffer, MAX_OUT_STRING, "%f,%f", v.x, v.y);
        parent[name] = buffer;
    }

    void try_read_style_float4(Yaml::Node &parent, const char *name, ImVec4 &v, bool print_err = true)
    {
        Yaml::Node &node = parent[name];
        if (!node.IsNone())
        {
            std::string value = node.As<std::string>();
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float w = 0.0f;
            if (sscanf(value.c_str(), "%f,%f,%f,%f", &x, &y, &z, &w) == 4)
            {
                v.x = x;
                v.y = y;
                v.z = z;
                v.w = w;
            }
            else if (print_err)
            {
                fprintf(stderr, "Could not parse float4 '%s=%s'\n", name, value.c_str());
            }
        }
    }

    void write_style_float4(Yaml::Node &parent, const char *name, const ImVec4 &v)
    {
        enum
        {
            MAX_OUT_STRING = 128
        };
        char buffer[MAX_OUT_STRING];
        snprintf(buffer, MAX_OUT_STRING, "%f,%f,%f,%f", v.x, v.y, v.z, v.w);
        parent[name] = buffer;
    }

    void try_read_style_bool(Yaml::Node &parent, const char *name, bool &v, bool print_err = true)
    {
        Yaml::Node &node = parent[name];
        if (!node.IsNone())
        {
            std::string value = node.As<std::string>();
            int i = 0;
            if (sscanf(value.c_str(), "%i", &i) == 1)
            {
                v = (i > 1);
            }
            else if (print_err)
            {
                fprintf(stderr, "Could not parse bool '%s=%s'\n", name, value.c_str());
            }
        }
    }

    void write_style_bool(Yaml::Node &parent, const char *name, const bool &v)
    {
        parent[name] = v ? "1" : "0";
    }
} // namespace

int imgui_style::load(const char *const path)
{
    if (path::file_exists(path) != 0)
    {
        return -1;
    }

    ImGuiStyle &style = ImGui::GetStyle();

    Yaml::Node cfg_root;
    Yaml::Parse(cfg_root, path);

    Yaml::Node &cfg_sizes = cfg_root["Sizes"];
    if (!cfg_sizes.IsNone())
    {
        try_read_style_float2(cfg_sizes, "WindowPadding", style.WindowPadding);
        try_read_style_float2(cfg_sizes, "FramePadding", style.FramePadding);
        try_read_style_float2(cfg_sizes, "ItemSpacing", style.ItemSpacing);
        try_read_style_float2(cfg_sizes, "ItemInnerSpacing", style.ItemInnerSpacing);
        try_read_style_float2(cfg_sizes, "TouchExtraPadding", style.TouchExtraPadding);
        try_read_style_float(cfg_sizes, "IndentSpacing", style.IndentSpacing);
        try_read_style_float(cfg_sizes, "ScrollbarSize", style.ScrollbarSize);
        try_read_style_float(cfg_sizes, "GrabMinSize", style.GrabMinSize);
        try_read_style_float(cfg_sizes, "WindowBorderSize", style.WindowBorderSize);
        try_read_style_float(cfg_sizes, "ChildBorderSize", style.ChildBorderSize);
        try_read_style_float(cfg_sizes, "PopupBorderSize", style.PopupBorderSize);
        try_read_style_float(cfg_sizes, "FrameBorderSize", style.FrameBorderSize);
        try_read_style_float(cfg_sizes, "TabBorderSize", style.TabBorderSize);
        try_read_style_float(cfg_sizes, "WindowRounding", style.WindowRounding);
        try_read_style_float(cfg_sizes, "ChildRounding", style.ChildRounding);
        try_read_style_float(cfg_sizes, "FrameRounding", style.FrameRounding);
        try_read_style_float(cfg_sizes, "PopupRounding", style.PopupRounding);
        try_read_style_float(cfg_sizes, "ScrollbarRounding", style.ScrollbarRounding);
        try_read_style_float(cfg_sizes, "GrabRounding", style.GrabRounding);
        try_read_style_float(cfg_sizes, "TabRounding", style.TabRounding);
        try_read_style_float2(cfg_sizes, "WindowTitleAlign", style.WindowTitleAlign);
        try_read_style_float2(cfg_sizes, "ButtonTextAlign", style.ButtonTextAlign);
        try_read_style_float2(cfg_sizes, "SelectableTextAlign", style.SelectableTextAlign);
        try_read_style_float2(cfg_sizes, "DisplaySafeAreaPadding", style.DisplaySafeAreaPadding);
        // ImGui::Text("Safe Area Padding"); ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
        // int window_menu_button_position = style.WindowMenuButtonPosition + 1;
        // if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
        // 	style.WindowMenuButtonPosition = window_menu_button_position - 1;
        // ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
    }

    Yaml::Node &cfg_colors = cfg_root["Colors"];
    if (!cfg_colors.IsNone())
    {
        ImVec4 *colors = ImGui::GetStyle().Colors;
        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            ImGuiCol_ col = static_cast<ImGuiCol_>(i);
            const char *col_name = ImGui::GetStyleColorName(col);
            ImVec4 &color = colors[col];
            try_read_style_float4(cfg_colors, col_name, color);
        }
    }

    // todo: fonts

    Yaml::Node &cfg_rendering = cfg_root["Rendering"];
    if (!cfg_rendering.IsNone())
    {
        try_read_style_bool(cfg_rendering, "AntiAliasedLines", style.AntiAliasedLines);
        try_read_style_bool(cfg_rendering, "AntiAliasedFill", style.AntiAliasedFill);
        try_read_style_float(cfg_rendering, "CurveTessellationTolerance", style.CurveTessellationTol);
        try_read_style_float(cfg_rendering, "Alpha", style.Alpha);
    }

    return 0;
}

int imgui_style::save(const char *const path)
{
    ImGuiStyle &style = ImGui::GetStyle();

    Yaml::Node cfg_root;

    Yaml::Node &cfg_sizes = cfg_root["Sizes"];
    write_style_float2(cfg_sizes, "WindowPadding", style.WindowPadding);
    write_style_float2(cfg_sizes, "FramePadding", style.FramePadding);
    write_style_float2(cfg_sizes, "ItemSpacing", style.ItemSpacing);
    write_style_float2(cfg_sizes, "ItemInnerSpacing", style.ItemInnerSpacing);
    write_style_float2(cfg_sizes, "TouchExtraPadding", style.TouchExtraPadding);
    write_style_float(cfg_sizes, "IndentSpacing", style.IndentSpacing);
    write_style_float(cfg_sizes, "ScrollbarSize", style.ScrollbarSize);
    write_style_float(cfg_sizes, "GrabMinSize", style.GrabMinSize);
    write_style_float(cfg_sizes, "WindowBorderSize", style.WindowBorderSize);
    write_style_float(cfg_sizes, "ChildBorderSize", style.ChildBorderSize);
    write_style_float(cfg_sizes, "PopupBorderSize", style.PopupBorderSize);
    write_style_float(cfg_sizes, "FrameBorderSize", style.FrameBorderSize);
    write_style_float(cfg_sizes, "TabBorderSize", style.TabBorderSize);
    write_style_float(cfg_sizes, "WindowRounding", style.WindowRounding);
    write_style_float(cfg_sizes, "ChildRounding", style.ChildRounding);
    write_style_float(cfg_sizes, "FrameRounding", style.FrameRounding);
    write_style_float(cfg_sizes, "PopupRounding", style.PopupRounding);
    write_style_float(cfg_sizes, "ScrollbarRounding", style.ScrollbarRounding);
    write_style_float(cfg_sizes, "GrabRounding", style.GrabRounding);
    write_style_float(cfg_sizes, "TabRounding", style.TabRounding);
    write_style_float2(cfg_sizes, "WindowTitleAlign", style.WindowTitleAlign);
    write_style_float2(cfg_sizes, "ButtonTextAlign", style.ButtonTextAlign);
    write_style_float2(cfg_sizes, "SelectableTextAlign", style.SelectableTextAlign);
    write_style_float2(cfg_sizes, "DisplaySafeAreaPadding", style.DisplaySafeAreaPadding);

    ImVec4 *colors = ImGui::GetStyle().Colors;
    Yaml::Node &cfg_colors = cfg_root["Colors"];
    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        ImGuiCol_ col = static_cast<ImGuiCol_>(i);
        const char *col_name = ImGui::GetStyleColorName(col);
        write_style_float4(cfg_colors, col_name, colors[col]);
    }

    // todo: fonts

    Yaml::Node &cfg_rendering = cfg_root["Rendering"];
    write_style_bool(cfg_rendering, "AntiAliasedLines", style.AntiAliasedLines);
    write_style_bool(cfg_rendering, "AntiAliasedFill", style.AntiAliasedFill);
    write_style_float(cfg_rendering, "CurveTessellationTolerance", style.CurveTessellationTol);
    write_style_float(cfg_rendering, "Alpha", style.Alpha);

    Yaml::Serialize(cfg_root, path);

    return 0;
}

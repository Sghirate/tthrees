#pragma once

#include <stdarg.h>
#include <stdint.h>

struct TUI
{

enum class EColors : uint8_t
{
	Black                  = 0,
	Blue                   = 1,
	Green                  = 2,
	Cyan                   = 3,
	Red                    = 4,
	Magenta                = 5,
	Brown                  = 6,
	LightGray              = 7,
	DarkGray               = 8,
	LightBlue              = 9,
	LightGreen             = 10,
	LightCyan              = 11,
	LightRed               = 12,
	LightMagenta           = 13,
	Yellow                 = 14,
	White                  = 15,
};
enum class EKeys : uint8_t
{
	Key_None               = 0,
	Key_Space              = 1,
	Key_Apostrophe         = 2,
	Key_Comma              = 3,
	Key_Minus              = 4,
	Key_Period             = 5,
	Key_Slash              = 6,
	Key_0                  = 7,
	Key_1                  = 8,
	Key_2                  = 9,
	Key_3                  = 10,
	Key_4                  = 11,
	Key_5                  = 12,
	Key_6                  = 13,
	Key_7                  = 14,
	Key_8                  = 15,
	Key_9                  = 16,
	Key_Semicolon          = 17,
	Key_Equal              = 18,
	Key_A                  = 19,
	Key_B                  = 20,
	Key_C                  = 21,
	Key_D                  = 22,
	Key_E                  = 23,
	Key_F                  = 24,
	Key_G                  = 25,
	Key_H                  = 26,
	Key_I                  = 27,
	Key_J                  = 28,
	Key_K                  = 29,
	Key_L                  = 30,
	Key_M                  = 31,
	Key_N                  = 32,
	Key_O                  = 33,
	Key_P                  = 34,
	Key_Q                  = 35,
	Key_R                  = 36,
	Key_S                  = 37,
	Key_T                  = 38,
	Key_U                  = 39,
	Key_V                  = 40,
	Key_W                  = 41,
	Key_X                  = 42,
	Key_Y                  = 43,
	Key_Z                  = 44,
	Key_LeftBracket        = 45,
	Key_Backslash          = 46,
	Key_RightBracket       = 47,
	Key_GraveAccent        = 48,
	Key_World1             = 49,
	Key_World2             = 50,
	Key_Escape             = 51,
	Key_Enter              = 52,
	Key_Tab                = 53,
	Key_Backspace          = 54,
	Key_Insert             = 55,
	Key_Delete             = 56,
	Key_Right              = 57,
	Key_Left               = 58,
	Key_Down               = 59,
	Key_Up                 = 60,
	Key_PageUp             = 61,
	Key_PageDown           = 62,
	Key_Home               = 63,
	Key_End                = 64,
	Key_CapsLock           = 65,
	Key_ScrollLock         = 66,
	Key_NumLock            = 67,
	Key_PrintScreen        = 68,
	Key_Pause              = 69,
	Key_F1                 = 70,
	Key_F2                 = 71,
	Key_F3                 = 72,
	Key_F4                 = 73,
	Key_F5                 = 74,
	Key_F6                 = 75,
	Key_F7                 = 76,
	Key_F8                 = 77,
	Key_F9                 = 78,
	Key_F10                = 79,
	Key_F11                = 80,
	Key_F12                = 81,
	Key_F13                = 82,
	Key_F14                = 83,
	Key_F15                = 84,
	Key_F16                = 85,
	Key_F17                = 86,
	Key_F18                = 87,
	Key_F19                = 88,
	Key_F20                = 89,
	Key_F21                = 90,
	Key_F22                = 91,
	Key_F23                = 92,
	Key_F24                = 93,
	Key_F25                = 94,
	Key_Kp0                = 95,
	Key_Kp1                = 96,
	Key_Kp2                = 97,
	Key_Kp3                = 98,
	Key_Kp4                = 99,
	Key_Kp5                = 100,
	Key_Kp6                = 101,
	Key_Kp7                = 102,
	Key_Kp8                = 103,
	Key_Kp9                = 104,
	Key_KpDecimal          = 105,
	Key_KpDivide           = 106,
	Key_KpMultiply         = 107,
	Key_KpSubtract         = 108,
	Key_KpAdd              = 109,
	Key_KpEnter            = 110,
	Key_KpEqual            = 111,
	Key_LeftShift          = 112,
	Key_LeftControl        = 113,
	Key_LeftAlt            = 114,
	Key_LeftSuper          = 115,
	Key_RightShift         = 116,
	Key_RightControl       = 117,
	Key_RightAlt           = 118,
	Key_RightSuper         = 119,
	Key_Menu               = 120,

	Key_COUNT
};
enum class EModifiers : uint8_t
{
	Modifier_None          = 0x0,
	Modifier_Control       = 0x1,
	Modifier_Shift         = 0x2,
	Modifier_Alt           = 0x4,

	Modifier_COUNT
};

union Color
{
	uint8_t raw;
	struct
	{
		EColors foreground : 4;
		EColors background : 4;
	};

	Color(uint8_t a_raw = 0) : raw(a_raw) {}
	Color(EColors a_foreground, EColors a_background) : foreground(a_foreground), background(a_background) {}

	operator uint8_t() const { return raw; }
};
static Color s_color;
struct ColorScope
{
	ColorScope(Color a_color) :m_oldColor(s_color)  { SetColor(a_color); }
	ColorScope(EColors a_foreground, EColors a_background) : m_oldColor(s_color) { SetColor(a_foreground, a_background); }
	~ColorScope() { SetColor(m_oldColor); }

private:
	Color m_oldColor;
};

static void Init(bool a_doubleBuffered = true);
static void Shutdown();
static void BeginFrame(bool& out_sizeChanged);
static void EndFrame(int a_targetFps = 60);
static void ClearScreen();
static float GetDeltaSeconds(float a_max = 0.03f);
static bool IsKeyPressed(EKeys a_key, EModifiers a_modifiers = EModifiers::Modifier_None);
static void GetSize(int& out_w, int& out_h);
static void DrawLine(int a_fromX, int a_fromY, int a_toX, int a_toY);
static void DrawRect(int a_x, int a_y, int a_w, int a_h);
static void DrawChar(int a_x, int a_y, char a_c);
static void DrawTextV(int a_x, int a_y, const char* a_format, va_list args);
inline static void SetColor(EColors a_foreground, EColors a_background) { s_color = Color(a_foreground, a_background); }
inline static void SetColor(Color a_color) { s_color = a_color; }
inline static Color GetColor() { return s_color; };
inline static void DrawLine(Color a_color, int a_fromX, int a_fromY, int a_toX, int a_toY)
{
    ColorScope color(a_color);
    DrawLine(a_fromX, a_fromY, a_toX, a_toY);
}
inline static void DrawRect(Color a_color, int a_x, int a_y, int a_w, int a_h)
{
    ColorScope color(a_color);
    DrawRect(a_x, a_y, a_w, a_h);
}
inline static void DrawChar(Color a_color, int a_x, int a_y, char a_c)
{
    ColorScope color(a_color);
    DrawChar(a_x, a_y, a_c);
}
inline static void DrawText(int a_x, int a_y, const char* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    DrawTextV(a_x, a_y, a_format, args);
    va_end(args);
}
inline static void DrawText(Color a_color, int a_x, int a_y, const char* a_format, ...)
{
    ColorScope color(a_color);
    va_list args;
    va_start(args, a_format);
    DrawTextV(a_x, a_y, a_format, args);
    va_end(args);
}

};

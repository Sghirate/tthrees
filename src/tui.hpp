#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

struct TUI
{
    enum class EColors : uint8_t
    {
        Black        = 0,
        Blue         = 1,
        Green        = 2,
        Cyan         = 3,
        Red          = 4,
        Magenta      = 5,
        Brown        = 6,
        LightGray    = 7,
        DarkGray     = 8,
        LightBlue    = 9,
        LightGreen   = 10,
        LightCyan    = 11,
        LightRed     = 12,
        LightMagenta = 13,
        Yellow       = 14,
        White        = 15,
    };
    enum class EKeys : uint8_t
    {
        Key_None         = 0,
        Key_Space        = 1,
        Key_Apostrophe   = 2,
        Key_Comma        = 3,
        Key_Minus        = 4,
        Key_Period       = 5,
        Key_Slash        = 6,
        Key_0            = 7,
        Key_1            = 8,
        Key_2            = 9,
        Key_3            = 10,
        Key_4            = 11,
        Key_5            = 12,
        Key_6            = 13,
        Key_7            = 14,
        Key_8            = 15,
        Key_9            = 16,
        Key_Semicolon    = 17,
        Key_Equal        = 18,
        Key_A            = 19,
        Key_B            = 20,
        Key_C            = 21,
        Key_D            = 22,
        Key_E            = 23,
        Key_F            = 24,
        Key_G            = 25,
        Key_H            = 26,
        Key_I            = 27,
        Key_J            = 28,
        Key_K            = 29,
        Key_L            = 30,
        Key_M            = 31,
        Key_N            = 32,
        Key_O            = 33,
        Key_P            = 34,
        Key_Q            = 35,
        Key_R            = 36,
        Key_S            = 37,
        Key_T            = 38,
        Key_U            = 39,
        Key_V            = 40,
        Key_W            = 41,
        Key_X            = 42,
        Key_Y            = 43,
        Key_Z            = 44,
        Key_LeftBracket  = 45,
        Key_Backslash    = 46,
        Key_RightBracket = 47,
        Key_GraveAccent  = 48,
        Key_World1       = 49,
        Key_World2       = 50,
        Key_Escape       = 51,
        Key_Enter        = 52,
        Key_Tab          = 53,
        Key_Backspace    = 54,
        Key_Insert       = 55,
        Key_Delete       = 56,
        Key_Right        = 57,
        Key_Left         = 58,
        Key_Down         = 59,
        Key_Up           = 60,
        Key_PageUp       = 61,
        Key_PageDown     = 62,
        Key_Home         = 63,
        Key_End          = 64,
        Key_CapsLock     = 65,
        Key_ScrollLock   = 66,
        Key_NumLock      = 67,
        Key_PrintScreen  = 68,
        Key_Pause        = 69,
        Key_F1           = 70,
        Key_F2           = 71,
        Key_F3           = 72,
        Key_F4           = 73,
        Key_F5           = 74,
        Key_F6           = 75,
        Key_F7           = 76,
        Key_F8           = 77,
        Key_F9           = 78,
        Key_F10          = 79,
        Key_F11          = 80,
        Key_F12          = 81,
        Key_F13          = 82,
        Key_F14          = 83,
        Key_F15          = 84,
        Key_F16          = 85,
        Key_F17          = 86,
        Key_F18          = 87,
        Key_F19          = 88,
        Key_F20          = 89,
        Key_F21          = 90,
        Key_F22          = 91,
        Key_F23          = 92,
        Key_F24          = 93,
        Key_F25          = 94,
        Key_Kp0          = 95,
        Key_Kp1          = 96,
        Key_Kp2          = 97,
        Key_Kp3          = 98,
        Key_Kp4          = 99,
        Key_Kp5          = 100,
        Key_Kp6          = 101,
        Key_Kp7          = 102,
        Key_Kp8          = 103,
        Key_Kp9          = 104,
        Key_KpDecimal    = 105,
        Key_KpDivide     = 106,
        Key_KpMultiply   = 107,
        Key_KpSubtract   = 108,
        Key_KpAdd        = 109,
        Key_KpEnter      = 110,
        Key_KpEqual      = 111,
        Key_LeftShift    = 112,
        Key_LeftControl  = 113,
        Key_LeftAlt      = 114,
        Key_LeftSuper    = 115,
        Key_RightShift   = 116,
        Key_RightControl = 117,
        Key_RightAlt     = 118,
        Key_RightSuper   = 119,
        Key_Menu         = 120,

        Key_COUNT
    };
    enum class EModifiers : uint8_t
    {
        Modifier_None    = 0x0,
        Modifier_Control = 0x1,
        Modifier_Shift   = 0x2,
        Modifier_Alt     = 0x4,

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

        Color(uint8_t a_raw = 0)
            : raw(a_raw)
        {
        }
        Color(EColors a_foreground, EColors a_background)
            : foreground(a_foreground)
            , background(a_background)
        {
        }

        operator uint8_t() const { return raw; }
    };
    static Color s_color;
    struct ColorScope
    {
        ColorScope(Color a_color)
            : m_oldColor(s_color)
        {
            SetColor(a_color);
        }
        ColorScope(EColors a_foreground, EColors a_background)
            : m_oldColor(s_color)
        {
            SetColor(a_foreground, a_background);
        }
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
    static void DrawLine(int a_fromX, int a_fromY, int a_toX, int a_toY, char a_char = ' ');
    static void DrawRect(int a_x, int a_y, int a_w, int a_h, char a_char = ' ');
    static void DrawChar(int a_x, int a_y, char a_c);
    static void DrawTextV(int a_x, int a_y, const char* a_format, va_list args);
    inline static void SetColor(EColors a_foreground, EColors a_background) { s_color = Color(a_foreground, a_background); }
    inline static void SetColor(Color a_color) { s_color = a_color; }
    inline static Color GetColor() { return s_color; };
    inline static void DrawLine(Color a_color, int a_fromX, int a_fromY, int a_toX, int a_toY, char a_char = ' ')
    {
        ColorScope color(a_color);
        DrawLine(a_fromX, a_fromY, a_toX, a_toY, a_char);
    }
    inline static void DrawRect(Color a_color, int a_x, int a_y, int a_w, int a_h, char a_char = ' ')
    {
        ColorScope color(a_color);
        DrawRect(a_x, a_y, a_w, a_h, a_char);
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

#ifdef TUI_IMPLEMENTATION
#include <chrono>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>

TUI::Color TUI::s_color(TUI::EColors::White, TUI::EColors::Black);

namespace TUI_Shared
{

template <size_t BITS>
struct BitSet
{
    struct Reference
    {
        Reference& operator=(bool a_value)
        {
            m_bitSet->Set(m_bit, a_value);
            return *this;
        }
        Reference& Flip()
        {
            m_bitSet->Flip(m_bit);
            return *this;
        }
        bool operator~() const
        {
            return !m_bitSet->Get(m_bit);
        }
        operator bool() const
        {
            return m_bitSet->Get(m_bit);
        }

    private:
        friend struct BitSet<BITS>;
        Reference(BitSet* a_bitSet, int a_bit)
            : m_bitSet(a_bitSet)
            , m_bit(a_bit)
        {
        }

        BitSet* m_bitSet;
        int m_bit;
    };

    BitSet() { Reset(); }

    inline void Reset() { memset(m_bytes, 0, sizeof(uint8_t) * kByteCount); }
    inline bool Get(int a_bit) const
    {
        const uint8_t idx  = a_bit / 8;
        const uint8_t mask = 1 << (a_bit % 8);
        return (m_bytes[idx] & mask) == mask;
    }
    inline void Set(int a_bit, bool a_value)
    {
        const uint8_t idx  = a_bit / 8;
        const uint8_t mask = 1 << (a_bit % 8);
        if (a_value)
            m_bytes[idx] |= mask;
        else
            m_bytes[idx] &= ~mask;
    }
    inline void Flip(int a_bit)
    {
        const uint8_t idx  = a_bit / 8;
        const uint8_t mask = 1 << (a_bit % 8);
        m_bytes[idx] ^= mask;
    }
    Reference operator[](int a_bit) { return Reference(this, a_bit); }

private:
    enum
    {
        kByteCount = (BITS / 8 + (BITS % 8 != 0))
    };
    uint8_t m_bytes[kByteCount];
};

static struct FrameTimer
{
    std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaSeconds;

    void BeginFrame()
    {
        frameStart = std::chrono::high_resolution_clock::now();
    }
    void EndFrame(int a_targetFps = 60)
    {
        if (a_targetFps > 0)
        {
            const double targetMs                                   = 1000.0 / static_cast<double>(a_targetFps);
            auto frameEnd                                           = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> frameDuration = frameEnd - frameStart;
            if (frameDuration.count() < targetMs)
            {
                std::chrono::duration<double, std::milli> deltaMs(targetMs - frameDuration.count());
                auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(deltaMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration.count()));
            }
        }
        deltaSeconds = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - frameStart);
    }
    float GetDeltaSeconds(float a_max = 0.03f) const
    {
        std::chrono::duration<float> max(a_max);
        return (deltaSeconds > max ? max : deltaSeconds).count();
    }
} g_frameTimer;

struct Input
{
    BitSet<128> keys;
    uint8_t modifiers = 0;
} g_input;

union Cell
{
    uint16_t raw;
    struct
    {
        uint8_t color;
        uint8_t value;
    };

    Cell(uint16_t a_raw = 0)
        : raw(a_raw)
    {
    }
    Cell(uint8_t a_color, uint8_t a_value)
        : color(a_color)
        , value(a_value)
    {
    }

    operator int() const { return raw; }
};
struct Buffer
{
    static char s_eraseChar;
    uint16_t width  = 0;
    uint16_t height = 0;
    std::vector<Cell> data;

    void Clear()
    {
        Cell c(TUI::Color(0), s_eraseChar);
        std::fill(data.begin(), data.end(), c);
    }
    void Resize(uint16_t a_width, uint16_t a_height)
    {
        width  = a_width;
        height = a_height;
        data.resize(width * height);
        Clear();
    }
    Cell& operator()(uint16_t a_x, uint16_t a_y) { return data[a_y * width + a_x]; }
    const Cell& operator()(uint16_t a_x, uint16_t a_y) const { return data[a_y * width + a_x]; }
} g_consoleData;
char Buffer::s_eraseChar = ' ';

} // namespace TUI_Shared

void TUI::ClearScreen()
{
    TUI_Shared::g_consoleData.Clear();
}

float TUI::GetDeltaSeconds(float a_max)
{
    return TUI_Shared::g_frameTimer.GetDeltaSeconds(a_max);
}

bool TUI::IsKeyPressed(EKeys a_key, EModifiers a_modifiers)
{
    const int key      = (int)a_key;
    const uint8_t mods = (uint8_t)a_modifiers;
    return a_key < EKeys::Key_COUNT && mods == TUI_Shared::g_input.modifiers && TUI_Shared::g_input.keys[key];
}

void TUI::GetSize(int& out_w, int& out_h)
{
    out_w = TUI_Shared::g_consoleData.width;
    out_h = TUI_Shared::g_consoleData.height;
}

void TUI::DrawLine(int a_fromX, int a_fromY, int a_toX, int a_toY, char a_char)
{
    int w, h;
    GetSize(w, h);
    const int incX = (a_toX - a_fromX) < 0 ? -1 : ((a_toX - a_fromX) > 0 ? 1 : 0);
    const int incY = (a_toY - a_fromY) < 0 ? -1 : ((a_toY - a_fromY) > 0 ? 1 : 0);
    const int dx   = abs(a_toX - a_fromX);
    const int dy   = abs(a_toY - a_fromY);

    const int xStart = std::max(0, std::min(a_fromX, a_toX));
    const int xEnd   = std::min(w - 1, std::max(a_fromX, a_toX));
    const int yStart = std::max(0, std::min(a_fromY, a_toY));
    const int yEnd   = std::min(h - 1, std::max(a_fromY, a_toY));

    const TUI_Shared::Cell c(s_color, a_char);
    if (dy == 0 && a_fromY >= 0 && a_fromY < h) // horizontal line
    {
        for (int x = xStart; x <= xEnd; ++x)
            TUI_Shared::g_consoleData(x, a_fromY) = c;
    }
    else if (dx == 0 && a_fromX >= 0 && a_fromX < w) // vertical line
    {
        for (int y = yStart; y <= yEnd; ++y)
            TUI_Shared::g_consoleData(a_fromX, y) = c;
    }
    else if (dx >= dy) // more horizontal than vertical
    {
        int slope    = 2 * dy;
        int error    = -dx;
        int errorInc = -2 * dx;
        int y        = a_fromY;
        for (int x = a_fromX; x != a_toX + incX; x += incX)
        {
            if (x > 0 && x < w && y > 0 && y < h)
                TUI_Shared::g_consoleData(x, y) = c;
            error += slope;
            if (error >= 0)
            {
                y += incY;
                error += errorInc;
            }
        }
    }
    else // more vertical than horizontal
    {
        int slope    = 2 * dx;
        int error    = -dy;
        int errorInc = -2 * dy;
        int x        = a_fromX;
        for (int y = a_fromY; y != a_toY + incY; y += incY)
        {
            if (x > 0 && x < w && y > 0 && y < h)
                TUI_Shared::g_consoleData(x, y) = c;
            error += slope;
            if (error >= 0)
            {
                x += incX;
                error += errorInc;
            }
        }
    }
}

void TUI::DrawRect(int a_x, int a_y, int a_w, int a_h, char a_char)
{
    int w, h;
    GetSize(w, h);
    const int x0 = std::max(0, std::min(a_x, a_x + a_w));
    const int x1 = std::min(w, std::max(a_x, a_x + a_w));
    const int y0 = std::max(0, std::min(a_y, a_y + a_h));
    const int y1 = std::min(h, std::max(a_y, a_y + a_h));

    const TUI_Shared::Cell c(s_color, a_char);
    for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x)
            TUI_Shared::g_consoleData(x, y) = c;
}

void TUI::DrawChar(int a_x, int a_y, char a_c)
{
    int w, h;
    GetSize(w, h);
    if (a_x > 0 && a_x < w && a_y > 0 && a_y < h)
    {
        const TUI_Shared::Cell c(s_color, ' ');
        TUI_Shared::g_consoleData(a_x, a_y) = c;
    }
}

void TUI::DrawTextV(int a_x, int a_y, const char* a_format, va_list args)
{
    int w, h;
    GetSize(w, h);
    if (a_y < 0 || a_y >= h || a_x >= w)
        return;

    char buffer[1024];
    vsnprintf(buffer, 1024, a_format, args);
    size_t n = strlen(buffer);

    for (int i = 0; i < n; ++i)
    {
        if (a_x + i >= 0 && a_x + i < w)
        {
            const TUI_Shared::Cell c(s_color, buffer[i]);
            TUI_Shared::g_consoleData(a_x + i, a_y) = c;
        }
    }
}

#if defined(_WIN32)
// Windows-Header-Diet:
#define WIN32_LEAN_AND_MEAN
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
//#define NOUSER <-- needed for virtual keys
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#include <windows.h>
#include <tchar.h>
namespace TUI_Platform
{
static void Assert(bool a_condition, TCHAR* a_format, ...)
{
    if (!a_condition)
    {
        TCHAR buffer[1024];
        va_list args;

        va_start(args, a_format);
        _vstprintf_s(buffer, _countof(buffer), a_format, args);
        va_end(args);

        system("cls");
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), buffer, (DWORD)(sizeof(*buffer) * (_tcslen(buffer) + 1)), 0, 0);
        exit(EXIT_FAILURE);
    }
}

static struct ConsoleState
{
    HANDLE oldHandle;
    bool isInitialized = false;
} g_consoleState;
static struct ConsoleBuffer
{
    HANDLE handle;
    WORD attributes = 0;
    int width       = -1;
    int height      = -1;
    int oldWidth    = -1;
    int oldHeight   = -1;
    int sizeX       = -1;
    int sizeY       = -1;
    TUI_Shared::Buffer data;
} g_consoleBuffer;
void PresentBuffer()
{
    const bool isActiveScreenBuffer = SetConsoleActiveScreenBuffer(TUI_Platform::g_consoleBuffer.handle);
    TUI_Platform::Assert(isActiveScreenBuffer, _T("Could not set active console screen buffer: %ld\n"), GetLastError());
}

static BOOL WINAPI CtrlHandler(DWORD signal)
{
    UNREFERENCED_PARAMETER(signal);
    exit(EXIT_SUCCESS);
}

static void RestoreConsole(void)
{
    if (g_consoleState.isInitialized)
    {
        SetConsoleActiveScreenBuffer(g_consoleState.oldHandle);
        g_consoleState.isInitialized = false;
    }
}

static void DisableCursor(ConsoleBuffer& buffer)
{
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(buffer.handle, &ci);
    ci.bVisible = FALSE; /* Note: this has to be set every time console buffer resizes! */
    SetConsoleCursorInfo(buffer.handle, &ci);
}

static TUI::EKeys MapKey(KEY_EVENT_RECORD& a_keyEvent)
{
    switch (a_keyEvent.wVirtualKeyCode)
    {
        case VK_BACK: return TUI::EKeys::Key_Backspace;
        case VK_TAB: return TUI::EKeys::Key_Tab;
        case VK_RETURN: return TUI::EKeys::Key_Enter;
        case VK_SHIFT: return TUI::EKeys::Key_LeftShift;
        case VK_CONTROL: return TUI::EKeys::Key_LeftControl;
        case VK_MENU: return TUI::EKeys::Key_Menu;
        case VK_PAUSE: return TUI::EKeys::Key_Pause;
        case VK_CAPITAL: return TUI::EKeys::Key_CapsLock;
        case VK_ESCAPE: return TUI::EKeys::Key_Escape;
        case VK_SPACE: return TUI::EKeys::Key_Space;
        case VK_END: return TUI::EKeys::Key_End;
        case VK_HOME: return TUI::EKeys::Key_Home;
        case VK_LEFT: return TUI::EKeys::Key_Left;
        case VK_UP: return TUI::EKeys::Key_Up;
        case VK_RIGHT: return TUI::EKeys::Key_Right;
        case VK_DOWN: return TUI::EKeys::Key_Down;
        case VK_PRINT: return TUI::EKeys::Key_PrintScreen;
        case VK_INSERT: return TUI::EKeys::Key_Insert;
        case VK_DELETE: return TUI::EKeys::Key_Delete;
        case VK_NUMPAD0: return TUI::EKeys::Key_Kp0;
        case VK_NUMPAD1: return TUI::EKeys::Key_Kp1;
        case VK_NUMPAD2: return TUI::EKeys::Key_Kp2;
        case VK_NUMPAD3: return TUI::EKeys::Key_Kp3;
        case VK_NUMPAD4: return TUI::EKeys::Key_Kp4;
        case VK_NUMPAD5: return TUI::EKeys::Key_Kp5;
        case VK_NUMPAD6: return TUI::EKeys::Key_Kp6;
        case VK_NUMPAD7: return TUI::EKeys::Key_Kp7;
        case VK_NUMPAD8: return TUI::EKeys::Key_Kp8;
        case VK_NUMPAD9: return TUI::EKeys::Key_Kp9;
        case VK_MULTIPLY: return TUI::EKeys::Key_KpMultiply;
        case VK_ADD: return TUI::EKeys::Key_KpAdd;
        case VK_SUBTRACT: return TUI::EKeys::Key_KpSubtract;
        case VK_DECIMAL: return TUI::EKeys::Key_KpDecimal;
        case VK_DIVIDE: return TUI::EKeys::Key_KpDivide;
        case VK_F1: return TUI::EKeys::Key_F1;
        case VK_F2: return TUI::EKeys::Key_F2;
        case VK_F3: return TUI::EKeys::Key_F3;
        case VK_F4: return TUI::EKeys::Key_F4;
        case VK_F5: return TUI::EKeys::Key_F5;
        case VK_F6: return TUI::EKeys::Key_F6;
        case VK_F7: return TUI::EKeys::Key_F7;
        case VK_F8: return TUI::EKeys::Key_F8;
        case VK_F9: return TUI::EKeys::Key_F9;
        case VK_F10: return TUI::EKeys::Key_F10;
        case VK_F11: return TUI::EKeys::Key_F11;
        case VK_F12: return TUI::EKeys::Key_F12;
        case VK_F13: return TUI::EKeys::Key_F13;
        case VK_F14: return TUI::EKeys::Key_F14;
        case VK_F15: return TUI::EKeys::Key_F15;
        case VK_F16: return TUI::EKeys::Key_F16;
        case VK_F17: return TUI::EKeys::Key_F17;
        case VK_F18: return TUI::EKeys::Key_F18;
        case VK_F19: return TUI::EKeys::Key_F19;
        case VK_F20: return TUI::EKeys::Key_F20;
        case VK_F21: return TUI::EKeys::Key_F21;
        case VK_F22: return TUI::EKeys::Key_F22;
        case VK_F23: return TUI::EKeys::Key_F23;
        case VK_F24: return TUI::EKeys::Key_F24;
    }
    switch (a_keyEvent.uChar.AsciiChar)
    {
        case '0': return TUI::EKeys::Key_0;
        case '1': return TUI::EKeys::Key_1;
        case '2': return TUI::EKeys::Key_2;
        case '3': return TUI::EKeys::Key_3;
        case '4': return TUI::EKeys::Key_4;
        case '5': return TUI::EKeys::Key_5;
        case '6': return TUI::EKeys::Key_6;
        case '7': return TUI::EKeys::Key_7;
        case '8': return TUI::EKeys::Key_8;
        case '9': return TUI::EKeys::Key_9;
        case 'a':
        case 'A': return TUI::EKeys::Key_A;
        case 'b':
        case 'B': return TUI::EKeys::Key_B;
        case 'c':
        case 'C': return TUI::EKeys::Key_C;
        case 'd':
        case 'D': return TUI::EKeys::Key_D;
        case 'e':
        case 'E': return TUI::EKeys::Key_E;
        case 'f':
        case 'F': return TUI::EKeys::Key_F;
        case 'g':
        case 'G': return TUI::EKeys::Key_G;
        case 'h':
        case 'H': return TUI::EKeys::Key_H;
        case 'i':
        case 'I': return TUI::EKeys::Key_I;
        case 'j':
        case 'J': return TUI::EKeys::Key_J;
        case 'k':
        case 'K': return TUI::EKeys::Key_K;
        case 'l':
        case 'L': return TUI::EKeys::Key_L;
        case 'm':
        case 'M': return TUI::EKeys::Key_M;
        case 'n':
        case 'N': return TUI::EKeys::Key_N;
        case 'o':
        case 'O': return TUI::EKeys::Key_O;
        case 'p':
        case 'P': return TUI::EKeys::Key_P;
        case 'q':
        case 'Q': return TUI::EKeys::Key_Q;
        case 'r':
        case 'R': return TUI::EKeys::Key_R;
        case 's':
        case 'S': return TUI::EKeys::Key_S;
        case 't':
        case 'T': return TUI::EKeys::Key_T;
        case 'u':
        case 'U': return TUI::EKeys::Key_U;
        case 'v':
        case 'V': return TUI::EKeys::Key_V;
        case 'w':
        case 'W': return TUI::EKeys::Key_W;
        case 'x':
        case 'X': return TUI::EKeys::Key_X;
        case 'y':
        case 'Y': return TUI::EKeys::Key_Y;
        case 'z':
        case 'Z': return TUI::EKeys::Key_Z;
    }
    return TUI::EKeys::Key_None;
}

static void PollInput()
{
    TUI_Shared::g_input.keys.Reset();

    static INPUT_RECORD inputs[128];
    DWORD num;
    GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &num);
    if (num == 0)
        return;

    ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), inputs, 128, &num);
    for (DWORD i = 0; i < num; ++i)
    {
        INPUT_RECORD& input = inputs[i];
        if (input.EventType == KEY_EVENT)
        {
            TUI::EKeys key = MapKey(input.Event.KeyEvent);
            if (key != TUI::EKeys::Key_None &&
                input.Event.KeyEvent.bKeyDown)
            {
                int keyId                       = (int)key;
                TUI_Shared::g_input.keys[keyId] = true;
            }
        }
    }
}

} // namespace TUI_Platform

void TUI::Init(bool a_doubleBuffered)
{
    TUI_Platform::Assert(!TUI_Platform::g_consoleState.isInitialized, _T("TUI::Init called twice! Terminating!"));

    SetConsoleCtrlHandler(TUI_Platform::CtrlHandler, TRUE);
    TUI_Platform::g_consoleState.oldHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    TUI_Platform::g_consoleBuffer.handle = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        CONSOLE_TEXTMODE_BUFFER,
        0);
    TUI_Platform::Assert(TUI_Platform::g_consoleBuffer.handle != INVALID_HANDLE_VALUE, _T("Could not create console screen buffer: %ld\n"), GetLastError());
    SetConsoleMode(TUI_Platform::g_consoleBuffer.handle, ENABLE_PROCESSED_INPUT | DISABLE_NEWLINE_AUTO_RETURN);
    TUI_Platform::DisableCursor(TUI_Platform::g_consoleBuffer);

    TUI_Platform::PresentBuffer();

    TUI_Platform::g_consoleState.isInitialized = true;

    atexit(TUI_Platform::RestoreConsole);
}

void TUI::Shutdown()
{
    TUI_Platform::RestoreConsole();
}

void TUI::BeginFrame(bool& out_sizeChanged)
{
    TUI_Shared::g_frameTimer.BeginFrame();

    TUI_Platform::PollInput();

    out_sizeChanged = false;

    TUI_Platform::ConsoleBuffer& buffer = TUI_Platform::g_consoleBuffer;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(buffer.handle, &csbi);
    buffer.width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    buffer.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    if (buffer.width != buffer.oldWidth ||
        buffer.height != buffer.oldHeight)
    {
        TUI_Platform::DisableCursor(buffer);
        COORD size;
        size.X = (USHORT)buffer.width;
        size.Y = (USHORT)buffer.height;
        SetConsoleScreenBufferSize(buffer.handle, size);

        COORD coord = { 0, 0 };
        DWORD written;
        FillConsoleOutputAttribute(buffer.handle, 0, size.X * size.Y, coord, &written);
        FillConsoleOutputCharacter(buffer.handle, ' ', size.X * size.Y, coord, &written);

        buffer.oldWidth  = buffer.width;
        buffer.oldHeight = buffer.height;

        out_sizeChanged = true;
    }

    buffer.sizeX = (int)csbi.dwSize.X;
    buffer.sizeY = (int)csbi.dwSize.Y;

    if (buffer.attributes == 0)
        buffer.attributes = csbi.wAttributes;

    TUI_Shared::Buffer& data = TUI_Shared::g_consoleData;
    if (buffer.width != data.width ||
        buffer.height != data.height)
    {
        data.Resize(buffer.width, buffer.height);
        out_sizeChanged = true;
    }
}

void TUI::EndFrame(int a_targetFps)
{
    HANDLE console            = TUI_Platform::g_consoleBuffer.handle;
    TUI_Shared::Buffer& cache = TUI_Platform::g_consoleBuffer.data;
    TUI_Shared::Buffer& data  = TUI_Shared::g_consoleData;
    if (cache.width != data.width || cache.height != data.height)
        cache.Resize(data.width, data.height);

    DWORD written;
    for (int y = 0; y < data.height; ++y)
    {
        for (int x = 0; x < data.width; ++x)
        {
            const TUI_Shared::Cell& dataCell = data(x, y);
            TUI_Shared::Cell& cacheCell      = cache(x, y);
            if (cacheCell.raw != dataCell.raw)
            {
                const COORD coord = { (SHORT)x, (SHORT)y };
                const WORD color  = dataCell.color;
                const char value  = dataCell.value;
                WriteConsoleOutputAttribute(console, &color, 1, coord, &written);
                WriteConsoleOutputCharacter(console, &value, 1, coord, &written);
                cacheCell = dataCell;
            }
        }
    }

    TUI_Shared::g_frameTimer.EndFrame(a_targetFps);
}

#elif defined(HAS_NCURSES)
#include <ncurses.h>
#include <array>

namespace TUI_Platform
{

static struct ConsoleState
{
    bool isInitialized = false;
} g_consoleState;
static struct ConsoleBuffer
{
    int width     = -1;
    int height    = -1;
    int oldWidth  = -1;
    int oldHeight = -1;
    TUI_Shared::Buffer data;
} g_consoleBuffer;

void RestoreConsole()
{
    if (g_consoleState.isInitialized)
    {
        endwin();
        g_consoleState.isInitialized = false;
    }
}

static TUI::EKeys MapKey(int a_c)
{
    switch (a_c)
    {
        case KEY_DOWN: return TUI::EKeys::Key_Down;
        case KEY_UP: return TUI::EKeys::Key_Up;
        case KEY_LEFT: return TUI::EKeys::Key_Left;
        case KEY_RIGHT: return TUI::EKeys::Key_Right;
        case KEY_HOME: return TUI::EKeys::Key_Home;
        case KEY_BACKSPACE: return TUI::EKeys::Key_Backspace;
        case KEY_F(1): return TUI::EKeys::Key_F1;
        case KEY_F(2): return TUI::EKeys::Key_F2;
        case KEY_F(3): return TUI::EKeys::Key_F3;
        case KEY_F(4): return TUI::EKeys::Key_F4;
        case KEY_F(5): return TUI::EKeys::Key_F5;
        case KEY_F(6): return TUI::EKeys::Key_F6;
        case KEY_F(7): return TUI::EKeys::Key_F7;
        case KEY_F(8): return TUI::EKeys::Key_F8;
        case KEY_F(9): return TUI::EKeys::Key_F9;
        case KEY_F(10): return TUI::EKeys::Key_F10;
        case KEY_F(11): return TUI::EKeys::Key_F11;
        case KEY_F(12): return TUI::EKeys::Key_F12;
        case KEY_F(13): return TUI::EKeys::Key_F13;
        case KEY_F(14): return TUI::EKeys::Key_F14;
        case KEY_F(15): return TUI::EKeys::Key_F15;
        case KEY_F(16): return TUI::EKeys::Key_F16;
        case KEY_F(17): return TUI::EKeys::Key_F17;
        case KEY_F(18): return TUI::EKeys::Key_F18;
        case KEY_F(19): return TUI::EKeys::Key_F19;
        case KEY_F(20): return TUI::EKeys::Key_F20;
        case KEY_F(21): return TUI::EKeys::Key_F21;
        case KEY_F(22): return TUI::EKeys::Key_F22;
        case KEY_F(23): return TUI::EKeys::Key_F23;
        case KEY_F(24): return TUI::EKeys::Key_F24;
        case KEY_F(25): return TUI::EKeys::Key_F25;
        case 9: return TUI::EKeys::Key_Tab;
        case 338: return TUI::EKeys::Key_PageDown;
        case 339: return TUI::EKeys::Key_PageUp;
        case 360: return TUI::EKeys::Key_End;
        case 331: return TUI::EKeys::Key_Insert;
        case 330: return TUI::EKeys::Key_Delete;
        case 32: return TUI::EKeys::Key_Space;
        case 343: return TUI::EKeys::Key_KpEnter;
        case 27: return TUI::EKeys::Key_Escape;
        case '0': return TUI::EKeys::Key_0;
        case '1': return TUI::EKeys::Key_1;
        case '2': return TUI::EKeys::Key_2;
        case '3': return TUI::EKeys::Key_3;
        case '4': return TUI::EKeys::Key_4;
        case '5': return TUI::EKeys::Key_5;
        case '6': return TUI::EKeys::Key_6;
        case '7': return TUI::EKeys::Key_7;
        case '8': return TUI::EKeys::Key_8;
        case '9': return TUI::EKeys::Key_9;
        case 'a':
        case 'A': return TUI::EKeys::Key_A;
        case 'b':
        case 'B': return TUI::EKeys::Key_B;
        case 'c':
        case 'C': return TUI::EKeys::Key_C;
        case 'd':
        case 'D': return TUI::EKeys::Key_D;
        case 'e':
        case 'E': return TUI::EKeys::Key_E;
        case 'f':
        case 'F': return TUI::EKeys::Key_F;
        case 'g':
        case 'G': return TUI::EKeys::Key_G;
        case 'h':
        case 'H': return TUI::EKeys::Key_H;
        case 'i':
        case 'I': return TUI::EKeys::Key_I;
        case 'j':
        case 'J': return TUI::EKeys::Key_J;
        case 'k':
        case 'K': return TUI::EKeys::Key_K;
        case 'l':
        case 'L': return TUI::EKeys::Key_L;
        case 'm':
        case 'M': return TUI::EKeys::Key_M;
        case 'n':
        case 'N': return TUI::EKeys::Key_N;
        case 'o':
        case 'O': return TUI::EKeys::Key_O;
        case 'p':
        case 'P': return TUI::EKeys::Key_P;
        case 'q':
        case 'Q': return TUI::EKeys::Key_Q;
        case 'r':
        case 'R': return TUI::EKeys::Key_R;
        case 's':
        case 'S': return TUI::EKeys::Key_S;
        case 't':
        case 'T': return TUI::EKeys::Key_T;
        case 'u':
        case 'U': return TUI::EKeys::Key_U;
        case 'v':
        case 'V': return TUI::EKeys::Key_V;
        case 'w':
        case 'W': return TUI::EKeys::Key_W;
        case 'x':
        case 'X': return TUI::EKeys::Key_X;
        case 'y':
        case 'Y': return TUI::EKeys::Key_Y;
        case 'z':
        case 'Z': return TUI::EKeys::Key_Z;
    }
    return TUI::EKeys::Key_None;
}

static void PollInput()
{
    TUI_Shared::g_input.keys.Reset();

    while (true)
    {
        int c = wgetch(stdscr);
        if (c == ERR)
            break;

        TUI::EKeys key = MapKey(c);
        if (key != TUI::EKeys::Key_None)
        {
            int keyId                       = (int)key;
            TUI_Shared::g_input.keys[keyId] = true;
        }
    }
}

struct ColorPairs
{
    int operator()(const TUI::Color& a_color)
    {
        if (m_cache[a_color.raw] < 1)
        {
            init_pair(m_nRgistered, (uint16_t)a_color.foreground, (uint16_t)a_color.background);
            m_cache[a_color.raw] = m_nRgistered;
            ++m_nRgistered;
        }
        return m_cache[a_color.raw];
    }

private:
    int m_nRgistered = 1;
    std::array<int, 256> m_cache;
} g_colorPairs;

} // namespace TUI_Platform

void TUI::Init(bool a_doubleBuffered)
{
    initscr();
    use_default_colors();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    wtimeout(stdscr, 1);
    set_escdelay(25);
    keypad(stdscr, true);

    TUI_Platform::g_consoleState.isInitialized = true;

    atexit(TUI_Platform::RestoreConsole);
}

void TUI::Shutdown()
{
    TUI_Platform::RestoreConsole();
}

void TUI::BeginFrame(bool& out_sizeChanged)
{
    TUI_Shared::g_frameTimer.BeginFrame();

    TUI_Platform::PollInput();

    TUI_Platform::ConsoleBuffer& buffer = TUI_Platform::g_consoleBuffer;
    getmaxyx(stdscr, buffer.height, buffer.width);
    if (buffer.width != buffer.oldWidth ||
        buffer.height != buffer.oldHeight)
    {
        buffer.oldWidth  = buffer.width;
        buffer.oldHeight = buffer.height;

        out_sizeChanged = true;
    }

    TUI_Shared::Buffer& data = TUI_Shared::g_consoleData;
    if (buffer.width != data.width ||
        buffer.height != data.height)
    {
        data.Resize(buffer.width, buffer.height);
        out_sizeChanged = true;
    }
}

void TUI::EndFrame(int a_targetFps)
{
    wrefresh(stdscr);

    TUI_Shared::Buffer& cache = TUI_Platform::g_consoleBuffer.data;
    TUI_Shared::Buffer& data  = TUI_Shared::g_consoleData;
    if (cache.width != data.width || cache.height != data.height)
        cache.Resize(data.width, data.height);

    int activeColor = 0xFFFFFFFF;
    for (int y = 0; y < data.height; ++y)
    {
        for (int x = 0; x < data.width; ++x)
        {
            const TUI_Shared::Cell& dataCell = data(x, y);
            TUI_Shared::Cell& cacheCell      = cache(x, y);
            if (cacheCell.raw != dataCell.raw)
            {
                if (dataCell.color != activeColor)
                {
                    if (dataCell.color == 0)
                    {
                        attrset(A_NORMAL);
                        attron(COLOR_PAIR(0));
                    }
                    else
                    {
                        int pair = TUI_Platform::g_colorPairs(dataCell.color);
                        attron(COLOR_PAIR(pair));
                    }
                    activeColor = dataCell.color;
                }
                mvaddch(y, x, dataCell.value);
                cacheCell = dataCell;
            }
        }
    }

    TUI_Shared::g_frameTimer.EndFrame(a_targetFps);
}

#else
#error UNSUPPORTED PLATFORM!
#endif

#endif // TUI_IMPLEMENTATION

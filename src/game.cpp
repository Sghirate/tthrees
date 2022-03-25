#include "game.h"
#define TUI_IMPLEMENTATION
#include "tui.hpp"

namespace
{
const char* g_bindings = "Restart (F5) | Quit (q)";
// we have limited space in our tiles - therefore we have a limited number of possible tile values.
// btw, formula for the tile values: f(i) = floor(2^i - 2^(i-2))
//  => let's bake em in!
const char g_texts[][9] = {
    "        ",
    "    1   ",
    "    2   ",
    "    3   ",
    "    6   ",
    "   12   ",
    "   24   ",
    "   48   ",
    "   96   ",
    "   192  ",
    "   384  ",
    "   768  ",
    "  1536  ",
    "  3072  ",
    "  6144  ",
    "  12288 ",
    "  24576 ",
    "  49152 ",
    "  98304 ",
    " 196608 ",
    " 393216 ",
    " 786432 ",
    " 1572864",
    " 3145728",
    " 6291456",
    " 1582912",
    "25165824",
    "50331648",
};

static struct Random // PCG based
{
    Random(uint32_t a_seed)
    {
        uint64_t value = (((uint64_t)a_seed) << 1ULL) | 1ULL;
        value          = Murmur3Avalanche64(value);
        m_state[0]     = 0U;
        m_state[1]     = (value << 1ULL) | 1ULL;
        Next();
        m_state[0] += Murmur3Avalanche64(value);
        Next();
    }
    uint32_t Next()
    {
        uint64_t oldstate   = m_state[0];
        m_state[0]          = oldstate * 0x5851f42d4c957f2dULL + m_state[1];
        uint32_t xorshifted = (uint32_t)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
        uint32_t rot        = (uint32_t)(oldstate >> 59ULL);
        return (xorshifted >> rot) | (xorshifted << ((-(int)rot) & 31));
    }
    template <typename T>
    void Shuffle(T* a_buffer, size_t a_size)
    {
        for (size_t i = 0; i < a_size; ++i)
        {
            const size_t j = Next() % a_size;
            const T temp   = a_buffer[i];
            a_buffer[i]    = a_buffer[j];
            a_buffer[j]    = temp;
        }
    }
    template <typename T, size_t SIZE>
    inline void Shuffle(T (&a_buffer)[SIZE])
    {
        Shuffle(a_buffer, SIZE);
    }

private:
    static uint64_t Murmur3Avalanche64(uint64_t a_value)
    {
        a_value ^= a_value >> 33;
        a_value *= 0xff51afd7ed558ccd;
        a_value ^= a_value >> 33;
        a_value *= 0xc4ceb9fe1a85ec53;
        a_value ^= a_value >> 33;
        return a_value;
    }
    uint64_t m_state[2];
} g_random((uint32_t)time(NULL));

struct
{
    TUI::EKeys key;
    Game::EInputs input;
} g_keyMap[] = {
    // note: order determines priority (descending)!
    { TUI::EKeys::Key_Q, Game::EInputs::Quit },
    { TUI::EKeys::Key_F5, Game::EInputs::Restart },
    { TUI::EKeys::Key_Left, Game::EInputs::Left },
    { TUI::EKeys::Key_Up, Game::EInputs::Up },
    { TUI::EKeys::Key_Right, Game::EInputs::Right },
    { TUI::EKeys::Key_Down, Game::EInputs::Down },
    { TUI::EKeys::Key_Space, Game::EInputs::Space },
};
template <uint8_t SIZE>
struct Deck
{
    Deck()
    {
        Reset();
    }

    void Reset()
    {
        for (int i = 0; i < SIZE; ++i)
        {
            m_buffer[i] = (i / (SIZE / 3)) + 1;
        }
        m_n = 12;
        g_random.Shuffle(m_buffer);
    }
    bool IsEmpty() const
    {
        return m_n <= 0;
    }
    uint8_t Pop()
    {
        return m_buffer[--m_n];
    }

private:
    uint8_t m_buffer[SIZE];
    uint8_t m_n;
};
Deck<12> g_deck;
template <typename T, uint8_t SIZE>
struct RandomPool
{
    RandomPool()
        : m_n(0)
    {
    }

    void Push(const T& option)
    {
        m_options[m_n++] = option;
    }
    void PushUnique(const T& option)
    {
        for (int i = 0; i < m_n; ++i)
        {
            if (m_options[i] == option)
            {
                return;
            }
        }
        Push(option);
    }
    T Pick()
    {
        return m_options[g_random.Next() % m_n];
    }
    void Clear()
    {
        m_n = 0;
    }

private:
    T m_options[SIZE];
    uint8_t m_n;
};
RandomPool<uint8_t, 32> g_bonus_deck;

struct BoardRenderer
{
    typedef Pos2D<int> rpos;

    static rpos CalculateRenderPosition(const Game::Config& a_cfg, const Game::pos& a_pos)
    {
        return rpos(
            a_cfg.posX + a_pos.x * (a_cfg.tileWidth + a_cfg.tileSpacing),
            a_cfg.posY + a_pos.y * (a_cfg.tileHeight + a_cfg.tileSpacing));
    }

    template <typename t>
    static t Clamp(t a_x, t a_min, t a_max)
    {
        if (a_x < a_min)
            a_x = a_min;
        if (a_x > a_max)
            a_x = a_max;
        return a_x;
    }

    static float Smoothstep(float a_x)
    {
        return a_x * a_x * (3.0f - 2.0f * a_x);
    }

    static float Smootherstep(float a_x)
    {
        return a_x * a_x * a_x * (a_x * (a_x * 6.0f - 15.0f) + 10.0f);
    }

    static float Interpolate(float a_from, float a_to, float a_alpha)
    {
        return a_from + (a_to - a_from) * a_alpha;
    }

    static rpos Interpolate(const rpos& a_from, const rpos& a_to, float a_alpha)
    {
        return rpos(
            (int)Interpolate((float)a_from.x, (float)a_to.x, a_alpha),
            (int)Interpolate((float)a_from.y, (float)a_to.y, a_alpha));
    }

    static void RenderTile(const Game::Config& a_cfg, uint8_t a_value, const rpos& r, bool a_drawValue = true)
    {
        TUI::Color c(TUI::EColors::Black, TUI::EColors::LightGray);
        switch (a_value)
        {
            case 1: c = TUI::Color(TUI::EColors::Black, TUI::EColors::LightBlue); break;
            case 2: c = TUI::Color(TUI::EColors::Black, TUI::EColors::LightRed); break;
        }
        TUI::ColorScope applyColor(c);
        TUI::DrawRect(r.x, r.y, a_cfg.tileWidth, a_cfg.tileHeight);
        if (a_drawValue)
        {
	    TUI::DrawText(r.x, r.y + a_cfg.tileHeight / 2, g_texts[a_value]);
        }
    }

    static void Render(const Game::Config& a_cfg, const Game::Board& a_state, const Game::BoardAnimation& a_anim, uint8_t a_next)
    {
        int canvasWidth, canvasHeight;
        TUI::GetSize(canvasWidth, canvasHeight);

        // draw background
        {
            TUI::ColorScope boardLineColor(TUI::EColors::White, TUI::EColors::DarkGray);
            const int boardWidth  = Game::BOARD_EXTENT * (a_cfg.tileWidth + 1);
            const int boardHeight = Game::BOARD_EXTENT * (a_cfg.tileHeight + 1);
            TUI::DrawRect(a_cfg.posX, a_cfg.posY, boardWidth, boardHeight);
            TUI::ColorScope boardBackgroundColor(TUI::EColors::White, TUI::EColors::Black);
            for (int i = 0; i <= Game::BOARD_EXTENT; ++i)
            {
                TUI::DrawLine(
                    a_cfg.posX - a_cfg.tileSpacing,
                    a_cfg.posY - a_cfg.tileSpacing + (i * (a_cfg.tileHeight + a_cfg.tileSpacing)),
                    a_cfg.posX + Game::BOARD_EXTENT * (a_cfg.tileWidth + a_cfg.tileSpacing) - a_cfg.tileSpacing,
                    a_cfg.posY - a_cfg.tileSpacing + (i * (a_cfg.tileHeight + a_cfg.tileSpacing)));
                TUI::DrawLine(
                    a_cfg.posX - a_cfg.tileSpacing + (i * (a_cfg.tileWidth + a_cfg.tileSpacing)),
                    a_cfg.posY - a_cfg.tileSpacing,
                    a_cfg.posX - a_cfg.tileSpacing + (i * (a_cfg.tileWidth + a_cfg.tileSpacing)),
                    a_cfg.posY - a_cfg.tileSpacing + Game::BOARD_EXTENT * (a_cfg.tileHeight + a_cfg.tileSpacing));
            }
        }
        // draw fixed tiles
        for (uint8_t y = 0; y < Game::BOARD_EXTENT; ++y)
        {
            for (uint8_t x = 0; x < Game::BOARD_EXTENT; ++x)
            {
                Game::pos p(x, y);
                if (a_state.tiles[p.ToIndex()] == 0)
                {
                    continue;
                }
                RenderTile(a_cfg, a_state.tiles[p.ToIndex()], CalculateRenderPosition(a_cfg, p));
            }
        }
        // // draw moving tiles
        for (int i = 0; i < a_anim.nMoving; ++i)
        {
            const Game::TileAnimation& anim = a_anim.moving[i];
            RenderTile(
                a_cfg,
                anim.value,
                Interpolate(
                    CalculateRenderPosition(a_cfg, anim.from),
                    CalculateRenderPosition(a_cfg, anim.to),
                    Smootherstep(Clamp(a_anim.alpha, 0.0f, 0.9f))));
            // note: 0.9f is used to prevent an occasional overshoot issue
            //  might be worth trying to figure out why it happens . . .
        }
        // draw next tile
        static Game::pos next_tile_pos(5, 0);
        rpos r = CalculateRenderPosition(a_cfg, next_tile_pos);
        TUI::ColorScope headerColor(TUI::EColors::White, TUI::EColors::Black);
        TUI::DrawText(r.x, r.y - 1, "Next:");
        RenderTile(a_cfg, a_next, r, false);
    }
};

} // namespace

Game::Board::Board()
{
    Reset();
}

void Game::Board::Reset()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        tiles[i] = 0;
    }
    tiles[2] = 1;
    tiles[3] = 2;
    tiles[8] = 3;
}

Game::BoardAnimation::BoardAnimation()
{
    Reset();
}

void Game::BoardAnimation::Push(TileAnimation anim)
{
    moving[nMoving++] = anim;
}

void Game::BoardAnimation::Reset()
{
    alpha   = 0.0f;
    nMoving = 0;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        result[i] = 0;
    }
}

Game::Game()
    : quit(false)
{
    TUI::Init();
    Reset();
}

Game::~Game()
{
    TUI::Shutdown();
}

int Game::Run()
{
    while (!quit)
    {
        int res = Tick();
        if (res != 0)
        {
            return res;
        }
    }
    return 0;
}

int Game::Tick()
{
    bool sizeChanged;
    TUI::BeginFrame(sizeChanged);

    EInputs input = ReadInput();
    bool active   = Update(input);

    if (sizeChanged || active)
        Draw();

    TUI::EndFrame();

    return 0;
}

void Game::Reset()
{
    g_deck.Reset();
    state.Reset();
    anim.Reset();
    int n = 0;
    while (n < 9)
    {
        pos p(
            g_random.Next() % BOARD_EXTENT,
            g_random.Next() % BOARD_EXTENT);
        if (state.tiles[p.ToIndex()] == 0)
        {
            state.tiles[p.ToIndex()] = PickRandomValue();
            ++n;
        }
    }
    next  = PickRandomValue();
    phase = EPhases::Active;
}

uint8_t Game::CalculateTileMoveResult(pos a_from, pos a_diff)
{
    pos to = a_from + a_diff;
    if ((state.tiles[a_from.ToIndex()] == 1 && state.tiles[to.ToIndex()] == 2) ||
        (state.tiles[a_from.ToIndex()] == 2 && state.tiles[to.ToIndex()] == 1)) // combine small numbers
    {
        return 3;
    }
    else if (state.tiles[a_from.ToIndex()] != 0 && state.tiles[to.ToIndex()] == 0) // move to empty field
    {
        return state.tiles[a_from.ToIndex()];
    }
    else if (state.tiles[a_from.ToIndex()] >= 3 && state.tiles[a_from.ToIndex()] == state.tiles[to.ToIndex()]) // combine non-small equal numbers
    {
        return state.tiles[a_from.ToIndex()] + 1;
    }
    return 0;
}

bool Game::IsBoardMovePossible(EInputs a_dir)
{
    pos diff = CalculateMoveDiff(a_dir);
    switch (a_dir)
    {
        case EInputs::Left:
            for (int x = 1; x < BOARD_EXTENT; ++x)
                for (int y = 0; y < BOARD_EXTENT; ++y)
                    if (CalculateTileMoveResult(pos(x, y), diff) > 0)
                        return true;
            break;
        case EInputs::Right:
            for (int x = BOARD_EXTENT - 2; x >= 0; --x)
                for (int y = 0; y < BOARD_EXTENT; ++y)
                    if (CalculateTileMoveResult(pos(x, y), diff) > 0)
                        return true;
            break;
        case EInputs::Up:
            for (int x = 0; x < BOARD_EXTENT; ++x)
                for (int y = 1; y < BOARD_EXTENT; ++y)
                    if (CalculateTileMoveResult(pos(x, y), diff) > 0)
                        return true;
            break;
        case EInputs::Down:
            for (int x = 0; x < BOARD_EXTENT; ++x)
                for (int y = BOARD_EXTENT - 2; y >= 0; --y)
                    if (CalculateTileMoveResult(pos(x, y), diff) > 0)
                        return true;
            break;
        default: break;
    }
    return false;
}

bool Game::IsGameOver()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if (state.tiles[i] == 0)
        {
            return false;
        }
    }

    for (uint8_t dir = static_cast<uint8_t>(EInputs::FirstDir); dir <= static_cast<uint8_t>(EInputs::LastDir); ++dir)
    {
        if (IsBoardMovePossible(static_cast<EInputs>(dir)))
        {
            return false;
        }
    }

    return true;
}

bool Game::IsGameWon()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        constexpr int max = sizeof(g_texts) / sizeof(g_texts[0]) - 1;
        if (state.tiles[i] == max)
        {
            return true;
        }
    }
    return false;
}

Game::pos Game::CalculateMoveDiff(EInputs a_dir)
{
    switch (a_dir)
    {
        case EInputs::Left: return pos(-1, 0);
        case EInputs::Right: return pos(+1, 0);
        case EInputs::Up: return pos(0, -1);
        case EInputs::Down: return pos(0, +1);
        default: break;
    }
    return pos(0, 0);
}

bool Game::TryMoveTile(pos a_from, pos a_diff)
{
    uint8_t result = CalculateTileMoveResult(a_from, a_diff);
    if (result > 0)
    {
        pos to = a_from + a_diff;
        anim.Push(TileAnimation(a_from, to, state.tiles[a_from.ToIndex()]));
        anim.result[to.ToIndex()]     = result;
        state.tiles[a_from.ToIndex()] = 0;
        return true;
    }
    return false;
}

bool Game::TryMoveBoard(EInputs dir)
{
    pos diff = CalculateMoveDiff(dir);
    bool any = false;
    switch (dir)
    {
        case EInputs::Left:
            for (int x = 1; x < BOARD_EXTENT; ++x)
                for (int y = 0; y < BOARD_EXTENT; ++y)
                    any |= TryMoveTile(pos(x, y), diff);
            break;
        case EInputs::Right:
            for (int x = BOARD_EXTENT - 2; x >= 0; --x)
                for (int y = 0; y < BOARD_EXTENT; ++y)
                    any |= TryMoveTile(pos(x, y), diff);
            break;
        case EInputs::Up:
            for (int x = 0; x < BOARD_EXTENT; ++x)
                for (int y = 1; y < BOARD_EXTENT; ++y)
                    any |= TryMoveTile(pos(x, y), diff);
            break;
        case EInputs::Down:
            for (int x = 0; x < BOARD_EXTENT; ++x)
                for (int y = BOARD_EXTENT - 2; y >= 0; --y)
                    any |= TryMoveTile(pos(x, y), diff);
            break;
        default: break;
    }

    if (any)
    {
        pos new_pos                    = PickRandomTarget(dir);
        anim.result[new_pos.ToIndex()] = next;
        anim.Push(TileAnimation(new_pos - diff, new_pos, next));
        next = PickRandomValue();
    }

    return any;
}

uint8_t Game::PickRandomValue()
{
    // pretty much exactly taken from threesjs, with the math modified to match the value representation used here.
    bool bonus      = false;
    uint8_t highest = 0;
    for (uint8_t value : state.tiles)
    {
        if (value >= 7)
            bonus = true;
        if (value > highest)
            highest = value;
    }
    if (bonus && (rand() % 100) < 5)
    {
        g_bonus_deck.Clear();
        uint8_t size = highest - 2 * 3;
        for (int i = 0; i < size; ++i)
        {
            g_bonus_deck.Push(4 + i);
        }
        return g_bonus_deck.Pick();
    }

    if (g_deck.IsEmpty())
    {
        g_deck.Reset();
    }
    return g_deck.Pop();
}

Game::pos Game::PickRandomTarget(EInputs dir)
{
    pos p;
    switch (dir)
    {
        case EInputs::Left:
        case EInputs::Right:
        {
            p.x = dir == EInputs::Left ? (BOARD_EXTENT - 1) : 0;
            RandomPool<uint8_t, BOARD_EXTENT> y_pool;
            for (int i = 0; i < anim.nMoving; ++i)
            {
                y_pool.PushUnique(anim.moving[i].from.y);
            }
            p.y = y_pool.Pick();
        }
        break;

        case EInputs::Up:
        case EInputs::Down:
        {
            p.y = dir == EInputs::Up ? (BOARD_EXTENT - 1) : 0;
            RandomPool<uint8_t, BOARD_EXTENT> x_pool;
            for (int i = 0; i < anim.nMoving; ++i)
            {
                x_pool.PushUnique(anim.moving[i].from.x);
            }
            p.x = x_pool.Pick();
        }
        break;
        default: break;
    }
    return p;
}

Game::EInputs Game::ReadInput() const
{
    for (int i = 0; i < sizeof(g_keyMap) / sizeof(g_keyMap[0]); ++i)
    {
        if (TUI::IsKeyPressed(g_keyMap[i].key))
        {
            return g_keyMap[i].input;
        }
    }
    return EInputs::None;
}

bool Game::Update(EInputs input)
{
    bool stateChanged = false;
    if (input == EInputs::Quit)
    {
        quit = true;
    }
    else if (input == EInputs::Restart)
    {
        Reset();
        stateChanged = true;
    }

    switch (phase)
    {
        case EPhases::Active:
            if (TryMoveBoard(input))
            {
                anim.alpha   = 0.0f;
                phase        = EPhases::Animating;
                stateChanged = true;
            }
            break;
        case EPhases::Animating:
            anim.alpha += TUI::GetDeltaSeconds() * (1.0f / cfg.animSeconds);
            if (anim.alpha > 1.0f)
            {
                for (int i = 0; i < BOARD_SIZE; ++i)
                {
                    state.tiles[i] = anim.result[i] != 0 ? anim.result[i] : state.tiles[i];
                }
                anim.Reset();

                phase = IsGameOver() ? EPhases::GameOver : (IsGameWon() ? EPhases::GameWon : EPhases::Active);
            }
            stateChanged = true;
            break;
        case EPhases::GameOver:
        case EPhases::GameWon:
            if (input == EInputs::Space)
            {
                Reset();
                stateChanged = true;
            }
            break;
        default: break;
    }
    return stateChanged;
}

void Game::Draw() const
{
    int w, h;
    TUI::GetSize(w, h);
    TUI::ClearScreen();

    BoardRenderer::Render(cfg, state, anim, next);

    TUI::ColorScope nextTileTextColor(TUI::EColors::Black, TUI::EColors::LightGray);
    TUI::DrawLine(0, 0, w, 0);
    TUI::DrawText(1, 0, "Terminal Threes");
    TUI::DrawText(w - 23, 0, "Restart (F5) | Quit (q)");

    if (phase == EPhases::GameOver ||
        phase == EPhases::GameWon)
    {
        const int panelX = cfg.posX - cfg.tileSpacing;
        const int panelY = cfg.posY + (BOARD_EXTENT / 2) * (cfg.tileHeight + cfg.tileSpacing) - 2;
        const int panelW = BOARD_EXTENT * (cfg.tileWidth + cfg.tileSpacing) + cfg.tileSpacing;
        const int panelH = 4;
        TUI::ColorScope panelColor(TUI::EColors::Black, TUI::EColors::DarkGray);
        TUI::DrawRect(panelX, panelY, panelW, panelH);
        {
            TUI::ColorScope panelHeaderColor(TUI::EColors::Black, TUI::EColors::LightGray);
            TUI::DrawLine(panelX, panelY, panelX + panelW - cfg.tileSpacing, panelY);
            TUI::DrawText(panelX + 2, panelY, phase == EPhases::GameOver ? "Game Over!" : "GAME WON!");
        }
        TUI::DrawText(panelX + 2, panelY + 2, "Press space to start again");
    }
}

#include <game.h>
#include <imtui/imtui.h>
#include <imtui/imtui-impl-ncurses.h>
#include <util/path.h>
#include <util/file_watcher.h>
#include <util/imgui_style.h>
#include <util/imgui_keys.h>
#include <curses.h>
#include <limits.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

namespace
{
path_instance<PATH_MAX> g_style_path(path::get_home(), "/.tthrees/style");
file_watcher g_style_watcher(g_style_path.c_str());
ImTui::TScreen* g_screen = nullptr;
const char* g_bindings = "Restart (F5) | Quit (q)";
// we have limited space in our tiles - therefore we have a limited number of possible tile values.
// btw, formula for the tile values: f(i) = floor(2^i - 2^(i-2))
//  => let's bake em in!
const char g_texts[][9] = {
    "",
    "1",
    "2",
    "3",
    "6",
    "12",
    "24",
    "48",
    "96",
    "192",
    "384",
    "768",
    "1536",
    "3072",
    "6144",
    "12288",
    "24576",
    "49152",
    "98304",
    "196608",
    "393216",
    "786432",
    "1572864",
    "3145728",
    "6291456",
    "1582912",
    "25165824",
    "50331648",
};

struct
{
    uint16_t key;
    game::inputs input;
} g_keyMap[] = { // note: order determines priority (descending)!
    { 'q',       game::inputs::quit },
    { KEY_F(5),  game::inputs::restart },
    { KEY_LEFT,  game::inputs::left },
    { KEY_UP,    game::inputs::up },
    { KEY_RIGHT, game::inputs::right },
    { KEY_DOWN,  game::inputs::down },
    { ' ',       game::inputs::space },
};
struct deck
{
    deck()
    {
        reset();
    }

    void reset()
    {
        for (int i = 0; i < 12; ++i)
        {
            buffer[i] = (i / 4) + 1;
        }
        n = 12;
        std::random_shuffle(buffer, buffer + 12);
    }
    bool empty() const
    {
        return n <= 0;
    }
    uint8_t pop()
    {
        return buffer[--n];
    }

private:
    uint8_t buffer[12];
    uint8_t n;
} g_deck;
template<typename T, uint8_t SIZE>
struct random_pool
{
    random_pool() : n(0) {}

    void push(const T& option)
    {
        options[n++] = option;
    }
    void push_unique(const T& option)
    {
        for (int i = 0; i < n; ++i)
        {
            if (options[i] == option)
            {
                return;
            }
        }
        push(option);
    }
    T pick()
    {
        return options[rand() % n];
    }
    void clear()
    {
        n = 0;
    }

private:
    T options[SIZE];
    uint8_t n;
};
random_pool<uint8_t, 32> g_bonus_deck;

struct
{
    const ImU32 yellow = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f,1.0f,0.0f,1.0f));
    const ImU32 blue = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 1.0f, 1.0f));
    const ImU32 red = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    const ImU32 gray = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    const ImU32 black = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    const ImU32 white = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
} colU32;	
struct board_renderer
{
    typedef pos2d<float> rpos;

    static rpos calc_render_pos(const game::config& cfg, const game::pos& p)
    {
        return rpos(
            cfg.pos_x + p.x * (cfg.tile_width + cfg.tile_spacing),
            cfg.pos_y + p.y * (cfg.tile_height + cfg.tile_spacing));
    }

    template <typename t>
    static t clamp(t x, t min, t max)
    {
        if (x < min) x = min;
        if (x > max) x = max;
        return x;
    }

    static float smoothstep(float x)
    {
        return x * x * (3.0f - 2.0f * x);
    }

    static float smootherstep(float x)
    {
        return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
    }

    static float interpolate(float from, float to, float alpha)
    {
        return from + (to - from) * alpha;
    }

    static rpos interpolate(const rpos& from, const rpos& to, float alpha)
    {
        return rpos(
            interpolate(from.x, to.x, alpha),
            interpolate(from.y, to.y, alpha));
    }

    static void render_tile(const game::config& cfg, uint8_t value, const rpos& r, bool draw_value = true)
    {
        ImU32 color = colU32.white;
        if (value == 1)
        {
            color = colU32.blue;
        }
        else if(value == 2)
        {
            color = colU32.red;
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(ImVec2(r.x, r.y), ImVec2(r.x + cfg.tile_width - 0.5f, r.y + cfg.tile_height - 0.5f), color, 0.0f, ImDrawCornerFlags_None);
        if (draw_value)
        {
            ImVec2 text_size = ImGui::CalcTextSize(g_texts[value]);
            draw_list->AddText(
                ImVec2(
                    r.x + (cfg.tile_width - text_size.x) / 2.0f - 0.5f,
                    r.y + cfg.tile_height / 2 - 0.5f), 
                colU32.black,
                g_texts[value]);
        }
    }

    static void render(const game::config& cfg, const game::board& state, const game::board_animation& anim, uint8_t next)
    {
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiStyle& style = ImGui::GetStyle();

        // draw lines
        for (int i = 0; i <= game::BOARD_EXTENT; ++i)
        {
            draw_list->AddLine(
                ImVec2(cfg.pos_x - cfg.tile_spacing, cfg.pos_y - cfg.tile_spacing + (i * (cfg.tile_height + cfg.tile_spacing))),
                ImVec2(cfg.pos_x + game::BOARD_EXTENT * (cfg.tile_width + cfg.tile_spacing) - 0.5f, cfg.pos_y - cfg.tile_spacing + (i * (cfg.tile_height + cfg.tile_spacing))),
                colU32.gray, 
                0.0f);

            draw_list->AddLine(
                ImVec2(cfg.pos_x - cfg.tile_spacing + (i * (cfg.tile_width + cfg.tile_spacing)), cfg.pos_y - cfg.tile_spacing),
                ImVec2(cfg.pos_x - cfg.tile_spacing + (i * (cfg.tile_width + cfg.tile_spacing)) , cfg.pos_y - cfg.tile_spacing + game::BOARD_EXTENT * (cfg.tile_height + cfg.tile_spacing) - 0.5f),
                colU32.gray, 
                0.0f);
        }

        // draw fixed tiles
        for (uint8_t y = 0; y < game::BOARD_EXTENT; ++y)
        {
            for (uint8_t x = 0; x < game::BOARD_EXTENT; ++x)
            {
                game::pos p(x, y);
                if (state.tiles[p] == 0)
                {
                    continue;
                }

                render_tile(cfg, state.tiles[p], calc_render_pos(cfg, p));
            }
        }

        // draw moving tiles
        std::for_each(anim.moving, anim.moving + anim.moving_count, [&](const game::tile_animation& a)
        {
            render_tile(
                cfg,
                a.value,
                interpolate(
                    calc_render_pos(cfg, a.from),
                    calc_render_pos(cfg, a.to),
                    smootherstep(clamp(anim.alpha, 0.0f, 0.9f))));
            // note: 0.9f is used to prevent an occasional overshoot issue
            //  might be worth trying to figure out why it happens . . .
        });

        // draw next tile:
        static game::pos next_tile_pos(5, 0);
        rpos r = calc_render_pos(cfg, next_tile_pos);
        draw_list->AddText(ImVec2(r.x, r.y - 1.0f), ImGui::GetColorU32(style.Colors[ImGuiCol_Text]), "Next:");
        render_tile(cfg, next, r, false);
    }
};

} // namespace

game::board::board()
{
    reset();
}

void game::board::reset()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        tiles[i] = 0;
    }
    tiles[2] = 1;
    tiles[3] = 2;
    tiles[8] = 3;
}

game::board_animation::board_animation()
{
    reset();
}

void game::board_animation::push(tile_animation anim)
{
    moving[moving_count++] = anim;
}

void game::board_animation::reset()
{
    alpha = 0.0f;
    moving_count = 0;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        result[i] = 0;
    }
}

game::game()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 0;
    style.FrameRounding = 0;
    style.ScrollbarRounding = 0;

    imgui_style::load(g_style_path.c_str());

    g_screen = ImTui_ImplNcurses_Init(true, 60.0f, 5.0f);
    ImTui_ImplText_Init();

    ImGuiIO &io = ImGui::GetIO();
    io.NavActive = false;
    io.NavVisible = false;

    reset();
}

game::~game()
{
    ImTui_ImplText_Shutdown();
    ImTui_ImplNcurses_Shutdown();
}

int game::run()
{
    while (!quit)
    {
        int res = tick();
        if (res != 0)
        {
            return res;
        }
    }
    return 0;
}

int game::tick()
{
    if (g_style_watcher.check_modified())
    {
        imgui_style::load(g_style_path.c_str());
    }

    ImTui_ImplNcurses_NewFrame();
    ImTui_ImplText_NewFrame();

    ImGui::NewFrame();

    inputs input = read_input();
    bool active = update(input);
    draw();

    ImGui::Render();

    ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), g_screen);
    ImTui_ImplNcurses_DrawScreen(active);

    return 0;
}

void game::reset()
{
    g_deck.reset();
    state.reset();
    anim.reset();
    int n = 0;
    while(n < 9)
    {
        pos p(
            rand() % BOARD_EXTENT,
            rand() % BOARD_EXTENT);
        if (state.tiles[p] == 0)
        {
            state.tiles[p] = pick_random_value();
            ++n;
        }
    }
    next = pick_random_value();
    phase = phases::active;
}

uint8_t game::tile_move_result(pos from, pos diff)
{
    pos to = from + diff;
    if ((state.tiles[from] == 1 && state.tiles[to] == 2) ||
        (state.tiles[from] == 2 && state.tiles[to] == 1)) // combine small numbers
    {
        return 3;
    }
    else if (state.tiles[from] != 0 && state.tiles[to] == 0) // move to empty field
    {
        return state.tiles[from];
    }
    else if (state.tiles[from] >= 3 &&
        state.tiles[from] == state.tiles[to]) // combine non-small equal numbers
    {
        return state.tiles[from] + 1;
    }
    return 0;
}

bool game::board_move_possible(inputs dir)
{
    pos diff = move_diff(dir);
    switch (dir)
    {
    case inputs::left:
        for (int x = 1; x < BOARD_EXTENT;++x)
            for (int y = 0; y < BOARD_EXTENT;++y)
                if (tile_move_result(pos(x, y), diff) > 0)
                    return true;
        break;
    case inputs::right:
        for (int x = BOARD_EXTENT - 2; x >= 0;--x)
            for (int y = 0; y < BOARD_EXTENT;++y)
                if (tile_move_result(pos(x, y), diff) > 0)
                    return true;
        break;
    case inputs::up:
        for (int x = 0; x < BOARD_EXTENT;++x)
            for (int y = 1; y < BOARD_EXTENT;++y)
                if (tile_move_result(pos(x, y), diff) > 0)
                    return true;
        break;
    case inputs::down:
        for (int x = 0; x < BOARD_EXTENT;++x)
            for (int y = BOARD_EXTENT-2; y >= 0;--y)
                if (tile_move_result(pos(x, y), diff) > 0)
                    return true;
        break;
    }
    return false;
}

bool game::game_over()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if (state.tiles[i] == 0)
        {
            return false;
        }
    }

    for (uint8_t dir = static_cast<uint8_t>(inputs::first_dir); dir <= static_cast<uint8_t>(inputs::last_dir); ++dir)
    {
        if (board_move_possible(static_cast<inputs>(dir)))
        {
            return false;
        }
    }

    return true;
}

bool game::game_won()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        constexpr int max = sizeof(g_texts)/sizeof(g_texts[0]) - 1;
        if (state.tiles[i] == max)
        {
            return true;
        }
    }
    return false;
}

game::pos game::move_diff(inputs dir)
{
    switch (dir)
    {
    case inputs::left:  return pos(-1,  0);
    case inputs::right: return pos(+1,  0);
    case inputs::up:    return pos( 0, -1);
    case inputs::down:  return pos( 0, +1);
    }
    return pos(0, 0);
}

bool game::try_tile_move(pos from, pos diff)
{
    uint8_t result = tile_move_result(from, diff);
    if (result > 0)
    {
        pos to = from + diff;
        anim.push(tile_animation(from, to, state.tiles[from]));
        anim.result[to] = result;
        state.tiles[from] = 0;
        return true;
    }
    return false;
}

bool game::try_board_move(inputs dir)
{
    pos diff = move_diff(dir);
    bool any = false;
    switch (dir)
    {
    case inputs::left:
        for (int x = 1; x < BOARD_EXTENT;++x)
            for (int y = 0; y < BOARD_EXTENT;++y)
                any |= try_tile_move(pos(x, y), diff);
        break;
    case inputs::right:
        for (int x = BOARD_EXTENT - 2; x >= 0;--x)
            for (int y = 0; y < BOARD_EXTENT;++y)
                any |= try_tile_move(pos(x, y), diff);
        break;
    case inputs::up:
        for (int x = 0; x < BOARD_EXTENT;++x)
            for (int y = 1; y < BOARD_EXTENT;++y)
                any |= try_tile_move(pos(x, y), diff);
        break;
    case inputs::down:
        for (int x = 0; x < BOARD_EXTENT;++x)
            for (int y = BOARD_EXTENT-2; y >= 0;--y)
                any |= try_tile_move(pos(x, y), diff);
        break;
    }

    if (any)
    {
        pos new_pos = pick_random_target(dir);
        anim.result[new_pos] = next;
        anim.push(tile_animation(new_pos - diff, new_pos, next));
        next = pick_random_value();
    }

    return any;
}

uint8_t game::pick_random_value()
{
    // pretty much exactly taken from threesjs, with the math modified to match the value representation used here.
    bool bonus = std::any_of(state.tiles, state.tiles + BOARD_EXTENT, [](uint8_t v) { return v >= 7; });
    if (bonus && (rand() % 100) < 5)
    {
        g_bonus_deck.clear();
        uint8_t highest = *std::max_element(state.tiles, state.tiles + BOARD_EXTENT);
        uint8_t size = highest - 2 * 3;
        for (int i = 0; i < size; ++i)
        {
            g_bonus_deck.push(4 + i);
        }
        return g_bonus_deck.pick();
    }

    if (g_deck.empty())
    {
        g_deck.reset();
    }
    return g_deck.pop();
}

game::pos game::pick_random_target(inputs dir)
{
    pos p;
    switch (dir)
    {
    case inputs::left:
    case inputs::right:
        {
            p.x = dir == inputs::left ? (BOARD_EXTENT - 1) : 0;
            random_pool<uint8_t, BOARD_EXTENT> y_pool;
            for (int i = 0; i < anim.moving_count; ++i)
            {
                y_pool.push_unique(anim.moving[i].from.y);
            }
            p.y = y_pool.pick();
        }
        break;

    case inputs::up:
    case inputs::down:
        {
            p.y = dir == inputs::up ? (BOARD_EXTENT - 1) : 0;
            random_pool<uint8_t, BOARD_EXTENT> x_pool;
            for (int i = 0; i < anim.moving_count; ++i)
            {
                x_pool.push_unique(anim.moving[i].from.x);
            }
            p.x = x_pool.pick();
        }
        break;
    }
    return p;
}

game::inputs game::read_input() const
{
    ImGuiIO &io = ImGui::GetIO();
    if (imgui_keys::no_mod(io))
    {
        for(int i = 0; i < sizeof(g_keyMap)/sizeof(g_keyMap[0]); ++i)
        {
            if (io.KeysDown[g_keyMap[i].key])
            {
                return g_keyMap[i].input;
            }
        }
    }
    return inputs::none;
}

bool game::update(inputs input)
{
    bool stateChanged = false;
    if (input == inputs::quit)
    {
        quit = true;
    }
    else if (input == inputs::restart)
    {
        reset();
        stateChanged = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    switch (phase)
    {
    case phases::active:
        if (try_board_move(input))
        {
            anim.alpha = 0.0f;
            phase = phases::animating;
            stateChanged = true;
        }
        break;
    case phases::animating:
        anim.alpha += io.DeltaTime * (1.0f / cfg.anim_duration);
        if (anim.alpha > 1.0f)
        {
            for (int i = 0; i < BOARD_SIZE; ++i)
            {
                state.tiles[i] = anim.result[i] != 0 ? anim.result[i] : state.tiles[i];
            }
            anim.reset();

            phase = game_over() ? phases::game_over : (game_won() ? phases::game_won : phases::active);
        }
        stateChanged = true;
        break;
    case phases::game_over:
    case phases::game_won:
        if (input == inputs::space)
        {
            reset();
            stateChanged = true;
        }
        break;
    }
    return stateChanged;
}

void game::draw() const
{
    ImGuiIO &io = ImGui::GetIO();

    if (ImGui::BeginMainMenuBar())
    {
        ImGui::Text("Terminal Threes");
        ImVec2 text_size = ImGui::CalcTextSize(g_bindings);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - text_size.x - 5.0f);
        ImGui::Text("%s", g_bindings);
        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowPos(ImVec2(0, 1));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - 1));
    if (ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        board_renderer::render(cfg, state, anim, next);

        if (phase == phases::game_over ||
            phase == phases::game_won)
        {
            ImGui::SetNextWindowPos(ImVec2(cfg.pos_x, cfg.pos_y + (BOARD_EXTENT / 2) * (cfg.tile_height + cfg.tile_spacing) - 2.5f));
            ImGui::SetNextWindowSize(ImVec2(BOARD_EXTENT * (cfg.tile_width + cfg.tile_spacing) - 1.0f, 5.0f));
            if (ImGui::Begin(
                phase == phases::game_over ? "Game Over!" : "GAME WON!", 
                nullptr, 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::Dummy(ImVec2(0.0f, 1.0f));
                ImGui::Text("Press space to start again");
                ImGui::End();
            }
        }

        ImGui::End();
    }
}

#pragma once

#include <stdint.h>
#include <util/pos2d.h>

struct game
{
    enum { BOARD_EXTENT = 4 };
    enum { BOARD_SIZE = BOARD_EXTENT * BOARD_EXTENT };
    
    typedef pos2d<int8_t, BOARD_EXTENT> pos;

    enum class phases : uint8_t
    {
        active = 0,
        animating,
        game_over,

        count,
    };
    enum class inputs : uint8_t
    {
        none = 0,
        left,
        right,
        up,
        down,
        space,
        restart,
        quit,

        count,
        first_dir = left,
        last_dir = down,
    };
    struct config
    {
        float pos_x = 3.0f;
        float pos_y = 3.0f;
        float tile_width = 8.0f;
        float tile_height = 5.0f;
        float tile_spacing = 1.0f;
        float anim_duration = 0.25f;
    };
    struct board
    {
        board();

        void reset();

        uint8_t tiles[BOARD_SIZE];
    };
    struct tile_animation
    {
        pos from;
        pos to;
        uint8_t value;

        tile_animation() : value(0) {}
        tile_animation(pos from, pos to, uint8_t value) : from(from), to(to), value(value) {}
    };
    struct board_animation
    {
        float alpha = 0.0f;
        uint8_t result[BOARD_SIZE];
        tile_animation moving[BOARD_SIZE+BOARD_EXTENT]; // impossible worst case: the entire board moves and a full row/column spawns
        uint8_t moving_count;

        board_animation();

        void push(tile_animation anim);
        void reset();
    };

    game();
    ~game();

    // static int8_t tile_id(int8_t x, int8_t y);
    // static void tile_coordinates(int8_t id, int8_t& x, int8_t& y);

    int run();
    int tick();

private:
    void reset();
    uint8_t tile_move_result(pos from, pos to);
    bool board_move_possible(inputs dir);
    bool game_over();
    pos move_diff(inputs dir);
    bool try_tile_move(pos from, pos to);
    bool try_board_move(inputs dir);
    uint8_t pick_random_value();
    pos pick_random_target(inputs dir);
    inputs read_input() const;
    void update(inputs input);
    void draw() const;

    config cfg;
    board state;
    board_animation anim;
    phases phase = phases::active;
    uint8_t next;
    bool quit;
};

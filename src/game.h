#pragma once

#include <stdint.h>
#include <util/pos2d.h>

struct Game
{
    enum { BOARD_EXTENT = 4 };
    enum { BOARD_SIZE = BOARD_EXTENT * BOARD_EXTENT };
    
    typedef Pos2D<int8_t, BOARD_EXTENT> pos;

    enum class EPhases : uint8_t
    {
        Active = 0,
        Animating,
        GameOver,
        GameWon,

        COUNT,
    };
    enum class EInputs : uint8_t
    {
        None = 0,
        Left,
        Right,
        Up,
        Down,
        Space,
        Restart,
        Quit,

        COUNT,
        FirstDir = Left,
        LastDir = Down,
    };
    struct Config
    {
        int posX = 3;
        int posY = 3;
        int tileWidth = 8;
        int tileHeight = 5;
        int tileSpacing = 1;
        float animSeconds = 0.25f;
    };
    struct Board
    {
        Board();

        void Reset();

        uint8_t tiles[BOARD_SIZE];
    };
    struct TileAnimation
    {
        pos from;
        pos to;
        uint8_t value;

        TileAnimation() : value(0) {}
        TileAnimation(pos from, pos to, uint8_t value) : from(from), to(to), value(value) {}
    };
    struct BoardAnimation
    {
        float alpha = 0.0f;
        uint8_t result[BOARD_SIZE];
        TileAnimation moving[BOARD_SIZE+BOARD_EXTENT]; // impossible worst case: the entire board moves and a full row/column spawns
        uint8_t nMoving;

        BoardAnimation();

        void Push(TileAnimation anim);
        void Reset();
    };

    Game();
    ~Game();

    int Run();
    int Tick();

private:
    void Reset();
    uint8_t CalculateTileMoveResult(pos a_from, pos a_to);
    bool IsBoardMovePossible(EInputs dir);
    bool IsGameOver();
    bool IsGameWon();
    pos CalculateMoveDiff(EInputs dir);
    bool TryMoveTile(pos from, pos to);
    bool TryMoveBoard(EInputs dir);
    uint8_t PickRandomValue();
    pos PickRandomTarget(EInputs dir);
    EInputs ReadInput() const;
    bool Update(EInputs input);
    void Draw() const;

    Config cfg;
    Board state;
    BoardAnimation anim;
    EPhases phase = EPhases::Active;
    uint8_t next;
    bool quit;
};

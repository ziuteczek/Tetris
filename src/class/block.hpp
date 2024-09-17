#include <SDL2/SDL.h>

#include <array>
#include <cmath>

#ifndef ROWS_QUANTITY || COLUMNS_QUANTITY
#define ROWS_QUANTITY 20
#define COLUMNS_QUANTITY 10
#endif

enum blockTypesNames
{
    BLOCK_TYPE_T,
    BLOCK_TYPE_SQUARE,
    BLOCK_TYPE_STICK,
    BLOCK_TYPE_L,
    BLOCK_TYPE_L_REVERSED,
    BLOCK_TYPE_DOG,
    BLOCK_TYPE_DOG_REVERSED,
    BLOCK_TYPES_TOTAL
};

typedef struct cell
{
    SDL_Color color;
    SDL_Point pos;
} cell;

class Block
{
private:
    blockTypesNames blockType;

    std::array<cell, 4> cells;
    SDL_Color color;

    int blockWidth;
    int blockLength;

    void setBlockType(blockTypesNames blockType);
    void setBlockTypeRandom();

    void calcBlockWidth();
    void calcBlockLength();

    void resetPos();

public:
    Block();

    SDL_Point pos;

    void rotate();
    void reset();
};
Block::Block()
{
    setBlockTypeRandom();
}
void Block::calcBlockLength()
{
    int maxTilt = 0;
    for (auto cell : cells)
    {
        maxTilt = std::max(maxTilt, cell.pos.x);
    }
    blockWidth = maxTilt + 1;
}
void Block::calcBlockWidth()
{
    int maxTilt = 0;
    for (auto cell : cells)
    {
        maxTilt = std::max(maxTilt, cell.pos.x);
    }
    blockWidth = maxTilt + 1;
}
void Block::reset()
{
    resetPos();

    calcBlockWidth();
    calcBlockLength();
}
void Block::resetPos()
{
    pos.y = -blockLength + 1;
    pos.x = rand() % (COLUMNS_QUANTITY - blockWidth);
}
void Block::setBlockTypeRandom()
{
    blockTypesNames blockTypeDrawn = static_cast<blockTypesNames>(rand() % BLOCK_TYPES_TOTAL);
    setBlockType(blockTypeDrawn);
}
void Block::setBlockType(blockTypesNames blockType)
{
    this->blockType = blockType;

    switch (this->blockType)
    {
    case BLOCK_TYPE_T:
        cells = {0, 1, 1, 1, 2, 1, 1, 0};
        color = {0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPE_SQUARE:
        cells = {0, 0, 1, 0, 0, 1, 1, 1};
        color = {0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPE_STICK:
        cells = {0, 0, 0, 1, 0, 2, 0, 3};
        color = {0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPE_L_REVERSED:
        cells = {0, 0, 0, 1, 0, 2, 1, 2};
        color = {0x00, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPE_DOG:
        cells = {1, 0, 1, 1, 1, 2, 0, 2};
        color = {0xFF, 0xFF, 0x00, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPE_DOG_REVERSED:
        cells = {1, 0, 2, 0, 0, 1, 1, 1};
        color = {0xFF, 0xFF, 0x00, SDL_ALPHA_OPAQUE};
        break;
    case BLOCK_TYPES_TOTAL:
        cells = {0, 0, 1, 0, 1, 1, 2, 1};
        color = {0xFF, 0x00, 0xFF, SDL_ALPHA_OPAQUE};
        break;
    }
}
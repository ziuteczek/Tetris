#include <SDL2/SDL.h>

#include <array>
#include <cmath>

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

class Block
{
private:
    blockTypesNames blockType;

    std::array<SDL_Point, 4> cells;
    SDL_Color color;

public:
    Block(blockTypesNames blockType);
    Block() = default;

    SDL_Point pos;

    void rotate();
    void setBlockType(blockTypesNames blockType);
    void setBlockTypeRandom();
    void reset();
};

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
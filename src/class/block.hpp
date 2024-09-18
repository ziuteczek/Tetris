#include <SDL2/SDL.h>

#include <array>
#include <cmath>

#ifndef ROWS_QUANTITY
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
    SDL_Point pos;
    SDL_Color color;
} cell;

class TBlock
{
private:
    blockTypesNames blockType;

    int width;
    int length;

    void calcBlockWidth();
    void calcBlockLength();

    void resetPos();

    std::vector<cell> &placedCells;

public:
    TBlock(std::vector<cell> placedCells) : placedCells(placedCells) {}

    SDL_Point pos;
    SDL_Color color;

    void rotate();
    void reset();

    void setBlockType(blockTypesNames blockType);

    int getWidth();
    int getLength();

    bool checkColisionRight(std::array<SDL_Point, 4> *block /*= nullptr*/);
    bool checkColisionLeft(std::array<SDL_Point, 4> *block /*= nullptr*/);

    std::array<SDL_Point, 4> cells;
};
void TBlock::rotate()
{
    std::swap(this->width, this->length);

    std::array<SDL_Point, 4> rotateCopy = cells;

    for (auto &blockCell : rotateCopy)
    {
        int blockCellYold = blockCell.x;
        blockCell.x = blockCell.y;
        blockCell.y = this->width - 1 - blockCellYold;
    }

    if (!checkColisionRight(nullptr) && !checkColisionLeft(nullptr))
    {
        cells = rotateCopy;
    }

    calcBlockLength();
    calcBlockWidth();
}
void TBlock::calcBlockLength()
{
    int maxTilt = 0;
    for (auto cell : cells)
    {
        maxTilt = std::max(maxTilt, cell.x);
    }
    length = maxTilt + 1;
}
void TBlock::calcBlockWidth()
{
    int maxTilt = 0;
    for (auto cell : cells)
    {
        maxTilt = std::max(maxTilt, cell.x);
    }
    width = maxTilt + 1;
}
void TBlock::reset()
{
    calcBlockWidth();
    calcBlockLength();

    resetPos();
}
void TBlock::resetPos()
{
    pos.y = -this->length + 1;
    pos.x = rand() % (COLUMNS_QUANTITY - this->width);
}
int TBlock::getWidth()
{
    return width;
}
int TBlock::getLength()
{
    return length;
}
void TBlock::setBlockType(blockTypesNames blockType)
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
bool TBlock::checkColisionLeft(std::array<SDL_Point, 4> *block = nullptr)
{
    std::array<SDL_Point, 4> cellsToCheck = (block == nullptr) ? cells : *block;

    if (pos.x == 0)
    {
        return true;
    }
    for (auto placedCell : placedCells)
    {
        for (auto blockCell : cells)
        {
            bool cellsSameLevel = blockCell.y + pos.y == placedCell.pos.y;
            bool cellsSticking = blockCell.x + pos.x - 1 == placedCell.pos.x;

            if (cellsSameLevel && cellsSticking)
            {
                return true;
            }
        }
    }
    return false;
}
bool TBlock::checkColisionRight(std::array<SDL_Point, 4> *block = nullptr)
{
    std::array<SDL_Point, 4> cellsToCheck = (block == nullptr) ? cells : *block;

    if (pos.x == COLUMNS_QUANTITY - width)
    {
        return true;
    }
    for (auto placedCell : placedCells)
    {
        for (auto blockCell : cells)
        {
            bool cellsSameLevel = blockCell.y + pos.y == placedCell.pos.y;
            bool cellsSticking = blockCell.x + pos.x + 1 == placedCell.pos.x;

            if (cellsSameLevel && cellsSticking)
            {
                return true;
            }
        }
    }
    return false;
}
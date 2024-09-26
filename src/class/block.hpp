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

#ifndef CELL_STRUCT
#define CELL_STRUCT
typedef struct cell
{
    SDL_Point pos;
    SDL_Color color;
} cell;
#endif

class TBlock
{
private:
    int width;
    int length;

    void calcBlockWidth();
    void calcBlockLength();
    void resetPos();

    std::vector<cell> &placedCells;  // Fixed error: Reference to the placedCells vector

public:
    blockTypesNames type;

    // Constructor: Initialize the reference to placedCells using an initializer list
    TBlock(std::vector<cell> &placedCells) : placedCells(placedCells) {}

    SDL_Point pos;
    SDL_Color color;

    std::array<SDL_Point, 4> cells;

    void rotate();
    void reset();

    int getWidth();
    int getLength();

    bool checkColisionRight(std::array<SDL_Point, 4> *block = nullptr);
    bool checkColisionLeft(std::array<SDL_Point, 4> *block = nullptr);
    bool isPlaced();
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
        maxTilt = std::max(maxTilt, cell.y);
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
    pos.y = -(this->length);
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

bool TBlock::checkColisionLeft(std::array<SDL_Point, 4> *block /*= nullptr*/)
{
    std::array<SDL_Point, 4> cellsToCheck = (block == nullptr) ? cells : *block;

    if (pos.x == 0)
    {
        return true;
    }

    for (auto placedCell : placedCells)
    {
        for (auto blockCell : cellsToCheck)
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

bool TBlock::isPlaced()
{
    for (auto currentBlockCell : cells)
    {
        int currentBlockCellY = pos.y + currentBlockCell.y + 1;

        if (currentBlockCellY == ROWS_QUANTITY)
        {
            return true;
        }
        for (auto placedCell : placedCells)
        {
            if (pos.x + currentBlockCell.x == placedCell.pos.x && currentBlockCellY == placedCell.pos.y)
            {
                return true;
            }
        }
    }
    return false;
}

bool TBlock::checkColisionRight(std::array<SDL_Point, 4> *block /*= nullptr*/)
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

#include <SDL2/SDL.h>

#include <iostream>
#include <vector>
#include <map>
#include <array>

#ifndef ROWS_QUANTITY
#define ROWS_QUANTITY 20
#define COLUMNS_QUANTITY 10
#endif

#ifndef CELL_STRUCT
#define CELL_STRUCT
typedef struct cell
{
    SDL_Point pos;
    SDL_Color color;
} cell;
#endif

class PlacedCells
{
public:
    PlacedCells(std::vector<cell> &loadPlacedCells);

    void placeBlock(SDL_Point blockPos, std::array<SDL_Point, 4> block, SDL_Color color);
    void clearRows(std::vector<int> rowsToClear);
    std::vector<int> getFilledRows();
    std::vector<cell> &cells;
    bool isLost();
};
PlacedCells::PlacedCells(std::vector<cell> &loadPlacedCells) : cells(loadPlacedCells)
{
}
std::vector<int> PlacedCells::getFilledRows()
{
    std::map<int, int> rowsLength;

    for (auto placedCell : cells)
    {
        rowsLength[placedCell.pos.y]++;
    }
    std::vector<int> filledRows;
    for (auto [rowIndex, rowLength] : rowsLength)
    {
        if (rowLength == COLUMNS_QUANTITY)
        {
            filledRows.push_back(rowIndex);
        }
    }
    return filledRows;
}
bool PlacedCells::isLost()
{
    bool isGameLost = false;
    for (auto placedCell : cells)
    {
        if (placedCell.pos.y <= 0)
        {
            isGameLost = true;
            break;
        }
    }
    return isGameLost;
}
void PlacedCells::placeBlock(SDL_Point blockPos, std::array<SDL_Point, 4> block, SDL_Color color)
{
    for (auto blockCell : block)
    {
        cell cellToPlace;

        cellToPlace.color = color;
        cellToPlace.pos.x = blockCell.x + blockPos.x;
        cellToPlace.pos.y = blockCell.y + blockPos.y;

        cells.push_back(cellToPlace);
    }
}
void PlacedCells::clearRows(std::vector<int> rowsToClear)
{
    for (auto rowIndex : rowsToClear)
    {
        for (int i = cells.size() - 1, columnsCleared = 0; columnsCleared < 10; i--)
        {
            if (cells[i].pos.y == rowIndex)
            {
                cells.erase(cells.begin() + i);
                columnsCleared++;
            }
        }
        for (auto &placedCell : cells)
        {
            if (placedCell.pos.y < rowIndex)
            {
                placedCell.pos.y++;
            }
        }
    }
}
#include <SDL2/SDL.h>

#include <array>
#include <vector>
#include <cmath>
#include <map>

#include "./texture.hpp"

#define CELLS_IN_BLOCK 4

#define ROWS_QUANTITY 20
#define COLUMNS_QUANTITY 10

typedef struct block
{
    SDL_Color color;
    std::array<SDL_Point, CELLS_IN_BLOCK> cells;
} block;

typedef struct cell
{
    SDL_Color color;
    SDL_Point pos;
} cell;
/*
enum checkColisionDirections
{
    COLISION_DIRECTION_RIGHT = 0b1,
    COLISION_DIRECTION_BOTTOM = 0b10,
    COLISION_DIRECTION_LEFT = 0b100,
};
enum checkColisionDirectionsENUM
{
    COLISION_DIRECTION_RIGHT,
    COLISION_DIRECTION_BOTTOM,
    COLISION_DIRECTION_LEFT,
    COLISION_DIRECTION_TOTAL
};
*/
enum arrows
{
    ARROW_NONE,
    ARROW_UP,
    ARROW_RIGHT,
    ARROW_DOWN,
    ARROW_LEFT
};

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

class Game
{
private:
    SDL_Window *gWindow = nullptr;
    SDL_Renderer *gRenderer = nullptr;

    TTF_Font *gFont = nullptr;

    SDL_Event e;

    int cellW, cellH;
    int currentBlockMaxRightTilt;
    int currentBlockLength;
    SDL_Point currentBlockPos;

    Uint64 startTime;
    Uint64 currentFrameTime;
    GTexture currentTimeTexture;

    std::vector<cell> placedCells;

    std::array<block, BLOCK_TYPES_TOTAL> blockTypes;

    block *currentBlock = nullptr;
    block *nextBlock = nullptr;

    SDL_Rect gameViewPort;
    SDL_Rect generalViewPort;

    arrows keyPressed = ARROW_NONE;

    std::string getCurrentTimeStr();

    void handleGameResize();

    void loadBlocks();

    void calcCurrentBlockMaxRightTilt();
    void calcCurrentBlockLength();
    void placeCurrentBlock();
    bool isBlockPlaced();

    bool checkBlockColisionLeft();
    bool checkBlockColisionRight();

    void drawCurrentBlock();
    void drawPlacedCells();
    void renderNextBlock();

    std::vector<int> getFilledRows();
    void clearRows(std::vector<int> rowsToClear);
    int calcPoints(int rowsCleared);

    void drawCell(SDL_Point cords, SDL_Color color);

public:
    Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont);

    bool exit = false;

    void handleEvents();
    void update();
    void render();
};
Game::Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont) : gWindow(loadWindow), gRenderer(loadRenderer), currentTimeTexture(gRenderer), gFont(loadFont)
{
    handleGameResize();
    loadBlocks();

    SDL_RenderGetViewport(gRenderer, &generalViewPort);

    currentBlock = &blockTypes[rand() % BLOCK_TYPES_TOTAL];
    nextBlock = &blockTypes[rand() % BLOCK_TYPES_TOTAL];

    calcCurrentBlockMaxRightTilt();
    calcCurrentBlockLength();

    currentBlockPos.x = rand() % (COLUMNS_QUANTITY - currentBlockMaxRightTilt);
    currentBlockPos.y = -currentBlockLength + 1;

    startTime = SDL_GetTicks64();
}
void Game::handleEvents()
{
    keyPressed = ARROW_NONE;

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            exit = true;
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                handleGameResize();
            }
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_UP:
                keyPressed = ARROW_UP;
                break;
            case SDLK_RIGHT:
                keyPressed = ARROW_RIGHT;
                break;
            case SDLK_DOWN:
                keyPressed = ARROW_DOWN;
                break;
            case SDLK_LEFT:
                keyPressed = ARROW_LEFT;
                break;
            }
            break;
        }
    }
}
void Game::update()
{
    bool blockMovedPlayer = false;

    switch (keyPressed)
    {
    case ARROW_NONE:
        break;
    case ARROW_LEFT:
        if (!checkBlockColisionLeft())
        {
            currentBlockPos.x--;
        }
        break;
    case ARROW_RIGHT:
        if (!checkBlockColisionRight())
        {
            currentBlockPos.x++;
        }
        break;
    }

    if (keyPressed != ARROW_NONE)
    {
        blockMovedPlayer = true;
    }

    // Time handle
    Uint64 lastFrameTime = currentFrameTime;
    currentFrameTime = SDL_GetTicks64() - startTime;
    currentTimeTexture.loadTextTexture(getCurrentTimeStr(), {0, 0, 0, SDL_ALPHA_OPAQUE}, gFont);

    // In milisecounds
    const int autoFallFrequency = 750;

    // When less than %autoFallFrequency% passed since last frame
    bool blockAutoMoved1 = currentFrameTime % autoFallFrequency < lastFrameTime % autoFallFrequency;
    // When more than %autoFallFrequency% passed since last frame (in case of a lag)
    bool blockAutoMoved2 = currentFrameTime - lastFrameTime > autoFallFrequency;

    bool blockAutoMoved = blockAutoMoved1 || blockAutoMoved2;

    if (blockAutoMoved || blockMovedPlayer)
    {
        if (isBlockPlaced())
        {
            placeCurrentBlock();

            currentBlock = nextBlock;
            nextBlock = &blockTypes[rand() % BLOCK_TYPES_TOTAL];
            calcCurrentBlockMaxRightTilt();
            calcCurrentBlockLength();

            currentBlockPos.x = rand() % (COLUMNS_QUANTITY - currentBlockMaxRightTilt);
            currentBlockPos.y = -currentBlockLength + 1;

            std::vector<int> filledRows = getFilledRows();

            if (!filledRows.empty())
            {
                clearRows(filledRows);
                // calcPoints(filledRows.size());
            }
        }
        else if (blockAutoMoved || keyPressed == ARROW_DOWN)
        {
            currentBlockPos.y++;
        }
    }
}
void Game::render()
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(gRenderer, &gameViewPort);

    currentTimeTexture.render(0, 0, nullptr);

    SDL_RenderSetViewport(gRenderer, &gameViewPort);

    drawCurrentBlock();
    drawPlacedCells();
    // renderNextBlock();

    SDL_RenderSetViewport(gRenderer, &generalViewPort);

    SDL_RenderPresent(gRenderer);
}
void Game::drawCell(SDL_Point coords, SDL_Color color)
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);

    SDL_Rect cellRect;

    cellRect.x = cellW * coords.x;
    cellRect.y = cellH * coords.y;

    cellRect.w = cellW;
    cellRect.h = cellH;

    SDL_RenderFillRect(gRenderer, &cellRect);
}
bool Game::isBlockPlaced()
{
    for (auto currentBlockCell : currentBlock->cells)
    {
        int currentBlockCellY = currentBlockPos.y + currentBlockCell.y + 1;

        if (currentBlockCellY == ROWS_QUANTITY)
        {
            return true;
        }
        for (auto placedCell : placedCells)
        {
            if (currentBlockPos.x + currentBlockCell.x == placedCell.pos.x && currentBlockCellY == placedCell.pos.y)
            {
                return true;
            }
        }
    }
    return false;
}
void Game::placeCurrentBlock()
{
    for (auto currentBlockCell : currentBlock->cells)
    {
        cell cellToPlace;

        cellToPlace.color = currentBlock->color;
        cellToPlace.pos.x = currentBlockCell.x + currentBlockPos.x;
        cellToPlace.pos.y = currentBlockCell.y + currentBlockPos.y;

        placedCells.push_back(cellToPlace);
    }
}
void Game::handleGameResize()
{
    int screenW, screenH;

    SDL_GetWindowSize(gWindow, &screenW, &screenH);

    gameViewPort = {25, 0, 350, 700};
    gameViewPort.y = (screenH - gameViewPort.h) / 2;

    cellW = gameViewPort.w / COLUMNS_QUANTITY;
    cellH = gameViewPort.h / ROWS_QUANTITY;
}
void Game::drawCurrentBlock()
{
    for (auto cell : currentBlock->cells)
    {
        SDL_Point cellDrawPoint = {cell.x + currentBlockPos.x, cell.y + currentBlockPos.y};

        drawCell(cellDrawPoint, currentBlock->color);
    }
}
void Game::drawPlacedCells()
{
    for (auto placedCell : placedCells)
    {
        SDL_Point cellCords = {placedCell.pos.x, placedCell.pos.y};

        drawCell(cellCords, placedCell.color);
    }
}
void Game::calcCurrentBlockMaxRightTilt()
{
    int maxTilt = 0;
    for (auto cell : currentBlock->cells)
    {
        maxTilt = std::max(maxTilt, cell.x);
    }
    currentBlockMaxRightTilt = maxTilt + 1;
}
void Game::calcCurrentBlockLength()
{
    int maxTilt = 0;
    for (auto cell : currentBlock->cells)
    {
        maxTilt = std::max(maxTilt, cell.y);
    }
    currentBlockLength = maxTilt + 1;
}
void Game::clearRows(std::vector<int> rowsToClear)
{
    for (auto rowIndex : rowsToClear)
    {
        for (int i = placedCells.size() - 1, columnsCleared = 0; columnsCleared < 10; i--)
        {
            if (placedCells[i].pos.y == rowIndex)
            {
                placedCells.erase(placedCells.begin() + i);
                columnsCleared++;
            }
        }
        for (auto &placedCell : placedCells)
        {
            if (placedCell.pos.y < rowIndex)
            {
                placedCell.pos.y++;
            }
        }
    }
}
std::vector<int> Game::getFilledRows()
{
    std::map<int, int> rowsLength;

    for (auto placedCell : placedCells)
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
/*
bool Game::checkBlockColisions(checkColisionDirections directions)
{
    std::vector<checkColisionDirectionsENUM> directionsChosen;
    for (int i = 0; i < COLISION_DIRECTION_TOTAL; i++)
    {
        if ((reinterpret_cast<bool *>(&directions)[i]))
        {
            directionsChosen.push_back(*reinterpret_cast<checkColisionDirectionsENUM *>(&i));
        }
    }
    for (auto direction : directionsChosen)
    {
        switch (direction)
        {
        case COLISION_DIRECTION_BOTTOM:

            break;
        case COLISION_DIRECTION_LEFT:
            break;
        }
    }
}
*/
bool Game::checkBlockColisionRight()
{
    if (currentBlockPos.x == COLUMNS_QUANTITY - currentBlockMaxRightTilt)
    {
        return true;
    }
    for (auto placedCell : placedCells)
    {
        for (auto blockCell : currentBlock->cells)
        {
            bool cellsSameLevel = blockCell.y + currentBlockPos.y == placedCell.pos.y;
            bool cellsSticking = blockCell.x + currentBlockPos.x + 1 == placedCell.pos.x;

            if (cellsSameLevel && cellsSticking)
            {
                return true;
            }
        }
    }
    return false;
}
bool Game::checkBlockColisionLeft()
{
    if (currentBlockPos.x == 0)
    {
        return true;
    }
    for (auto placedCell : placedCells)
    {
        for (auto blockCell : currentBlock->cells)
        {
            bool cellsSameLevel = blockCell.y + currentBlockPos.y == placedCell.pos.y;
            bool cellsSticking = blockCell.x + currentBlockPos.x - 1 == placedCell.pos.x;

            if (cellsSameLevel && cellsSticking)
            {
                return true;
            }
        }
    }
    return false;
}
void Game::loadBlocks()
{
    blockTypes[BLOCK_TYPE_T].cells = {0, 1, 1, 1, 2, 1, 1, 0};
    blockTypes[BLOCK_TYPE_T].color = {0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_SQUARE].cells = {0, 0, 1, 0, 0, 1, 1, 1};
    blockTypes[BLOCK_TYPE_SQUARE].color = {0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_STICK].cells = {0, 0, 0, 1, 0, 2, 0, 3};
    blockTypes[BLOCK_TYPE_STICK].color = {0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_L].cells = {0, 0, 0, 1, 0, 2, 1, 2};
    blockTypes[BLOCK_TYPE_L].color = {0x00, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_L_REVERSED].cells = {1, 0, 1, 1, 1, 2, 0, 2};
    blockTypes[BLOCK_TYPE_L_REVERSED].color = {0xFF, 0xFF, 0x00, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_DOG].cells = {1, 0, 2, 0, 0, 1, 1, 1};
    blockTypes[BLOCK_TYPE_DOG].color = {0xFF, 0xFF, 0x00, SDL_ALPHA_OPAQUE};

    blockTypes[BLOCK_TYPE_DOG_REVERSED].cells = {0, 0, 1, 0, 1, 1, 2, 1};
    blockTypes[BLOCK_TYPE_L_REVERSED].color = {0xFF, 0x00, 0xFF, SDL_ALPHA_OPAQUE};
}
std::string Game::getCurrentTimeStr()
{
    float currentTimeFormated = std::round(currentFrameTime / 10.0F) / 100;

    std::string currentTimeStr = std::to_string(currentTimeFormated);
    int dotIndex = currentTimeStr.find('.') + 1;
    currentTimeStr.erase(dotIndex + 2, currentTimeStr.length() - dotIndex);

    return currentTimeStr;
}
#include <SDL2/SDL.h>

#include <array>
#include <vector>
#include <cmath>
#include <map>

#include "./texture.hpp"
#include "./block.hpp"

#define CELLS_IN_BLOCK 4

#define ROWS_QUANTITY 20
#define COLUMNS_QUANTITY 10

#define DOUBLE_CLICK_DELAY 500

typedef struct block
{
    SDL_Color color;
    std::array<SDL_Point, CELLS_IN_BLOCK> cells;
} block;

enum arrows
{
    ARROW_NONE,
    ARROW_UP,
    ARROW_RIGHT,
    ARROW_DOWN,
    ARROW_LEFT
};

class Game
{
private:
    SDL_Window *gWindow;
    SDL_Renderer *gRenderer;

    TTF_Font *gFont;

    SDL_Event e;

    int cellW, cellH;

    Uint64 startTime;
    Uint64 currentFrameTime;
    GTexture currentFrameTimeTexture;

    int points = 0;
    GTexture pointsTexture;

    std::vector<cell> placedCells;

    TBlock currentBlock;
    blockTypesNames nextBlock;

    SDL_Rect gameViewPort;
    SDL_Rect generalViewPort;

    arrows keyPressed = ARROW_NONE;

    std::string getCurrentTimeStr();

    void handleGameResize();

    bool isBlockPlaced();
    bool isLost();

    void drawCurrentBlock();
    void drawPlacedCells();
    void drawNextBlock();
    void drawCell(SDL_Point cords, SDL_Color color);

    void placeCurrentBlock();
    void clearRows(std::vector<int> rowsToClear);

    int calcPoints(int rowsCleared);
    void addPoints(int points);

    std::vector<int> getFilledRows();

public:
    Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont);

    bool exit = false;

    void handleEvents();
    void update();
    void render();
};
Game::Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont) : gWindow(loadWindow),
                                                                                     gRenderer(loadRenderer), currentFrameTimeTexture(gRenderer), gFont(loadFont), currentBlock(placedCells), pointsTexture(gRenderer)
{
    handleGameResize();

    SDL_RenderGetViewport(gRenderer, &generalViewPort);

    currentBlock.setBlockType(static_cast<blockTypesNames>(rand() % BLOCK_TYPES_TOTAL));
    nextBlock = static_cast<blockTypesNames>(rand() % BLOCK_TYPES_TOTAL);

    currentBlock.reset();

    addPoints(0);

    startTime = SDL_GetTicks64();
}
void Game::handleEvents()
{
    static Uint64 lastArrowDownClick = 0;

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
                if (SDL_GetTicks64() - lastArrowDownClick < 500)
                {
                    lastArrowDownClick = 0;
                }
                else
                {
                    lastArrowDownClick = SDL_GetTicks64();
                }
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
        if (!currentBlock.checkColisionLeft())
        {
            currentBlock.pos.x--;
        }
        break;
    case ARROW_RIGHT:
        if (!currentBlock.checkColisionRight())
        {
            currentBlock.pos.x++;
        }
        break;
    case ARROW_UP:
        currentBlock.rotate();
        break;
    }

    if (keyPressed != ARROW_NONE)
    {
        blockMovedPlayer = true;
    }

    // Time handle
    Uint64 lastFrameTime = currentFrameTime;
    currentFrameTime = SDL_GetTicks64() - startTime;
    currentFrameTimeTexture.loadTextTexture(getCurrentTimeStr(), {0, 0, 0, SDL_ALPHA_OPAQUE}, gFont);

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

            currentBlock.setBlockType(nextBlock);
            nextBlock = static_cast<blockTypesNames>(rand() % BLOCK_TYPES_TOTAL);

            currentBlock.reset();

            std::vector<int> filledRows = getFilledRows();

            exit = isLost();

            if (!filledRows.empty())
            {
                clearRows(filledRows);

                int pointsGained = calcPoints(filledRows.size());
                addPoints(pointsGained);
            }
        }
        else if (blockAutoMoved || keyPressed == ARROW_DOWN)
        {
            currentBlock.pos.y++;
        }
    }
}
void Game::render()
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(gRenderer, &gameViewPort);

    int currentTimeTextureX = (gameViewPort.x + gameViewPort.w) + (generalViewPort.w - (gameViewPort.x + gameViewPort.w)) * (2.0 / 3.0) - currentFrameTimeTexture.getHeight();
    int currentTimeTextureY = generalViewPort.y;
    currentFrameTimeTexture.render(currentTimeTextureX, currentTimeTextureY, nullptr);

    int pointsTextureX = (gameViewPort.x + gameViewPort.w) + (generalViewPort.w - (gameViewPort.x + gameViewPort.w)) * (1.0 / 3.0) - pointsTexture.getWidth();
    int pointsTextureY = gameViewPort.y;
    pointsTexture.render(pointsTextureX, pointsTextureY, nullptr);

    SDL_RenderSetViewport(gRenderer, &gameViewPort);

    drawCurrentBlock();
    drawPlacedCells();
    drawNextBlock();

    SDL_RenderSetViewport(gRenderer, &generalViewPort);

    SDL_RenderPresent(gRenderer);
}
void Game::drawCell(SDL_Point coords, SDL_Color color)
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);

    SDL_Rect cellRect;

    cellRect.x = cellW * coords.x + 1;
    cellRect.y = cellH * coords.y + 1;

    cellRect.w = cellW - 2;
    cellRect.h = cellH - 2;

    SDL_RenderFillRect(gRenderer, &cellRect);
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_TRANSPARENT);
    // SDL_RenderDrawRect(gRenderer, &cellRect);
}
bool Game::isBlockPlaced()
{
    for (auto currentBlockCell : currentBlock.cells)
    {
        int currentBlockCellY = currentBlock.pos.y + currentBlockCell.y + 1;

        if (currentBlockCellY == ROWS_QUANTITY)
        {
            return true;
        }
        for (auto placedCell : placedCells)
        {
            if (currentBlock.pos.x + currentBlockCell.x == placedCell.pos.x && currentBlockCellY == placedCell.pos.y)
            {
                return true;
            }
        }
    }
    return false;
}
void Game::placeCurrentBlock()
{
    for (auto currentBlockCell : currentBlock.cells)
    {
        cell cellToPlace;

        cellToPlace.color = currentBlock.color;
        cellToPlace.pos.x = currentBlockCell.x + currentBlock.pos.x;
        cellToPlace.pos.y = currentBlockCell.y + currentBlock.pos.y;

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
    for (auto cell : currentBlock.cells)
    {
        SDL_Point cellDrawPoint = {cell.x + currentBlock.pos.x, cell.y + currentBlock.pos.y};

        drawCell(cellDrawPoint, currentBlock.color);
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
void Game::drawNextBlock()
{
    //TO RENAME
    int gameMargin = gameViewPort.w + gameViewPort.x;
    int infoWidth =  generalViewPort.w - gameMargin;

    int infoCenter = (infoWidth /*- block width*/) / 2;
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
int Game::calcPoints(int rowsCleared)
{
    int pointsScored;
    switch (rowsCleared)
    {
    case 1:
        pointsScored = 40;
        break;
    case 2:
        pointsScored = 100;
        break;
    case 3:
        pointsScored = 300;
        break;
    case 4:
        pointsScored = 1200;
        break;
    default:
        pointsScored = 0;
        break;
    }
    return pointsScored;
}
void Game::addPoints(int pointsToAdd)
{
    points += pointsToAdd;

    // Update points texture
    SDL_Color pointsTextureColor = {0, 0, 0, SDL_ALPHA_OPAQUE};
    pointsTexture.loadTextTexture(std::to_string(points), pointsTextureColor, gFont);
}
bool Game::isLost()
{
    bool isGameLost = false;
    for (auto placedCell : placedCells)
    {
        if (placedCell.pos.y <= 0)
        {
            isGameLost = true;
            break;
        }
    }
    return isGameLost;
}
std::string Game::getCurrentTimeStr()
{
    float currentTimeFormated = std::round(currentFrameTime / 10.0F) / 100;

    std::string currentTimeStr = std::to_string(currentTimeFormated);
    int dotIndex = currentTimeStr.find('.') + 1;
    currentTimeStr.erase(dotIndex + 2, currentTimeStr.length() - dotIndex);

    return currentTimeStr;
}
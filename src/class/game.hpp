#include <SDL2/SDL.h>

#include <array>

#include "./texture.hpp"

#define CELLS_IN_BLOCK 4

#define ROWS_QUANTITY 20
#define COLUMNS_QUANTITY 10

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
    SDL_Point currentBlockPos = {0, 0};

    Uint64 startTime;
    Uint64 currentTime;
    GTexture currentTimeTexture;

    std::array<block, BLOCK_TYPES_TOTAL> blockTypes;

    block *currentBlock = nullptr;
    block *nextBlock = nullptr;

    SDL_Rect gameViewPort;
    SDL_Rect generalViewPort;

    arrows keyPressed = ARROW_NONE;

    std::string getCurrentTimeStr();

    void handleGameResize();

    void loadBlocks();

    void getCurrentBlockMaxRightTilt();

    void renderCurrentBlock();
    void renderNextBlock();
    void renderPlacedBlocks();

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

    getCurrentBlockMaxRightTilt();

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
    if (keyPressed != ARROW_NONE)
    {
        switch (keyPressed)
        {
        case ARROW_LEFT:
            if (currentBlockPos.x != 0)
            {
                currentBlockPos.x--;
            }
            break;
        case ARROW_RIGHT:
            if (currentBlockPos.x != COLUMNS_QUANTITY + currentBlockMaxRightTilt)
            {
                currentBlockPos.x++;
            }
            break;
        default:
            break;
        }
    }

    // Time handle
    currentTime = SDL_GetTicks64() - startTime;
    currentTimeTexture.loadTextTexture(getCurrentTimeStr(), {0, 0, 0, SDL_ALPHA_OPAQUE}, gFont);
}
void Game::render()
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(gRenderer, &gameViewPort);

    currentTimeTexture.render(0, 0, nullptr);

    SDL_RenderSetViewport(gRenderer, &gameViewPort);

    renderCurrentBlock();

    SDL_RenderSetViewport(gRenderer, &generalViewPort);

    SDL_RenderPresent(gRenderer);
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
void Game::renderCurrentBlock()
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);

    for (auto cell : currentBlock->cells)
    {
        SDL_Rect cellRect;

        cellRect.x = cellW * currentBlockPos.x + cell.x * cellW;
        cellRect.y = cellH * currentBlockPos.y + cell.y * cellH;

        cellRect.w = cellW;
        cellRect.h = cellH;

        SDL_RenderFillRect(gRenderer, &cellRect);
    }
}
void Game::getCurrentBlockMaxRightTilt()
{
    int maxTilt = 0;
    for (auto cell : currentBlock->cells)
    {
        maxTilt = std::max(maxTilt, cell.x);
    }
    currentBlockMaxRightTilt = -maxTilt - 1;
}
void Game::loadBlocks()
{
    blockTypes[BLOCK_TYPE_T].cells = {0, 1, 1, 1, 2, 1, 1, 0};

    blockTypes[BLOCK_TYPE_SQUARE].cells = {0, 0, 1, 0, 0, 1, 1, 1};

    blockTypes[BLOCK_TYPE_STICK].cells = {0, 0, 0, 1, 0, 2, 0, 3};

    blockTypes[BLOCK_TYPE_L].cells = {0, 0, 0, 1, 0, 2, 1, 2};

    blockTypes[BLOCK_TYPE_L_REVERSED].cells = {1, 0, 1, 1, 1, 2, 0, 2};

    blockTypes[BLOCK_TYPE_DOG].cells = {1, 0, 2, 0, 0, 1, 1, 1};

    blockTypes[BLOCK_TYPE_DOG_REVERSED].cells = {0, 0, 1, 0, 1, 1, 2, 1};
}
std::string Game::getCurrentTimeStr()
{
    float currentTimeFormated = round(currentTime / 10.0F) / 100;

    std::string currentTimeStr = std::to_string(currentTimeFormated);
    int dotIndex = currentTimeStr.find('.') + 1;
    currentTimeStr.erase(dotIndex + 2, currentTimeStr.length() - dotIndex);

    return currentTimeStr;
}
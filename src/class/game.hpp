#include <SDL2/SDL.h>

#include <array>

#include "./texture.hpp"

typedef std::array<int, 4> block;

enum arrows
{
    ARROW_NONE,
    ARROW_UP,
    ARROW_RIGHT,
    ARROW_DOWN,
    ARROW_LEFT
};

typedef std::array<SDL_Point, 4> tBlock;

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

    Uint64 startTime;
    Uint64 currentTime;
    GTexture currentTimeTexture;

    std::array<tBlock, BLOCK_TYPES_TOTAL> blockTypes;

    arrows keyPressed = ARROW_NONE;
    blockTypesNames nextBlock;

    std::string getCurrentTimeStr();

    void handleGameResize();

    void loadBlocks();

public:
    Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont);

    SDL_Rect gameViewPort;

    bool exit = false;

    void handleEvents();
    void update();
    void render();
};
Game::Game(SDL_Window *loadWindow, SDL_Renderer *loadRenderer, TTF_Font *loadFont) : gWindow(loadWindow), gRenderer(loadRenderer), currentTimeTexture(gRenderer), gFont(loadFont)
{
    handleGameResize();
    loadBlocks();
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
    currentTime = SDL_GetTicks64() - startTime;

    currentTimeTexture.loadTextTexture(getCurrentTimeStr(), {0, 0, 0, SDL_ALPHA_OPAQUE}, gFont);
}
void Game::render()
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(gRenderer, &gameViewPort);

    // currentTimeTexture.render(0, 0, nullptr);

    SDL_RenderPresent(gRenderer);
}
void Game::handleGameResize()
{
    int screenW, screenH;

    SDL_GetWindowSize(gWindow, &screenW, &screenH);

    gameViewPort = {25, 0, 350, 700};
    gameViewPort.y = (screenH - gameViewPort.h) / 2;

    cellW = gameViewPort.w / 10;
    cellH = gameViewPort.h / 20;
}
void Game::loadBlocks()
{
    blockTypes[BLOCK_TYPE_T][0] = {0, 1};
    blockTypes[BLOCK_TYPE_T][1] = {1, 1};
    blockTypes[BLOCK_TYPE_T][2] = {2, 1};
    blockTypes[BLOCK_TYPE_T][3] = {1, 0};

    blockTypes[BLOCK_TYPE_SQUARE][0] = {0, 0};
    blockTypes[BLOCK_TYPE_SQUARE][1] = {1, 0};
    blockTypes[BLOCK_TYPE_SQUARE][2] = {0, 1};
    blockTypes[BLOCK_TYPE_SQUARE][3] = {1, 1};

    blockTypes[BLOCK_TYPE_STICK][0] = {0, 0};
    blockTypes[BLOCK_TYPE_STICK][1] = {0, 1};
    blockTypes[BLOCK_TYPE_STICK][2] = {0, 2};
    blockTypes[BLOCK_TYPE_STICK][3] = {0, 3};

    blockTypes[BLOCK_TYPE_L][0] = {0, 0};
    blockTypes[BLOCK_TYPE_L][1] = {0, 1};
    blockTypes[BLOCK_TYPE_L][2] = {0, 2};
    blockTypes[BLOCK_TYPE_L][3] = {1, 2};

    blockTypes[BLOCK_TYPE_L_REVERSED][0] = {1, 0};
    blockTypes[BLOCK_TYPE_L_REVERSED][1] = {1, 1};
    blockTypes[BLOCK_TYPE_L_REVERSED][2] = {1, 2};
    blockTypes[BLOCK_TYPE_L_REVERSED][3] = {0, 2};

    blockTypes[BLOCK_TYPE_DOG][0] = {1, 0};
    blockTypes[BLOCK_TYPE_DOG][1] = {2, 0};
    blockTypes[BLOCK_TYPE_DOG][2] = {0, 1};
    blockTypes[BLOCK_TYPE_DOG][3] = {1, 1};

    blockTypes[BLOCK_TYPE_DOG_REVERSED][0] = {0, 0};
    blockTypes[BLOCK_TYPE_DOG_REVERSED][1] = {1, 0};
    blockTypes[BLOCK_TYPE_DOG_REVERSED][2] = {1, 1};
    blockTypes[BLOCK_TYPE_DOG_REVERSED][3] = {2, 1};
}
std::string Game::getCurrentTimeStr()
{
    float currentTimeFormated = round(currentTime / 10.0F) / 100;

    std::string currentTimeStr = std::to_string(currentTimeFormated);
    int dotIndex = currentTimeStr.find('.') + 1;
    currentTimeStr.erase(dotIndex + 2, currentTimeStr.length() - dotIndex);

    return currentTimeStr;
}
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <ctime>

#include "./class/game.hpp"

bool init(SDL_Window **gWindow, SDL_Renderer **gRenderer);
void load(TTF_Font **gFont);
void close(SDL_Window *gWindow, SDL_Renderer *gRenderer,TTF_Font *gFont);

int main(int argc, char **argv)
{
    SDL_Window *gWindow = nullptr;
    SDL_Renderer *gRenderer = nullptr;

    TTF_Font *gFont = nullptr;

    srand(time(NULL));

    init(&gWindow, &gRenderer);
    load(&gFont);

    Game tGame(gWindow, gRenderer, gFont);

    while (!tGame.exit)
    {
        tGame.handleEvents();
        tGame.update();
        tGame.render();
    }

    close(gWindow, gRenderer,gFont);
    return 0;
}

bool init(SDL_Window **gWindow, SDL_Renderer **gRenderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cerr << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0)
    {
        std::cerr << TTF_GetError() << std::endl;
        return false;
    }

    *gWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 675, 750, SDL_WINDOW_SHOWN);

    if (*gWindow == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        return false;
    }

    *gRenderer = SDL_CreateRenderer(*gWindow, 01, SDL_RENDERER_ACCELERATED);

    if (*gRenderer == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}
void load(TTF_Font **gFont)
{
    *gFont = TTF_OpenFont("./bin/fonts/PixelifySans-Regular.ttf", 50);

    if (*gFont == nullptr)
    {
        std::cerr << TTF_GetError() << std::endl;
    }
}
void close(SDL_Window *gWindow, SDL_Renderer *gRenderer, TTF_Font *gFont)
{
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);

    gWindow = nullptr;
    gRenderer = nullptr;
    gFont = nullptr;

    TTF_Quit();
    SDL_Quit();
}
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <thread>

#include "./class/game.hpp"

bool init(SDL_Window **gWindow, SDL_Renderer **gRenderer);
void close(SDL_Window *gWindow, SDL_Renderer *gRenderer);

int main(int argc, char **argv)
{
    SDL_Window *gWindow = nullptr;
    SDL_Renderer *gRenderer = nullptr;

    init(&gWindow, &gRenderer);

    Game tGame(gWindow, gRenderer);

    while (!tGame.exit)
    {
        tGame.handleEvents();
        tGame.render();
    }

    close(gWindow, gRenderer);
}

bool init(SDL_Window **gWindow, SDL_Renderer **gRenderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0)
    {
        std::cout << TTF_GetError() << std::endl;
        return false;
    }

    *gWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 675, 750, SDL_WINDOW_SHOWN);

    if (*gWindow == nullptr)
    {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    *gRenderer = SDL_CreateRenderer(*gWindow, 01, SDL_RENDERER_ACCELERATED);

    if (*gRenderer == nullptr)
    {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}
void close(SDL_Window *gWindow, SDL_Renderer *gRenderer)
{
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);

    gWindow = nullptr;
    gRenderer = nullptr;

    TTF_Quit();
    SDL_Quit();
}
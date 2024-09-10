#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <iostream>

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

class GTexture
{
private:
    int height = 0;
    int width = 0;

    SDL_Texture *mTexture = nullptr;
    SDL_Renderer *gRenderer = nullptr;

public:
    GTexture(SDL_Renderer *textureRenderer) : gRenderer(textureRenderer)
    {
        free();
    }

    void loadTextTexture(std::string text, SDL_Color textColor, TTF_Font *textFont);
    void loadImgTexture(std::string path);

    int getHeight();
    int getWidth();

    void render(int x, int y, SDL_Rect *clip);

    void free();

    ~GTexture();
};
void GTexture::render(int x, int y, SDL_Rect *clip)
{
    SDL_Rect target = {x, y, width, height};

    if (clip != nullptr)
    {
        target.w = clip->w;
        target.h = clip->h;
    }

    SDL_RenderCopy(gRenderer, mTexture, clip, &target);
}
void GTexture::loadImgTexture(std::string path)
{
    free();

    SDL_Surface *loadSuface = IMG_Load(path.c_str());

    if (loadSuface == nullptr)
    {
        std::cerr << IMG_GetError() << std::endl;
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(gRenderer, loadSuface);

    if (mTexture == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        SDL_FreeSurface(loadSuface);
        return;
    }

    this->height = loadSuface->h;
    this->width = loadSuface->w;

    SDL_FreeSurface(loadSuface);
}
void GTexture::loadTextTexture(std::string text, SDL_Color textColor, TTF_Font *textFont)
{
    free();

    SDL_Surface *textSuface = TTF_RenderText_Solid(textFont, text.c_str(), textColor);
    if (textSuface == nullptr)
    {
        std::cerr << TTF_GetError() << std::endl;
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSuface);

    if (mTexture == nullptr)
    {
        std::cerr << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSuface);
        return;
    }

    this->height = textSuface->h;
    this->width = textSuface->w;

    SDL_FreeSurface(textSuface);
}
GTexture::~GTexture()
{
    free();
}
void GTexture::free()
{
    if (mTexture != nullptr)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
    height = 0;
    width = 0;
}
int GTexture::getHeight()
{
    return height;
}
int GTexture::getWidth()
{
    return width;
}
#endif
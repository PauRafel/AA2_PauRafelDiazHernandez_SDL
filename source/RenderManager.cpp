#include "RenderManager.h"
#include <iostream>
#include <exception>

RenderManager::~RenderManager()
{
    for (std::map<std::string, SDL_Texture*>::iterator it = _textures.begin(); it != _textures.end(); it++)
    {
        SDL_DestroyTexture(it->second);
        it->second = nullptr;
    }
}

void RenderManager::InitSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw SDL_GetError();
}

void RenderManager::CreateWindowAndRenderer()
{
    if (!SDL_CreateWindowAndRenderer(
        "Menace 1988 - Pau Rafel Diaz Hernandez",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE,
        &_window,
        &_renderer
    ))
        throw SDL_GetError();
}

void RenderManager::Init()
{
    try
    {
        InitSDL();
        CreateWindowAndRenderer();
    }
    catch (std::exception& exception)
    {
        std::cout << "Error: " << exception.what();
        Release();
        return;
    }

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0xFF);
}

void RenderManager::Release()
{
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void RenderManager::ClearScreen()
{
    SDL_RenderClear(_renderer);
}

void RenderManager::RenderScreen()
{
    SDL_RenderPresent(_renderer);
}

void RenderManager::LoadTexture(std::string path)
{
    if (_textures.find(path) != _textures.end())
        return;

    _textures[path] = IMG_LoadTexture(_renderer, path.c_str());
    assert(_textures[path]);
}

SDL_Texture* RenderManager::GetTexture(std::string path)
{
    if (_textures.find(path) != _textures.end())
        return _textures[path];

    return nullptr;
}
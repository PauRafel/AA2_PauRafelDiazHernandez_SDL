#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>
#include <cassert>

#define RM RenderManager::Instance()

class RenderManager
{
public:
    static RenderManager& Instance()
    {
        static RenderManager instance;
        return instance;
    }

    const unsigned int WINDOW_WIDTH = 1360;
    const unsigned int WINDOW_HEIGHT = 768;

    void Init();
    void Release();
    void ClearScreen();
    void RenderScreen();

    SDL_Renderer* GetRenderer() { return _renderer; }
    void LoadTexture(std::string path);
    SDL_Texture* GetTexture(std::string path);
    void LoadFont(std::string path, int size);
    TTF_Font* GetFont(std::string path);

private:
    RenderManager() = default;
    RenderManager(RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;
    ~RenderManager();

    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
    std::map<std::string, SDL_Texture*> _textures;
    std::map<std::string, TTF_Font*> _fonts;

    void InitSDL();
    void CreateWindowAndRenderer();
};
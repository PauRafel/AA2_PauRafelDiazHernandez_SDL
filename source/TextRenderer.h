#pragma once
#include "Renderer.h"
#include "RenderManager.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <cassert>

class TextRenderer : public Renderer
{
protected:
    std::string _text;
    SDL_Texture* _textTexture;
    bool _autoSize = true;
    Vector2 _renderOffset;

public:
    TextRenderer(Transform* transform, std::string text, std::string fontPath);
    ~TextRenderer();

    virtual void Update(float dt) override;
    virtual void Render() override;

    void SetText(std::string text);
    void SetRenderOffset(Vector2 offset) { _renderOffset = offset; }
};
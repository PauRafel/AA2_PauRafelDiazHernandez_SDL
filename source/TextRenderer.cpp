#include "TextRenderer.h"
#include <iostream>

TextRenderer::TextRenderer(Transform* transform, std::string text, std::string fontPath)
    : Renderer(transform, fontPath)
{
    _textTexture = nullptr;
    _renderOffset = Vector2(0.f, 0.f);
    SetText(text);
}

TextRenderer::~TextRenderer()
{
    if (_textTexture != nullptr)
    {
        SDL_DestroyTexture(_textTexture);
        _textTexture = nullptr;
    }
}

void TextRenderer::Update(float dt)
{
    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;

    _destRect.x = _transform->position.x + offset.x + _renderOffset.x; 
    _destRect.y = _transform->position.y + offset.y + _renderOffset.y; 

    if (_autoSize)
    {
        _destRect.w = _sourceRect.w * _transform->scale.x;
        _destRect.h = _sourceRect.h * _transform->scale.y;
    }
    else
    {
        _destRect.w = _transform->size.x * _transform->scale.x;
        _destRect.h = _transform->size.y * _transform->scale.y;
    }
}

void TextRenderer::Render()
{
    SDL_RenderTextureRotated(
        RM.GetRenderer(),
        _textTexture,
        &_sourceRect,
        &_destRect,
        _transform->rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

void TextRenderer::SetText(std::string text)
{
    if (_textTexture != nullptr)
        SDL_DestroyTexture(_textTexture);

    if (text.empty())
        text = " ";

    TTF_Font* font = RM.GetFont(_resourcePath);
    if (font == nullptr)
    {
        std::cout << "ERROR: Font not loaded at path: " << _resourcePath << std::endl;
        return;
    }

    SDL_Surface* surf = TTF_RenderText_Solid(
        font,
        text.c_str(),
        text.length(),
        _color
    );

    if (surf == nullptr)
    {
        std::cout << "ERROR: Failed to create text surface. Text: '" << text << "'" << std::endl;
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    _textTexture = SDL_CreateTextureFromSurface(RM.GetRenderer(), surf);

    if (_textTexture == nullptr)
    {
        std::cout << "ERROR: Failed to create texture from surface." << std::endl;
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(surf);
        return;
    }

    _sourceRect = { 0.f, 0.f, (float)surf->w, (float)surf->h };

    SDL_DestroySurface(surf);

    _text = text;
}
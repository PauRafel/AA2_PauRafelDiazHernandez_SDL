#pragma once
#include "Object.h"
#include "TextRenderer.h"

class TextObject : public Object
{
public:
    TextObject(std::string text, std::string fontPath)
        : Object()
    {
        _renderer = new TextRenderer(_transform, text, fontPath);
        _renderer->SetColor({ 255, 255, 255, 255 });
    }

    void SetText(std::string text)
    {
        dynamic_cast<TextRenderer*>(_renderer)->SetText(text);
    }

    void SetColor(SDL_Color color)
    {
        _renderer->SetColor(color);
    }
};
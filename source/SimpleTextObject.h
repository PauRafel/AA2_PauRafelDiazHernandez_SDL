#pragma once
#include "Object.h"
#include "RenderManager.h"
#include <string>

class SimpleTextObject : public Object
{
private:
    std::string _text;
    Vector2 _position;
    SDL_Color _color;
    float _scale;

public:
    SimpleTextObject(std::string text, Vector2 position, SDL_Color color = { 255, 255, 255, 255 }, float scale = 2.0f)
        : Object(), _text(text), _position(position), _color(color), _scale(scale)
    {
        _transform->position = position;
    }

    void Update(float dt) override
    {}

    void Render() override
    {
        SDL_FRect rect;
        rect.x = _position.x;
        rect.y = _position.y;
        rect.w = _text.length() * 40.0f * _scale;
        rect.h = 60.0f * _scale;

        SDL_SetRenderDrawColor(RM.GetRenderer(), _color.r, _color.g, _color.b, _color.a);
        SDL_RenderRect(RM.GetRenderer(), &rect);

        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
    }
};
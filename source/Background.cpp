#include "Background.h"

Background::Background(std::string texturePath)
    : ImageObject(texturePath, Vector2(0.f, 0.f), Vector2(1360.f, 768.f))
{
    _texture = RM.GetTexture(texturePath);

    _offset1 = 0.0f;
    _offset2 = RM.WINDOW_WIDTH;

    _rect1 = { _offset1, 0.f, (float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT };
    _rect2 = { _offset2, 0.f, (float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT };
}

void Background::Update(float dt)
{
    _offset1 -= _scrollSpeed * dt;
    _offset2 -= _scrollSpeed * dt;

    if (_offset1 + RM.WINDOW_WIDTH <= 0)
    {
        _offset1 = _offset2 + RM.WINDOW_WIDTH;
    }

    if (_offset2 + RM.WINDOW_WIDTH <= 0)
    {
        _offset2 = _offset1 + RM.WINDOW_WIDTH;
    }

    _rect1.x = _offset1;
    _rect2.x = _offset2;
}

void Background::Render()
{
    SDL_RenderTexture(RM.GetRenderer(), _texture, NULL, &_rect1);
    SDL_RenderTexture(RM.GetRenderer(), _texture, NULL, &_rect2);
}
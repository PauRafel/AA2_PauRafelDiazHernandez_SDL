#pragma once
#include "ImageObject.h"
#include "RenderManager.h"

class Background : public ImageObject
{
private:
    float _scrollSpeed = 200.0f;
    float _offset1;
    float _offset2;
    SDL_FRect _rect1;
    SDL_FRect _rect2;
    SDL_Texture* _texture;
    bool _isScrolling = true; 

public:
    Background(std::string texturePath);

    void Update(float dt) override;
    void Render() override;

    void StopScrolling() { _isScrolling = false; }
    void StartScrolling() { _isScrolling = true; }
    bool IsScrolling() const { return _isScrolling; }
};
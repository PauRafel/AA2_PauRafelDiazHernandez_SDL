#pragma once
#include "ImageObject.h"
#include "RenderManager.h"

class Bullet : public ImageObject
{
private:
    float _speed = 500.0f;

public:
    Bullet(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize);

    void Update(float dt) override;
};
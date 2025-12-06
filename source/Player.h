#pragma once
#include "ImageObject.h"
#include "InputManager.h"
#include "RenderManager.h"
#include <vector>

class Bullet;

class Player : public ImageObject
{
private:
    float _moveSpeed = 300.0f;
    std::vector<Bullet*>* _bulletsVector;

public:
    Player(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, std::vector<Bullet*>* bulletsVector);

    void Update(float dt) override;
    void Shoot();
};
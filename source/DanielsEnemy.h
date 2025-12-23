#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class DanielsEnemy : public Enemy
{
private:
    float _scrollSpeed;

public:
    DanielsEnemy(Vector2 spawnPosition)
        : Enemy("resources/DanielsEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 4;
        _scoreValue = 180;
        _scrollSpeed = 350.0f;

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
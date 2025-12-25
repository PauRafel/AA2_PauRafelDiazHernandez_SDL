#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class UfoEnemy : public Enemy
{
private:
    float _moveSpeed;

public:
    UfoEnemy(Vector2 spawnPosition)
        : Enemy("resources/UfoEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 3;
        _scoreValue = 200;
        _moveSpeed = 250.0f;

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _moveSpeed);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
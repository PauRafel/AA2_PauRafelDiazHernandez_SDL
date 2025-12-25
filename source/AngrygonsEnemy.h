#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class AngrygonsEnemy : public Enemy
{
private:
    float _moveSpeed;

public:
    AngrygonsEnemy(Vector2 spawnPosition)
        : Enemy("resources/AngrygonsEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 5;
        _scoreValue = 220;
        _moveSpeed = 200.0f;

        _stateMachine->SetStateSimpleMove(Vector2(0.0f, -1.0f), _moveSpeed);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class AnnoyerEnemy : public Enemy
{
private:
    float _moveSpeed;

public:
    AnnoyerEnemy(Vector2 spawnPosition)
        : Enemy("resources/AnnoyerEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 2;
        _scoreValue = 140;
        _moveSpeed = 280.0f;

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _moveSpeed);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
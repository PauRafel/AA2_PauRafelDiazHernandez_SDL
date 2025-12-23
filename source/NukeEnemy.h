#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class NukeEnemy : public Enemy
{
private:
    float _moveSpeed;

public:
    NukeEnemy(Vector2 spawnPosition)
        : Enemy("resources/NukeEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 1;
        _scoreValue = 100;
        _moveSpeed = 300.0f;

        _stateMachine->SetStateSimpleMove(Vector2(0.0f, -1.0f), _moveSpeed);
    }
};
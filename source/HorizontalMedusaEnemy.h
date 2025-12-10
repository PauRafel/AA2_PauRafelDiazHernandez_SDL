#pragma once
#include "Enemy.h"
#include <cstdlib>
#include <iostream>

class HorizontalMedusaEnemy : public Enemy
{
private:
    float _speed;

public:
    HorizontalMedusaEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 1;
        _scoreValue = 100;
        _speed = 100.0f + (rand() % 201);
        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _speed);
    }
};
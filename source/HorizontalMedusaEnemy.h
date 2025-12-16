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
        : Enemy("resources/HorizontalMedusaEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 3;
        _scoreValue = 150;
        _speed = 350.0f + (rand() % 101);
        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _speed);
    }
};
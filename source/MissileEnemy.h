#pragma once
#include "Enemy.h"
#include <cstdlib>
#include <iostream>

class MissileEnemy : public Enemy
{
private:
    float _speed;

public:
    MissileEnemy(Vector2 spawnPosition)
        : Enemy("resources/MissileEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 2;
        _scoreValue = 120;
        _speed = 400.0f + (rand() % 101);
        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _speed);
    }
};
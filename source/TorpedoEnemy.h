#pragma once
#include "Enemy.h"
#include <cstdlib>
#include <iostream>

class TorpedoEnemy : public Enemy
{
private:
    float _speed;

public:
    TorpedoEnemy(Vector2 spawnPosition)
        : Enemy("resources/TorpedoEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 3;
        _scoreValue = 150;
        _speed = 350.0f + (rand() % 101);
        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _speed);
    }
};
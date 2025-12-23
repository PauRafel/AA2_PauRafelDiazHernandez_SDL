#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class RoboKrabsEnemy : public Enemy
{
private:
    bool _isOnFloor;
    float _jumpSpeed;
    float _targetY;

public:
    RoboKrabsEnemy(Vector2 spawnPosition, bool onFloor)
        : Enemy("resources/RoboKrabsEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 10;
        _scoreValue = 300;
        _isOnFloor = onFloor;
        _jumpSpeed = 200.0f;

        if (_isOnFloor)
        {
            _transform->position.y = RM.WINDOW_HEIGHT - 80.0f;
            _targetY = 80.0f;
        }
        else
        {
            _transform->position.y = 80.0f;
            _targetY = RM.WINDOW_HEIGHT - 80.0f;
        }

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), 200.0f);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
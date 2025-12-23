#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <cmath>

class TurboChainsaw : public Enemy
{
private:
    Vector2 _circleCenter;
    float _currentAngle;
    float _angularSpeed;
    float _radius;

public:
    TurboChainsaw(Vector2 spawnPosition)
        : Enemy("resources/TurboChainsawEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 5;
        _scoreValue = 200;
        _circleCenter = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        _radius = 120.0f;
        _currentAngle = 0.0f;
        _angularSpeed = 4.0f;

        _stateMachine->SetStateCircleMove(_circleCenter, _radius, _angularSpeed, _currentAngle);
    }

    void Update(float dt) override
    {
        Enemy::Update(dt);
    }
};
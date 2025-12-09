#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <cmath>

class ChomperEnemy : public Enemy
{
private:
    Vector2 _circleCenter;      
    float _circleRadius;
    float _circleAngle;
    float _angularSpeed;
    float _driftSpeed;          

public:
    ChomperEnemy(Vector2 spawnPosition, float startAngle = 0.0f)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 1;
        _scoreValue = 130;

        _circleCenter = spawnPosition;
        _circleRadius = 60.0f;          
        _circleAngle = startAngle;      
        _angularSpeed = 4.0f;           
        _driftSpeed = 80.0f;            

    }

    void Update(float dt) override
    {
        _circleAngle += _angularSpeed * dt;

        if (_circleAngle > 2.0f * 3.14159f)
            _circleAngle -= 2.0f * 3.14159f;

        _circleCenter.x -= _driftSpeed * dt;

        _transform->position.x = _circleCenter.x + _circleRadius * cos(_circleAngle);
        _transform->position.y = _circleCenter.y + _circleRadius * sin(_circleAngle);

        if (_renderer != nullptr)
            _renderer->Update(dt);

        Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
        _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));

        if (_transform->position.x + _transform->size.x < 0)
        {
            Destroy();
        }
    }
};
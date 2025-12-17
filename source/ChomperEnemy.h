#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

class ChomperEnemy : public Enemy
{
private:
    enum ChomperState
    {
        STATE_MOVING,
        STATE_PAUSED
    };

    ChomperState _currentState;
    Vector2 _circleCenter;
    float _circleRadius;
    float _circleAngle;
    float _previousAngle;   
    float _angularSpeed;
    float _driftSpeed;

    float _pauseTimer;
    float _pauseDuration;
    float _minPauseDuration;
    float _maxPauseDuration;

public:
    ChomperEnemy(Vector2 spawnPosition, float startAngle = 0.0f)
        : Enemy("resources/ChomperEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 3;
        _scoreValue = 150;

        _currentState = STATE_MOVING;
        _circleCenter = spawnPosition;
        _circleRadius = 60.0f;
        _circleAngle = startAngle;
        _previousAngle = startAngle;
        _angularSpeed = 4.0f;
        _driftSpeed = 80.0f;

        _pauseTimer = 0.0f;
        _minPauseDuration = 0.1f; 
        _maxPauseDuration = 0.6f;  
        _pauseDuration = 0.0f;

        _transform->position.x = _circleCenter.x + _circleRadius * cos(_circleAngle);
        _transform->position.y = _circleCenter.y + _circleRadius * sin(_circleAngle);
    }

    void Update(float dt) override
    {
        switch (_currentState)
        {
        case STATE_MOVING:
            UpdateMoving(dt);
            break;

        case STATE_PAUSED:
            UpdatePaused(dt);
            break;
        }

        if (_renderer != nullptr)
            _renderer->Update(dt);

        Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
        _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));

        if (_transform->position.x + _transform->size.x < -100.0f)
        {
            _escapedOffScreen = true;
            Destroy();
        }
    }

private:
    void UpdateMoving(float dt)
    {
        _previousAngle = _circleAngle;

        _circleAngle += _angularSpeed * dt;

        if (_previousAngle < 2.0f * 3.14159f && _circleAngle >= 2.0f * 3.14159f)
        {
            _circleAngle -= 2.0f * 3.14159f;
            _previousAngle -= 2.0f * 3.14159f;

            _currentState = STATE_PAUSED;
            _pauseTimer = 0.0f;

            float randomValue = (rand() % 100) / 100.0f; 
            _pauseDuration = _minPauseDuration + randomValue * (_maxPauseDuration - _minPauseDuration);

            return; 
        }

        if (_circleAngle > 2.0f * 3.14159f)
            _circleAngle -= 2.0f * 3.14159f;

        _circleCenter.x -= _driftSpeed * dt;

        _transform->position.x = _circleCenter.x + _circleRadius * cos(_circleAngle);
        _transform->position.y = _circleCenter.y + _circleRadius * sin(_circleAngle);
    }

    void UpdatePaused(float dt)
    {
        _pauseTimer += dt;

        _circleCenter.x -= _driftSpeed * dt;

        _transform->position.x = _circleCenter.x + _circleRadius * cos(_circleAngle);
        _transform->position.y = _circleCenter.y + _circleRadius * sin(_circleAngle);

        if (_pauseTimer >= _pauseDuration)
        {
            _currentState = STATE_MOVING;
        }
    }
};
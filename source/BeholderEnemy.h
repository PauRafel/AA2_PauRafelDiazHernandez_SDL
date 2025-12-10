#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

class BeholderEnemy : public Enemy
{
private:
    enum BeholderPhase
    {
        PHASE_MOVING,
        PHASE_STOPPED
    };

    BeholderPhase _currentPhase;
    float _moveDuration;    
    float _stopDuration;   
    float _moveSpeed;
    float _edgeThreshold;  

public:
    BeholderEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 2;
        _scoreValue = 180;
        _currentPhase = PHASE_MOVING;
        _moveDuration = 1.8f;  
        _stopDuration = 0.6f;   
        _moveSpeed = 120.0f;
        _edgeThreshold = 150.0f;

        ChooseRandomDirection();
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_MOVING:
        {
            if (_stateMachine->GetStateTimer() >= _moveDuration)
            {
                _stateMachine->SetStateStay(_stopDuration);
                _currentPhase = PHASE_STOPPED;
            }
            break;
        }

        case PHASE_STOPPED:
        {
            if (_stateMachine->GetStateTimer() >= _stopDuration)
            {
                ChooseRandomDirection();
                _currentPhase = PHASE_MOVING;
            }
            break;
        }
        }

        Enemy::Update(dt);
    }

private:
    void ChooseRandomDirection()
    {
        Vector2 direction;

        bool nearLeftEdge = (_transform->position.x < _edgeThreshold);
        bool nearRightEdge = (_transform->position.x > RM.WINDOW_WIDTH - _edgeThreshold);
        bool nearTopEdge = (_transform->position.y < _edgeThreshold);
        bool nearBottomEdge = (_transform->position.y > RM.WINDOW_HEIGHT - _edgeThreshold);

        if (nearLeftEdge && nearTopEdge)
        {
            direction = Vector2(1.0f, 1.0f);
        }
        else if (nearRightEdge && nearTopEdge)
        {
            direction = Vector2(-1.0f, 1.0f);
        }
        else if (nearLeftEdge && nearBottomEdge)
        {
            direction = Vector2(1.0f, -1.0f);
        }
        else if (nearRightEdge && nearBottomEdge)
        {
            direction = Vector2(-1.0f, -1.0f);
        }
        else if (nearLeftEdge)
        {
            float randomY = (rand() % 3 - 1) * 0.7f;
            direction = Vector2(1.0f, randomY);
        }
        else if (nearRightEdge)
        {
            float randomY = (rand() % 3 - 1) * 0.7f;
            direction = Vector2(-1.0f, randomY);
        }
        else if (nearTopEdge)
        {
            float randomX = (rand() % 3 - 1) * 0.7f;
            direction = Vector2(randomX, 1.0f);
        }
        else if (nearBottomEdge)
        {
            float randomX = (rand() % 3 - 1) * 0.7f;
            direction = Vector2(randomX, -1.0f);
        }
        else
        {
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            direction = Vector2(cos(angle), sin(angle));
        }

        direction.Normalize();
        _stateMachine->SetStateSimpleMove(direction, _moveSpeed);
    }
};
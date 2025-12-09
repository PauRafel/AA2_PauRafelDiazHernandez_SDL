#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class VerticalMedusaEnemy : public Enemy
{
private:
    enum VerticalMedusaPhase
    {
        PHASE_MOVING_UP,   
        PHASE_STOPPED      
    };

    VerticalMedusaPhase _currentPhase;
    float _moveDistance;     
    float _stopDuration;    
    Vector2 _phaseStartPos; 
    float _moveSpeed;

public:
    VerticalMedusaEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 1;
        _scoreValue = 120;
        _currentPhase = PHASE_MOVING_UP;
        _moveDistance = 120.0f;  
        _stopDuration = 0.8f;    
        _moveSpeed = 150.0f;    
        _phaseStartPos = spawnPosition;

        _stateMachine->SetStateSimpleMove(Vector2(0.0f, -1.0f), _moveSpeed);
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_MOVING_UP:
        {
            float distanceTraveled = _phaseStartPos.y - _transform->position.y;

            if (distanceTraveled >= _moveDistance)
            {
                _stateMachine->SetStateStay(_stopDuration);
                _currentPhase = PHASE_STOPPED;

                _phaseStartPos = _transform->position;
            }
            break;
        }

        case PHASE_STOPPED:
        {
            if (_stateMachine->GetStateTimer() >= _stopDuration)
            {
                _stateMachine->SetStateSimpleMove(Vector2(0.0f, -1.0f), _moveSpeed);
                _currentPhase = PHASE_MOVING_UP;

                _phaseStartPos = _transform->position;
            }
            break;
        }
        }

        Enemy::Update(dt);
    }
};
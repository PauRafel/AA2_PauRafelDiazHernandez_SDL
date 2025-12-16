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
        PHASE_ENTERING,  
        PHASE_WAITING,     
        PHASE_MOVING,
        PHASE_STOPPED,
        PHASE_ESCAPING      
    };

    BeholderPhase _currentPhase;
    float _moveDuration;
    float _stopDuration;
    float _moveSpeed;
    float _edgeThreshold;

    bool _isTopSpawn;      
    Vector2 _targetPosition; 
    float _entrySpeed;

    float _waitTimer;
    float _waitDuration;

    float _escapeTimer;
    float _escapeDelay;    
    Vector2 _escapeDirection;

public:
    BeholderEnemy(Vector2 spawnPosition)
        : Enemy("resources/BeholderEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 1;
        _scoreValue = 150;
        _currentPhase = PHASE_ENTERING;
        _moveDuration = 1.8f;
        _stopDuration = 0.6f;
        _moveSpeed = 120.0f;
        _edgeThreshold = 150.0f;

        _entrySpeed = 180.0f;
        _waitTimer = 0.0f;
        _waitDuration = 2.0f;

        _escapeTimer = 0.0f;
        _escapeDelay = 10.0f; 

        float screenMiddleY = RM.WINDOW_HEIGHT / 2.0f;
        _isTopSpawn = (spawnPosition.y < screenMiddleY);

        if (_isTopSpawn)
        {
            _transform->position = Vector2(spawnPosition.x, -100.0f);
            _targetPosition = Vector2(spawnPosition.x, 100.0f); 
        }
        else
        {
            _transform->position = Vector2(spawnPosition.x, RM.WINDOW_HEIGHT + 100.0f);
            _targetPosition = Vector2(spawnPosition.x, RM.WINDOW_HEIGHT - 100.0f); 
        }

        _stateMachine->SetStateStay(0.0f);
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_ENTERING:
            UpdateEntering(dt);
            break;

        case PHASE_WAITING:
            UpdateWaiting(dt);
            break;

        case PHASE_MOVING:
        {
            _escapeTimer += dt;

            if (_escapeTimer >= _escapeDelay)
            {
                StartEscaping();
                break;
            }

            if (_stateMachine->GetStateTimer() >= _moveDuration)
            {
                _stateMachine->SetStateStay(_stopDuration);
                _currentPhase = PHASE_STOPPED;
            }
            break;
        }

        case PHASE_STOPPED:
        {
            _escapeTimer += dt;

            if (_escapeTimer >= _escapeDelay)
            {
                StartEscaping();
                break;
            }

            if (_stateMachine->GetStateTimer() >= _stopDuration)
            {
                ChooseRandomDirection();
                _currentPhase = PHASE_MOVING;
            }
            break;
        }

        case PHASE_ESCAPING:
            UpdateEscaping(dt);
            break;
        }

        Enemy::Update(dt);
    }

private:
    void UpdateEntering(float dt)
    {
        Vector2 direction = _targetPosition - _transform->position;
        float distance = direction.Magnitude();

        if (distance > 5.0f)
        {
            direction.Normalize();
            _transform->position = _transform->position + direction * _entrySpeed * dt;
        }
        else
        {
            _transform->position = _targetPosition;
            _currentPhase = PHASE_WAITING;
            _waitTimer = 0.0f;
        }
    }

    void UpdateWaiting(float dt)
    {
        _waitTimer += dt;

        if (_waitTimer >= _waitDuration)
        {
            _currentPhase = PHASE_MOVING;
            _escapeTimer = 0.0f;
            ChooseRandomDirection();
        }
    }

    void StartEscaping()
    {
        _currentPhase = PHASE_ESCAPING;

        Vector2 escapeDirection;

        float centerX = RM.WINDOW_WIDTH / 2.0f;
        float centerY = RM.WINDOW_HEIGHT / 2.0f;

        float distToLeft = _transform->position.x;
        float distToRight = RM.WINDOW_WIDTH - _transform->position.x;
        float distToTop = _transform->position.y;
        float distToBottom = RM.WINDOW_HEIGHT - _transform->position.y;

        float minDist = std::min({ distToLeft, distToRight, distToTop, distToBottom });

        if (minDist == distToLeft)
            escapeDirection = Vector2(-1.0f, 0.0f);
        else if (minDist == distToRight)
            escapeDirection = Vector2(1.0f, 0.0f);
        else if (minDist == distToTop)
            escapeDirection = Vector2(0.0f, -1.0f);
        else
            escapeDirection = Vector2(0.0f, 1.0f);

        _escapeDirection = escapeDirection;
        _stateMachine->SetStateSimpleMove(_escapeDirection, _moveSpeed * 1.5f);
    }

    void UpdateEscaping(float dt)
    {
        if (_transform->position.x < -150.0f ||
            _transform->position.x > RM.WINDOW_WIDTH + 150.0f ||
            _transform->position.y < -150.0f ||
            _transform->position.y > RM.WINDOW_HEIGHT + 150.0f)
        {
            _escapedOffScreen = true;
            Destroy();
        }
    }

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
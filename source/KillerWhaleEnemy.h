#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class KillerWhaleEnemy : public Enemy
{
private:
    enum KillerWhalePhase
    {
        PHASE_SCROLLING_ATTACHED,  
        PHASE_JUMPING,             
        PHASE_SCROLLING_AFTER_JUMP 
    };

    KillerWhalePhase _currentPhase;
    bool _isOnCeiling;          
    float _detectionRange;
    Vector2* _playerPosition;  
    float _scrollSpeed;
    float _jumpSpeed;
    float _targetY;       

public:
    KillerWhaleEnemy(Vector2 spawnPosition, bool onCeiling, Vector2* playerPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _currentPhase = PHASE_SCROLLING_ATTACHED;
        _isOnCeiling = onCeiling;
        _detectionRange = 200.0f;  
        _playerPosition = playerPosition;
        _scrollSpeed = 100.0f;
        _jumpSpeed = 500.0f;       
        _health = 2;
        _scoreValue = 200;

        if (_isOnCeiling)
        {
            _transform->position.y = 50.0f;  
            _targetY = RM.WINDOW_HEIGHT - 50.0f;  
        }
        else
        {
            _transform->position.y = RM.WINDOW_HEIGHT - 50.0f; 
            _targetY = 50.0f;  
        }

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_SCROLLING_ATTACHED:
        {
            if (_playerPosition != nullptr)
            {
                float distanceToPlayer = (_transform->position - *_playerPosition).Magnitude();

                if (distanceToPlayer <= _detectionRange)
                {
                    _currentPhase = PHASE_JUMPING;

                    Vector2 jumpDirection;
                    if (_isOnCeiling)
                        jumpDirection = Vector2(0.0f, 1.0f);  
                    else
                        jumpDirection = Vector2(0.0f, -1.0f);

                    _stateMachine->SetStateSimpleMove(jumpDirection, _jumpSpeed);
                }
            }
            break;
        }

        case PHASE_JUMPING:
        {
            bool reachedTarget = false;

            if (_isOnCeiling)
            {
                if (_transform->position.y >= _targetY)
                {
                    reachedTarget = true;
                    _transform->position.y = _targetY;
                }
            }
            else
            {
                if (_transform->position.y <= _targetY)
                {
                    reachedTarget = true;
                    _transform->position.y = _targetY;
                }
            }

            if (reachedTarget)
            {
                _currentPhase = PHASE_SCROLLING_AFTER_JUMP;
                _isOnCeiling = !_isOnCeiling;
                _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
            }
            break;
        }

        case PHASE_SCROLLING_AFTER_JUMP:
        {
            break;
        }
        }

        Enemy::Update(dt);
    }
};
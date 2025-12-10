#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>

class BubbleEnemy : public Enemy
{
private:
    enum BubblePhase
    {
        PHASE_SCROLL,     
        PHASE_CIRCLE,      
        PHASE_DIAGONAL,    
        PHASE_STRAIGHT   
    };

    BubblePhase _currentPhase;
    float _targetX;          
    float _scrollSpeed;
    bool _startedAtTop;      
    float _circleStartAngle;
    float _targetY;          

public:
    BubbleEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _currentPhase = PHASE_SCROLL;
        _targetX = RM.WINDOW_WIDTH / 3.0f;  
        _scrollSpeed = 200.0f; 
        _health = 1;
        _scoreValue = 150;

        _startedAtTop = (_transform->position.y < RM.WINDOW_HEIGHT / 2.0f);

        if (_startedAtTop)
            _targetY = RM.WINDOW_HEIGHT - 100.0f;
        else
            _targetY = 100.0f; 

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_SCROLL:
        {
            if (_transform->position.x <= _targetX)
            {
                _circleStartAngle = 0.0f;
                _stateMachine->SetStateCircleMove(_transform->position, 80.0f, 3.0f, _circleStartAngle);
                _currentPhase = PHASE_CIRCLE;
            }
            break;
        }

        case PHASE_CIRCLE:
        {
            float currentAngle = _stateMachine->GetCircleAngle();

            if (currentAngle >= 5.236f)
            {
                Vector2 diagonalDirection;
                if (_startedAtTop)
                    diagonalDirection = Vector2(1.0f, 1.0f);  
                else
                    diagonalDirection = Vector2(1.0f, -1.0f);  

                diagonalDirection.Normalize();
                _stateMachine->SetStateSimpleMove(diagonalDirection, 200.0f);
                _currentPhase = PHASE_DIAGONAL;
            }
            break;
        }

        case PHASE_DIAGONAL:
        {
            bool reachedTarget = false;

            if (_startedAtTop)
                reachedTarget = (_transform->position.y >= _targetY); 
            else
                reachedTarget = (_transform->position.y <= _targetY);  

            if (reachedTarget)
            {
                _stateMachine->SetStateSimpleMove(Vector2(1.0f, 0.0f), 250.0f);
                _currentPhase = PHASE_STRAIGHT;
            }
            break;
        }

        case PHASE_STRAIGHT:
            break;
        }

        Enemy::Update(dt);
    }
};
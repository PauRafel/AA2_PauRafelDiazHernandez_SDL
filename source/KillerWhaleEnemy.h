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
    float _backgroundScrollSpeed;

public:
    KillerWhaleEnemy(Vector2 spawnPosition, bool onCeiling, Vector2* playerPosition)
        : Enemy("resources/KillerWhaleEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _currentPhase = PHASE_SCROLLING_ATTACHED;
        _isOnCeiling = onCeiling;
        _detectionRange = 550.0f;
        _playerPosition = playerPosition;
        _scrollSpeed = 200.0f;
        _jumpSpeed = 200.0f;
        _backgroundScrollSpeed = 200.0f; 
        _health = 15;
        _scoreValue = 750;

        _transform->size = Vector2(160.f, 160.f);
        _transform->scale = Vector2(1.0f, 1.0f);

        if (_isOnCeiling)
        {
            _transform->position.y = 80.0f;
            _targetY = RM.WINDOW_HEIGHT - 80.0f;
        }
        else
        {
            _transform->position.y = RM.WINDOW_HEIGHT - 80.0f;
            _targetY = 80.0f;
        }

        _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_SCROLLING_ATTACHED:
        {
            _stateMachine->Update(dt);

            if (_playerPosition != nullptr)
            {
                float distanceToPlayer = (_transform->position - *_playerPosition).Magnitude();

                if (distanceToPlayer <= _detectionRange)
                {
                    _currentPhase = PHASE_JUMPING;

                    _stateMachine->SetStateStay(0.0f);
                }
            }
            break;
        }

        case PHASE_JUMPING:
        {
            _transform->position.x -= _backgroundScrollSpeed * dt;

            if (_isOnCeiling)
            {
                _transform->position.y += _jumpSpeed * dt;

                if (_transform->position.y >= _targetY)
                {
                    _transform->position.y = _targetY;
                    _currentPhase = PHASE_SCROLLING_AFTER_JUMP;
                    _isOnCeiling = false;
                    _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
                }
            }
            else
            {
                _transform->position.y -= _jumpSpeed * dt;

                if (_transform->position.y <= _targetY)
                {
                    _transform->position.y = _targetY;
                    _currentPhase = PHASE_SCROLLING_AFTER_JUMP;
                    _isOnCeiling = true;
                    _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), _scrollSpeed);
                }
            }
            break;
        }

        case PHASE_SCROLLING_AFTER_JUMP:
        {
            _stateMachine->Update(dt);
            break;
        }
        }

        if (_renderer != nullptr)
            _renderer->Update(dt);

        Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
        _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));

        if (_transform->position.x + _transform->size.x < -100.f ||
            _transform->position.x - _transform->size.x > RM.WINDOW_WIDTH + 100.f ||
            _transform->position.y + _transform->size.y < -100.f ||
            _transform->position.y - _transform->size.y > RM.WINDOW_HEIGHT + 100.f)
        {
            _escapedOffScreen = true;
            Destroy();
        }
    }
};
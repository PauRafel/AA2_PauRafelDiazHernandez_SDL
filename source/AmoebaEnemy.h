#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <vector>
#include <cmath>

class AmoebaEnemy;

class AmoebaClone
{
private:
    Vector2 _position;
    Vector2 _size;
    Vector2 _scale;
    std::string _texturePath;
    Vector2 _offsetFromCenter;
    bool _isDestroyed;

public:
    AmoebaClone(Vector2 position, std::string texturePath, Vector2 offsetFromCenter)
    {
        _position = position;
        _texturePath = texturePath;
        _size = Vector2(64.f, 64.f);
        _scale = Vector2(1.0f, 1.0f);
        _offsetFromCenter = offsetFromCenter;
        _isDestroyed = false;
    }

    void SetPosition(Vector2 pos) { _position = pos; }
    Vector2 GetPosition() const { return _position; }
    Vector2 GetOffsetFromCenter() const { return _offsetFromCenter; }
    bool IsDestroyed() const { return _isDestroyed; }
    void SetDestroyed(bool destroyed) { _isDestroyed = destroyed; }

    bool CheckCollision(Vector2 point, float radius)
    {
        if (_isDestroyed) return false;

        float distance = (_position - point).Magnitude();
        return distance < (32.0f + radius); 
    }

    void Render()
    {
        if (_isDestroyed) return;

        SDL_FRect destRect;
        destRect.x = _position.x - (_size.x * _scale.x) / 2.0f;
        destRect.y = _position.y - (_size.y * _scale.y) / 2.0f;
        destRect.w = _size.x * _scale.x;
        destRect.h = _size.y * _scale.y;

        SDL_FRect sourceRect = { 0.f, 0.f, 64.f, 64.f };

        SDL_RenderTexture(
            RM.GetRenderer(),
            RM.GetTexture(_texturePath),
            &sourceRect,
            &destRect
        );
    }
};

class AmoebaEnemy : public Enemy
{
private:
    enum AmoebaPhase
    {
        PHASE_APPROACHING,
        PHASE_SPLITTING,
        PHASE_CIRCLING,
        PHASE_RECOMBINING,
        PHASE_LEAVING
    };

    AmoebaPhase _currentPhase;

    Vector2 _targetCenter;
    float _approachSpeed;

    std::vector<AmoebaClone*> _clones;
    Vector2 _blockCenter;
    float _circleAngle;
    float _circleRadius;
    float _angularSpeed;
    float _totalAngleRotated;
    float _gridSpacing;

    float _recombineSpeed;

    int _totalClones;
    int _clonesDestroyed;
    int _scorePerClone;

    std::vector<Vector2> _bulletPositionsToCheck;

public:
    AmoebaEnemy(Vector2 spawnPosition)
        : Enemy("resources/AmoebaEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 8;             
        _scoreValue = 800;
        _totalClones = 8;
        _clonesDestroyed = 0;
        _scorePerClone = 100;

        _currentPhase = PHASE_APPROACHING;

        _targetCenter = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        _approachSpeed = 150.0f;

        _gridSpacing = 80.0f;
        _circleAngle = 0.0f;
        _circleRadius = 30.0f;
        _angularSpeed = 2.0f;
        _totalAngleRotated = 0.0f;

        _recombineSpeed = 200.0f;

        Vector2 direction = _targetCenter - _transform->position;
        direction.Normalize();
        _stateMachine->SetStateSimpleMove(direction, _approachSpeed);
    }

    ~AmoebaEnemy()
    {
        for (AmoebaClone* clone : _clones)
            delete clone;
        _clones.clear();
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_APPROACHING:
            UpdateApproaching(dt);
            break;

        case PHASE_SPLITTING:
            UpdateSplitting(dt);
            break;

        case PHASE_CIRCLING:
            UpdateCircling(dt);
            break;

        case PHASE_RECOMBINING:
            UpdateRecombining(dt);
            break;

        case PHASE_LEAVING:
            UpdateLeaving(dt);
            break;
        }

        if (_renderer != nullptr)
            _renderer->Update(dt);

        UpdateColliders();
    }

    void Render() override
    {
        if (_currentPhase == PHASE_APPROACHING || _currentPhase == PHASE_LEAVING)
        {
            Enemy::Render();
        }
        else if (_currentPhase == PHASE_CIRCLING || _currentPhase == PHASE_RECOMBINING)
        {
            for (AmoebaClone* clone : _clones)
                clone->Render();
        }
    }

    void TakeDamage(int damage)
    {
        if (_currentPhase == PHASE_CIRCLING || _currentPhase == PHASE_RECOMBINING)
        {
            Vector2 hitPosition = _transform->position; 

            for (AmoebaClone* clone : _clones)
            {
                if (!clone->IsDestroyed() && clone->CheckCollision(hitPosition, 32.0f))
                {
                    clone->SetDestroyed(true);
                    _clonesDestroyed++;
                    _health--;

                    std::cout << "Clone destroyed! Remaining: " << (_totalClones - _clonesDestroyed) << std::endl;

                    if (_health <= 0)
                    {
                        _escapedOffScreen = false;
                        Destroy();
                    }
                    return;
                }
            }
        }
        else
        {
            _health -= damage;
            if (_health <= 0)
            {
                _escapedOffScreen = false;
                Destroy();
            }
        }
    }

    int GetScoreValue() const 
    {
        if (_currentPhase == PHASE_CIRCLING || _currentPhase == PHASE_RECOMBINING)
        {
            return _scorePerClone;
        }
        else
        {
            return (_totalClones - _clonesDestroyed) * _scorePerClone;
        }
    }

private:
    void UpdateColliders()
    {
        if (_currentPhase == PHASE_APPROACHING || _currentPhase == PHASE_LEAVING)
        {
            Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
            _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
        }
        else if (_currentPhase == PHASE_CIRCLING || _currentPhase == PHASE_RECOMBINING)
        {
            for (AmoebaClone* clone : _clones)
            {
                if (!clone->IsDestroyed())
                {
                    Vector2 clonePos = clone->GetPosition();
                    Vector2 cloneSize(64.f, 64.f);
                    Vector2 offset = (Vector2(-cloneSize.x, -cloneSize.y) / 2.0f);
                    _physics->AddCollider(new AABB(clonePos + offset, cloneSize));
                }
            }
        }
    }

    void UpdateApproaching(float dt)
    {
        float distance = (_transform->position - _targetCenter).Magnitude();

        if (distance < 10.0f)
        {
            _transform->position = _targetCenter;
            _currentPhase = PHASE_SPLITTING;
        }
        else
        {
            _stateMachine->Update(dt);
        }
    }

    void UpdateSplitting(float dt)
    {
        _blockCenter = _transform->position;

        Vector2 gridOffsets[8] = {
            Vector2(0, -_gridSpacing),
            Vector2(_gridSpacing, -_gridSpacing),
            Vector2(_gridSpacing, 0),
            Vector2(_gridSpacing, _gridSpacing),
            Vector2(0, _gridSpacing),
            Vector2(-_gridSpacing, _gridSpacing),
            Vector2(-_gridSpacing, 0),
            Vector2(-_gridSpacing, -_gridSpacing)
        };

        for (int i = 0; i < 8; i++)
        {
            Vector2 clonePos = _blockCenter + gridOffsets[i];
            AmoebaClone* clone = new AmoebaClone(clonePos, "resources/AmoebaEnemy.png", gridOffsets[i]);
            _clones.push_back(clone);
        }

        _circleAngle = 0.0f;
        _totalAngleRotated = 0.0f;
        _currentPhase = PHASE_CIRCLING;
    }

    void UpdateCircling(float dt)
    {
        _circleAngle += _angularSpeed * dt;
        _totalAngleRotated += _angularSpeed * dt;

        _blockCenter.x = _targetCenter.x + _circleRadius * cos(_circleAngle);
        _blockCenter.y = _targetCenter.y + _circleRadius * sin(_circleAngle);

        for (AmoebaClone* clone : _clones)
        {
            Vector2 newPos = _blockCenter + clone->GetOffsetFromCenter();
            clone->SetPosition(newPos);
        }

        if (_totalAngleRotated >= 4.0f * 3.14159f)
        {
            _currentPhase = PHASE_RECOMBINING;
        }
    }

    void UpdateRecombining(float dt)
    {
        bool allReachedCenter = true;

        for (AmoebaClone* clone : _clones)
        {
            if (clone->IsDestroyed())
                continue;

            Vector2 direction = _blockCenter - clone->GetPosition();
            float distance = direction.Magnitude();

            if (distance > 5.0f)
            {
                allReachedCenter = false;
                direction.Normalize();
                clone->SetPosition(clone->GetPosition() + direction * _recombineSpeed * dt);
            }
            else
            {
                clone->SetPosition(_blockCenter);
            }
        }

        if (allReachedCenter)
        {
            for (AmoebaClone* clone : _clones)
                delete clone;
            _clones.clear();

            _transform->position = _blockCenter;

            _health = _totalClones - _clonesDestroyed;
            _scoreValue = (_totalClones - _clonesDestroyed) * _scorePerClone;

            if (_health <= 0)
            {
                Destroy();
            }
            else
            {
                _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), 150.0f);
                _currentPhase = PHASE_LEAVING;
            }
        }
    }

    void UpdateLeaving(float dt)
    {
        _stateMachine->Update(dt);

        if (_transform->position.x + _transform->size.x < 0)
        {
            _escapedOffScreen = true;
            Destroy();
        }
    }
};
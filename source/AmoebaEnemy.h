#pragma once
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
    float _circleAngle;          
    Vector2 _targetPosition;   

public:
    AmoebaClone(Vector2 position, std::string texturePath, float startAngle)
    {
        _position = position;
        _texturePath = texturePath;
        _size = Vector2(48.f, 48.f);
        _scale = Vector2(0.8f, 0.8f); 
        _circleAngle = startAngle;
    }

    void SetPosition(Vector2 pos) { _position = pos; }
    Vector2 GetPosition() const { return _position; }

    void SetCircleAngle(float angle) { _circleAngle = angle; }
    float GetCircleAngle() const { return _circleAngle; }

    void SetTargetPosition(Vector2 target) { _targetPosition = target; }
    Vector2 GetTargetPosition() const { return _targetPosition; }

    void Render()
    {
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
    Vector2 _circleCenter;
    float _circleRadius;
    float _angularSpeed;
    float _totalAngleRotated; 

    float _recombineSpeed;
    bool _clonesRecombined;

public:
    AmoebaEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 5;  
        _scoreValue = 500; 
        _currentPhase = PHASE_APPROACHING;

        _targetCenter = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        _approachSpeed = 150.0f;

        _circleRadius = 120.0f;
        _angularSpeed = 2.0f; 
        _totalAngleRotated = 0.0f;

        _recombineSpeed = 200.0f;
        _clonesRecombined = false;

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

        Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
        _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
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

private:
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
        _circleCenter = _transform->position;

        for (int i = 0; i < 8; i++)
        {
            float angle = (i * 2.0f * 3.14159f) / 8.0f;

            Vector2 clonePos = Vector2(
                _circleCenter.x + _circleRadius * cos(angle),
                _circleCenter.y + _circleRadius * sin(angle)
            );

            AmoebaClone* clone = new AmoebaClone(clonePos, "resources/enemy.png", angle);
            _clones.push_back(clone);
        }

        _totalAngleRotated = 0.0f;
        _currentPhase = PHASE_CIRCLING;
    }

    void UpdateCircling(float dt)
    {
        for (AmoebaClone* clone : _clones)
        {
            float currentAngle = clone->GetCircleAngle();
            currentAngle += _angularSpeed * dt;
            clone->SetCircleAngle(currentAngle);

            Vector2 newPos = Vector2(
                _circleCenter.x + _circleRadius * cos(currentAngle),
                _circleCenter.y + _circleRadius * sin(currentAngle)
            );
            clone->SetPosition(newPos);
        }

        _totalAngleRotated += _angularSpeed * dt;

        if (_totalAngleRotated >= 4.0f * 3.14159f)
        {
            for (AmoebaClone* clone : _clones)
                clone->SetTargetPosition(_circleCenter);

            _clonesRecombined = false;
            _currentPhase = PHASE_RECOMBINING;
        }
    }

    void UpdateRecombining(float dt)
    {
        bool allReachedCenter = true;

        for (AmoebaClone* clone : _clones)
        {
            Vector2 direction = clone->GetTargetPosition() - clone->GetPosition();
            float distance = direction.Magnitude();

            if (distance > 5.0f)
            {
                allReachedCenter = false;
                direction.Normalize();
                clone->SetPosition(clone->GetPosition() + direction * _recombineSpeed * dt);
            }
            else
            {
                clone->SetPosition(clone->GetTargetPosition());
            }
        }

        if (allReachedCenter)
        {
            for (AmoebaClone* clone : _clones)
                delete clone;
            _clones.clear();

            _transform->position = _circleCenter;

            _stateMachine->SetStateSimpleMove(Vector2(-1.0f, 0.0f), 150.0f);
            _currentPhase = PHASE_LEAVING;
        }
    }

    void UpdateLeaving(float dt)
    {
        _stateMachine->Update(dt);

        if (_transform->position.x + _transform->size.x < 0)
            Destroy();
    }
};
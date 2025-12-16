#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <vector>
#include <deque>
#include <cmath>

class CirclerSegment
{
private:
    Vector2 _position;
    Vector2 _size;
    Vector2 _scale;
    std::string _texturePath;

public:
    CirclerSegment(Vector2 position)
    {
        _position = position;
        _texturePath = "resources/CirclerEnemy_Body.png";
        _size = Vector2(48.f, 48.f);
        _scale = Vector2(0.8f, 0.8f);
    }

    void SetPosition(Vector2 pos) { _position = pos; }
    Vector2 GetPosition() const { return _position; }

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

class CirclerEnemy : public Enemy
{
private:
    enum CirclerPhase
    {
        PHASE_ENTERING,
        PHASE_CIRCLING,
        PHASE_ZIGZAG_EXIT
    };

    CirclerPhase _currentPhase;

    Vector2 _circleCenter;
    float _currentAngle;
    float _angularSpeed;
    float _initialRadius;
    float _finalRadius;
    float _currentRadius;
    int _completedCircles;
    int _maxCircles;

    float _zigzagSpeed;
    float _zigzagAmplitude;
    float _zigzagFrequency;
    float _zigzagTime;

    std::vector<CirclerSegment*> _bodySegments;
    std::deque<Vector2> _positionHistory;
    int _numSegments;
    int _segmentSpacing;

public:
    CirclerEnemy(Vector2 spawnPosition)
        : Enemy("resources/CirclerEnemy_Head.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 30;
        _scoreValue = 750;

        _currentPhase = PHASE_ENTERING;
        _circleCenter = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f);
        _initialRadius = 180.0f;  
        _finalRadius = 40.0f;    
        _currentRadius = _initialRadius;
        _currentAngle = -3.14159f / 2.0f; 
        _angularSpeed = 3.0f;    
        _completedCircles = 0;
        _maxCircles = 5;

        _zigzagSpeed = 200.0f;
        _zigzagAmplitude = 80.0f;
        _zigzagFrequency = 4.0f;
        _zigzagTime = 0.0f;

        _numSegments = 8;
        _segmentSpacing = 6;

        _transform->position = spawnPosition;

        for (int i = 0; i < _numSegments; i++)
        {
            CirclerSegment* segment = new CirclerSegment(_transform->position);
            _bodySegments.push_back(segment);
        }

        int historySize = _numSegments * _segmentSpacing + 10;
        for (int i = 0; i < historySize; i++)
        {
            _positionHistory.push_back(_transform->position);
        }
    }

    ~CirclerEnemy()
    {
        for (CirclerSegment* segment : _bodySegments)
            delete segment;
        _bodySegments.clear();
    }

    void Update(float dt) override
    {
        switch (_currentPhase)
        {
        case PHASE_ENTERING: 
            UpdateEntering(dt);
            break;

        case PHASE_CIRCLING:
            UpdateCircling(dt);
            break;

        case PHASE_ZIGZAG_EXIT:
            UpdateZigzag(dt);
            break;
        }

        _positionHistory.push_front(_transform->position);

        int maxHistorySize = _numSegments * _segmentSpacing + 10;
        while (_positionHistory.size() > maxHistorySize)
        {
            _positionHistory.pop_back();
        }

        for (int i = 0; i < _bodySegments.size(); i++)
        {
            int historyIndex = (i + 1) * _segmentSpacing;

            if (historyIndex < _positionHistory.size())
            {
                _bodySegments[i]->SetPosition(_positionHistory[historyIndex]);
            }
        }

        if (_renderer != nullptr)
            _renderer->Update(dt);

        Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
        _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));

        if (_currentPhase == PHASE_ZIGZAG_EXIT)
        {
            if (_transform->position.y < -100.0f)
            {
                _escapedOffScreen = true;
                Destroy();
            }
        }
    }

    void Render() override
    {
        for (CirclerSegment* segment : _bodySegments)
        {
            segment->Render();
        }

        Enemy::Render();
    }

private:
    void UpdateEntering(float dt)
    {
        float targetY = _circleCenter.y + _initialRadius * sin(_currentAngle);

        if (_transform->position.y < targetY)
        {
            _transform->position.y += 250.0f * dt; 

            if (_transform->position.y >= targetY)
            {
                _transform->position.y = targetY;
                _currentPhase = PHASE_CIRCLING;
            }
        }
    }
    void UpdateCircling(float dt)
    {
        float previousAngle = _currentAngle;
        _currentAngle += _angularSpeed * dt;

        if (previousAngle < 2.0f * 3.14159f && _currentAngle >= 2.0f * 3.14159f)
        {
            _completedCircles++;
            _currentAngle -= 2.0f * 3.14159f; 

            if (_completedCircles >= _maxCircles)
            {
                _currentPhase = PHASE_ZIGZAG_EXIT;
                _zigzagTime = 0.0f;
                return;
            }
        }

        float progress = (float)_completedCircles / (float)_maxCircles;
        _currentRadius = _initialRadius + (_finalRadius - _initialRadius) * progress;

        _transform->position.x = _circleCenter.x + _currentRadius * cos(_currentAngle);
        _transform->position.y = _circleCenter.y + _currentRadius * sin(_currentAngle);
    }

    void UpdateZigzag(float dt)
    {
        _zigzagTime += dt;

        _transform->position.y -= _zigzagSpeed * dt;

        float zigzagOffset = _zigzagAmplitude * sin(_zigzagFrequency * _zigzagTime);
        _transform->position.x = _circleCenter.x + zigzagOffset;
    }
};
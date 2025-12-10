#pragma once
#include "Enemy.h"
#include "RenderManager.h"
#include <iostream>
#include <vector>
#include <deque>

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
        _texturePath = "resources/enemy.png";  
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
    std::vector<CirclerSegment*> _bodySegments;  
    std::deque<Vector2> _positionHistory;        
    int _numSegments;                            
    int _segmentSpacing;                      

public:
    CirclerEnemy(Vector2 spawnPosition)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), spawnPosition)
    {
        _health = 3; 
        _scoreValue = 250;
        _numSegments = 8;     
        _segmentSpacing = 10;    

        Vector2 circleCenter = spawnPosition;
        float radius = 100.0f;
        float angularSpeed = 3.0f;
        float startAngle = 0.0f;

        _stateMachine->SetStateCircleMove(circleCenter, radius, angularSpeed, startAngle);

        for (int i = 0; i < _numSegments; i++)
        {
            CirclerSegment* segment = new CirclerSegment(spawnPosition);
            _bodySegments.push_back(segment);
        }

        int historySize = _numSegments * _segmentSpacing + 1;
        for (int i = 0; i < historySize; i++)
        {
            _positionHistory.push_back(spawnPosition);
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
        Enemy::Update(dt);

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
    }

    void Render() override
    {
        for (CirclerSegment* segment : _bodySegments)
        {
            segment->Render();
        }

        Enemy::Render();
    }
};
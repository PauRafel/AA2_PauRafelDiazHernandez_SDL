#include "PowerUp.h"

PowerUp::PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _cycleIndex = 0;
    _currentType = _powerUpCycle[_cycleIndex];
    _hitsToNextState = 5;
    _currentHits = 0;
    _scrollSpeed = 100.0f; 

    _transform->size = Vector2(48.f, 48.f);
    _transform->scale = Vector2(1.f, 1.f);
    _transform->position = spawnPosition;

    _physics->AddCollider(new AABB(_transform->position, _transform->size));
}

void PowerUp::Update(float dt)
{
    _transform->position.x -= _scrollSpeed * dt;

    if (_transform->position.x + _transform->size.x < 0)
    {
        Destroy();
    }

    if (_renderer != nullptr)
        _renderer->Update(dt);

    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
    _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
}

void PowerUp::Hit()
{
    _currentHits++;

    if (_currentHits >= _hitsToNextState)
    {
        _currentHits = 0;  
        _cycleIndex = (_cycleIndex + 1) % 8; 
        _currentType = _powerUpCycle[_cycleIndex];
    }
}
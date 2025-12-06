#include "PowerUp.h"

PowerUp::PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition, PowerUpType type)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _type = type;
    _hitsRequired = 5;
    _currentHits = 0;
    _activated = false;

    _transform->size = Vector2(48.f, 48.f);
    _transform->scale = Vector2(1.f, 1.f);
    _transform->position = spawnPosition;

    _physics->AddCollider(new AABB(_transform->position, _transform->size));
}

void PowerUp::Update(float dt)
{
    if (_renderer != nullptr)
        _renderer->Update(dt);

    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;

    _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
}

void PowerUp::Hit()
{
    _currentHits++;

    if (_currentHits >= _hitsRequired)
    {
        _activated = true;
    }
}
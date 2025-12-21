#include "PowerUp.h"

PowerUp::PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _cycleIndex = 0;
    _currentType = _powerUpCycle[_cycleIndex];
    _hitsToNextState = 5;
    _currentHits = 0;
    _scrollSpeed = 100.0f; 

    _transform->size = Vector2(60.f, 32.f);
    _transform->scale = Vector2(1.f, 1.f);
    _transform->position = spawnPosition;

    _physics->AddCollider(new AABB(_transform->position, _transform->size));

    UpdateSprite();
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
        _cycleIndex = (_cycleIndex + 1) % 9; 
        _currentType = _powerUpCycle[_cycleIndex];

        UpdateSprite();
    }
}

void PowerUp::UpdateSprite()
{
    ImageRenderer* imgRenderer = dynamic_cast<ImageRenderer*>(_renderer);

    if (imgRenderer != nullptr)
    {
        delete _renderer;
        _renderer = new ImageRenderer(
            _transform,
            _powerUpTextures[_cycleIndex],
            Vector2(0.f, 0.f),
            Vector2(60.f, 32.f)
        );
    }
}
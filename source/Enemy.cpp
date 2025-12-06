#include "Enemy.h"

Enemy::Enemy(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _transform->size = Vector2(64.f, 64.f);
    _transform->scale = Vector2(1.f, 1.f);

    _centerPoint = spawnPosition;
    _radius = 100.0f;
    _angle = 0.0f;
    _angularSpeed = 2.0f;

    _transform->position = Vector2(
        _centerPoint.x + _radius * cos(_angle),
        _centerPoint.y + _radius * sin(_angle)
    );

    _physics->AddCollider(new AABB(_transform->position, _transform->size));
}

void Enemy::Update(float dt)
{
    _angle += _angularSpeed * dt;

    if (_angle > 2.0f * 3.14159f)
        _angle -= 2.0f * 3.14159f;

    _transform->position = Vector2(
        _centerPoint.x + _radius * cos(_angle),
        _centerPoint.y + _radius * sin(_angle)
    );

    if (_renderer != nullptr)
        _renderer->Update(dt);

    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
    _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
}
#include "Enemy.h"

Enemy::Enemy(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _transform->size = Vector2(64.f, 64.f);
    _transform->scale = Vector2(1.f, 1.f);
    _transform->position = spawnPosition;

    _physics->AddCollider(new AABB(_transform->position, _transform->size));

    _stateMachine = new EnemyStateMachine(_transform);

    _health = 1;
    _scoreValue = 100;
    _escapedOffScreen = false;

    _stateMachine->SetStateCircleMove(spawnPosition, 100.0f, 2.0f, 0.0f);
}

Enemy::~Enemy()
{
    if (_stateMachine != nullptr)
    {
        delete _stateMachine;
        _stateMachine = nullptr;
    }
}

void Enemy::Update(float dt)
{
    if (_stateMachine != nullptr)
        _stateMachine->Update(dt);

    if (_transform->position.x + _transform->size.x < -100.f ||
        _transform->position.x - _transform->size.x > RM.WINDOW_WIDTH + 100.f ||
        _transform->position.y + _transform->size.y < -100.f ||
        _transform->position.y - _transform->size.y > RM.WINDOW_HEIGHT + 100.f)
    {
        _escapedOffScreen = true;
        Destroy();
    }

    if (_renderer != nullptr)
        _renderer->Update(dt);

    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;
    _physics->AddCollider(new AABB(_transform->position + offset, _transform->size * _transform->scale));
}

void Enemy::TakeDamage(int damage)
{
    _health -= damage;
    if (_health <= 0)
    {
        _escapedOffScreen = false;
        Destroy();
    }
}
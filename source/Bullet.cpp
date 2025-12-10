#include "Bullet.h"

Bullet::Bullet(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _transform->size = Vector2(32.f, 32.f);
    _transform->scale = Vector2(1.f, 1.f);

    _physics->AddCollider(new AABB(_transform->position, _transform->size));

    _physics->SetVelocity(Vector2(_speed, 0.f));
}

void Bullet::Update(float dt)
{
    if (_isDecorative)
    {
        _lifeTime += dt;
        if (_lifeTime >= _maxLifeTime)
        {
            Destroy();
            return;
        }
    }

    if (_isPlayerBullet)
    {
        if (_transform->position.x - _transform->size.x > RM.WINDOW_WIDTH)
            Destroy();
    }
    else
    {
        if (_transform->position.x + _transform->size.x < 0)
            Destroy();

        if (_transform->position.y + _transform->size.y < 0 ||
            _transform->position.y - _transform->size.y > RM.WINDOW_HEIGHT)
            Destroy();
    }

    Object::Update(dt);
}
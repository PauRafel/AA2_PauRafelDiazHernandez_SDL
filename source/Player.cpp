#include "Player.h"
#include "Bullet.h"

Player::Player(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, std::vector<Bullet*>* bulletsVector)
    : ImageObject(texturePath, sourceOffset, sourceSize)
{
    _bulletsVector = bulletsVector;

    _transform->size = Vector2(64.f, 64.f);
    _transform->position = Vector2(200.f, RM.WINDOW_HEIGHT / 2.0f);
    _transform->scale = Vector2(1.f, 1.f);

    _physics->AddCollider(new AABB(_transform->position, _transform->size));
    _physics->SetLinearDrag(5.0f);
}

void Player::Update(float dt)
{
    Vector2 direction(0.f, 0.f);

    if (IM.GetEvent(SDLK_W, HOLD) || IM.GetEvent(SDLK_UP, HOLD))
        direction.y -= 1.0f;

    if (IM.GetEvent(SDLK_S, HOLD) || IM.GetEvent(SDLK_DOWN, HOLD))
        direction.y += 1.0f;

    if (IM.GetEvent(SDLK_A, HOLD) || IM.GetEvent(SDLK_LEFT, HOLD))
        direction.x -= 1.0f;

    if (IM.GetEvent(SDLK_D, HOLD) || IM.GetEvent(SDLK_RIGHT, HOLD))
        direction.x += 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        direction.Normalize();
        _physics->AddForce(direction * _moveSpeed);
    }

    if (IM.GetEvent(SDLK_SPACE, DOWN))
    {
        Shoot();
    }

    if (_transform->position.x < 0)
        _transform->position.x = 0;
    if (_transform->position.x > RM.WINDOW_WIDTH)
        _transform->position.x = RM.WINDOW_WIDTH;
    if (_transform->position.y < 0)
        _transform->position.y = 0;
    if (_transform->position.y > RM.WINDOW_HEIGHT)
        _transform->position.y = RM.WINDOW_HEIGHT;

    Object::Update(dt);
}

void Player::Shoot()
{
    Bullet* bullet = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
    bullet->GetTransform()->position = _transform->position;
    _bulletsVector->push_back(bullet);
}
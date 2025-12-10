#pragma once
#include "ImageObject.h"
#include "RenderManager.h"

class Bullet : public ImageObject
{
private:
    float _speed = 500.0f;
    bool _isPlayerBullet = true;
    bool _isDecorative = false;     
    float _lifeTime = 0.0f;        
    float _maxLifeTime = 3.0f;      

public:
    Bullet(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize);

    void Update(float dt) override;

    bool IsPlayerBullet() const { return _isPlayerBullet; }
    void SetIsPlayerBullet(bool value) { _isPlayerBullet = value; }

    bool IsDecorative() const { return _isDecorative; }
    void SetIsDecorative(bool value)
    {
        _isDecorative = value;
        if (_isDecorative)
        {
            _physics->SetVelocity(Vector2(0.f, 0.f));
        }
    }

    void SetSpeed(float speed) { _speed = speed; }
    void SetDirection(Vector2 direction)
    {
        direction.Normalize();
        _physics->SetVelocity(direction * _speed);
    }
};
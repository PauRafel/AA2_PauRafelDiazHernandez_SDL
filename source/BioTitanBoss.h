#pragma once
#include "Enemy.h"
#include "Bullet.h"
#include "RenderManager.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

class BioTitanBoss : public Enemy
{
private:
    std::vector<Bullet*>* _bulletsVector;

    Vector2 _eyePosition;
    Vector2 _eyeSize;

    float _shootTimer;
    float _shootCooldown;
    int _minBullets;
    int _maxBullets;
    float _bulletSpeed;

    Vector2 _spriteSize;

public:
    BioTitanBoss(Vector2 bossPosition, std::vector<Bullet*>* bulletsVector)
        : Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), bossPosition)
    {
        _health = 50; 
        _scoreValue = 5000;
        _bulletsVector = bulletsVector;

        _spriteSize = Vector2(256.f, 256.f);
        _transform->size = _spriteSize;
        _transform->scale = Vector2(2.0f, 2.0f); 

        _eyePosition = bossPosition;  
        _eyeSize = Vector2(48.f, 48.f); 

        _shootTimer = 0.0f;
        _shootCooldown = 2.0f;
        _minBullets = 3;
        _maxBullets = 7;
        _bulletSpeed = 180.0f;

        _stateMachine->SetStateStay(0.0f);

        UpdateEyeCollider();
    }

    void Update(float dt) override
    {
        _shootTimer += dt;

        if (_shootTimer >= _shootCooldown)
        {
            Shoot();
            _shootTimer = 0.0f;
        }

        if (_renderer != nullptr)
            _renderer->Update(dt);

        UpdateEyeCollider();
    }

    void Render() override
    {
        Enemy::Render();
        RenderEyeDebug();
    }

    void TakeDamage(int damage)
    {
        _health -= damage;

        if (_health <= 0)
        {
            _health = 0;
            Destroy();
        }
    }

private:
    void UpdateEyeCollider()
    {
        Vector2 eyeOffset = (Vector2(-_eyeSize.x, -_eyeSize.y) / 2.0f);
        _physics->AddCollider(new AABB(_eyePosition + eyeOffset, _eyeSize));
    }

    void RenderEyeDebug()
    {
        SDL_FRect eyeRect;
        eyeRect.x = _eyePosition.x - (_eyeSize.x / 2.0f);
        eyeRect.y = _eyePosition.y - (_eyeSize.y / 2.0f);
        eyeRect.w = _eyeSize.x;
        eyeRect.h = _eyeSize.y;

        SDL_SetRenderDrawColor(RM.GetRenderer(), 255, 0, 0, 128);
        SDL_RenderFillRect(RM.GetRenderer(), &eyeRect);
    }

    void Shoot()
    {
        if (_bulletsVector == nullptr)
            return;

        int numBullets = _minBullets + (rand() % (_maxBullets - _minBullets + 1));

        Vector2 shootOrigin = _eyePosition;

        float minAngle = 135.0f;
        float maxAngle = 225.0f;
        float angleRange = maxAngle - minAngle;

        float sectorSize = angleRange / numBullets;

        for (int i = 0; i < numBullets; i++)
        {
            float baseSectorAngle = minAngle + (i * sectorSize);

            float randomOffset = (rand() % 100) / 100.0f * sectorSize * 0.7f;
            float finalAngle = baseSectorAngle + randomOffset;

            float angleRad = finalAngle * 3.14159f / 180.0f;

            Bullet* bullet = new Bullet("resources/bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
            bullet->GetTransform()->position = shootOrigin;  
            bullet->GetTransform()->scale = Vector2(1.5f, 1.5f);  

            bullet->SetIsPlayerBullet(false);

            Vector2 direction(cos(angleRad), sin(angleRad));
            bullet->SetSpeed(_bulletSpeed);
            bullet->SetDirection(direction);

            _bulletsVector->push_back(bullet);
        }
    }
};
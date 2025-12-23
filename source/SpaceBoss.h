#pragma once
#include "Enemy.h"
#include "Bullet.h"
#include "RenderManager.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

class SpaceBoss : public Enemy
{
private:
    enum BossPhase
    {
        PHASE_ENTERING,
        PHASE_FIGHTING,
        PHASE_DYING
    };

    BossPhase _currentPhase;
    std::vector<Bullet*>* _bulletsVector;

    Vector2 _eyePosition;
    Vector2 _eyeSize;
    Vector2 _targetPosition;
    float _entrySpeed;
    float _backgroundScrollSpeed;

    float _shootTimer;
    float _shootCooldown;
    int _minBullets;
    int _maxBullets;
    float _bulletSpeed;

    Vector2 _spriteSize;
    bool _isFullyVisible;

public:
    SpaceBoss(Vector2 bossPosition, std::vector<Bullet*>* bulletsVector)
        : Enemy("resources/SpaceBossEnemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), bossPosition)
    {
        _currentPhase = PHASE_ENTERING;
        _health = 50;
        _scoreValue = 5000;
        _bulletsVector = bulletsVector;

        _spriteSize = Vector2(256.f, 256.f);
        _transform->size = _spriteSize;
        _transform->scale = Vector2(2.0f, 2.0f);

        float bossWidth = _spriteSize.x * _transform->scale.x;

        _transform->position = Vector2(RM.WINDOW_WIDTH + bossWidth, RM.WINDOW_HEIGHT / 2.f);

        _eyePosition = _transform->position;
        _eyeSize = Vector2(80.f, 80.f);

        _targetPosition = Vector2(RM.WINDOW_WIDTH - (bossWidth / 2.f) - 50.f, RM.WINDOW_HEIGHT / 2.f);

        _entrySpeed = 0.0f;
        _backgroundScrollSpeed = 200.0f;
        _isFullyVisible = false;

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
        switch (_currentPhase)
        {
        case PHASE_ENTERING:
            UpdateEntering(dt);
            break;

        case PHASE_FIGHTING:
            UpdateFighting(dt);
            break;

        case PHASE_DYING:
            break;
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
        if (_currentPhase != PHASE_FIGHTING)
            return;

        _health -= damage;

        if (_health <= 0)
        {
            _health = 0;
            _currentPhase = PHASE_DYING;
            Destroy();
        }
    }

    bool IsFullyVisible() const { return _isFullyVisible; }

private:
    void UpdateEntering(float dt)
    {
        _transform->position.x -= _backgroundScrollSpeed * dt;
        _eyePosition = _transform->position;

        if (_transform->position.x <= _targetPosition.x)
        {
            _transform->position.x = _targetPosition.x;
            _eyePosition.x = _targetPosition.x;
            _isFullyVisible = true;
            _currentPhase = PHASE_FIGHTING;
            std::cout << "SPACE BOSS READY TO FIGHT!" << std::endl;
        }
    }

    void UpdateFighting(float dt)
    {
        _shootTimer += dt;

        if (_shootTimer >= _shootCooldown)
        {
            Shoot();
            _shootTimer = 0.0f;
        }
    }

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

            Bullet* bullet = new Bullet("resources/SpaceBoss_Bullet.png", Vector2(0.f, 0.f), Vector2(32.f, 32.f));
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
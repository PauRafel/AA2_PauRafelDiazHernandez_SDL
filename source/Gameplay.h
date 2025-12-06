#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "PowerUp.h"
#include "RenderManager.h"
#include <vector>

class Gameplay : public Scene
{
private:
    Player* _player = nullptr;
    Enemy* _enemy = nullptr;
    Background* _background = nullptr;
    std::vector<Bullet*> _bullets;
    std::vector<PowerUp*> _powerups;

    int _enemiesKilledInWave = 0;
    bool _powerUpSpawned = false;

public:
    Gameplay() = default;

    void OnEnter() override
    {
        _background = new Background("resources/background.png");

        _player = new Player("resources/player.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), &_bullets);
        _objects.push_back(_player);

        _enemy = new Enemy("resources/enemy.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f),
            Vector2(RM.WINDOW_WIDTH - 300.f, RM.WINDOW_HEIGHT / 2.0f));
        _objects.push_back(_enemy);

        _enemiesKilledInWave = 0;
        _powerUpSpawned = false;
    }

    void OnExit() override
    {
        _player = nullptr;
        _enemy = nullptr;

        if (_background != nullptr)
        {
            delete _background;
            _background = nullptr;
        }

        for (Bullet* bullet : _bullets)
            delete bullet;
        _bullets.clear();

        for (PowerUp* powerup : _powerups)
            delete powerup;
        _powerups.clear();

        Scene::OnExit();
    }

    void Update(float dt) override
    {
        if (_background != nullptr)
            _background->Update(dt);

        for (int i = _bullets.size() - 1; i >= 0; i--)
        {
            if (_bullets[i]->IsPendingDestroy())
            {
                delete _bullets[i];
                _bullets.erase(_bullets.begin() + i);
            }
            else
            {
                _bullets[i]->Update(dt);
            }
        }

        for (int i = _powerups.size() - 1; i >= 0; i--)
        {
            if (_powerups[i]->IsPendingDestroy())
            {
                delete _powerups[i];
                _powerups.erase(_powerups.begin() + i);
            }
            else
            {
                _powerups[i]->Update(dt);
            }
        }

        if (_enemy != nullptr && !_enemy->IsPendingDestroy())
        {
            for (Bullet* bullet : _bullets)
            {
                if (!bullet->IsPendingDestroy() &&
                    bullet->GetRigidBody()->CheckCollision(_enemy->GetRigidBody()))
                {
                    bullet->Destroy();
                    _enemy->Destroy();
                    _enemiesKilledInWave++;

                    if (_player != nullptr)
                        _player->AddScore(100);

                    if (!_powerUpSpawned)
                    {
                        SpawnPowerUp(_enemy->GetTransform()->position);
                        _powerUpSpawned = true;
                    }
                }
            }
        }

        for (PowerUp* powerup : _powerups)
        {
            if (!powerup->IsPendingDestroy() && !powerup->IsActivated())
            {
                for (Bullet* bullet : _bullets)
                {
                    if (!bullet->IsPendingDestroy() &&
                        bullet->GetRigidBody()->CheckCollision(powerup->GetRigidBody()))
                    {
                        bullet->Destroy();
                        powerup->Hit();

                        if (powerup->IsActivated() && _player != nullptr)
                        {
                            ApplyPowerUpToPlayer(powerup->GetType());
                            powerup->Destroy();
                        }
                    }
                }
            }
        }

        Scene::Update(dt);
    }

    void Render() override
    {
        if (_background != nullptr)
            _background->Render();

        Scene::Render();

        for (Bullet* bullet : _bullets)
            bullet->Render();

        for (PowerUp* powerup : _powerups)
            powerup->Render();
    }

private:
    void SpawnPowerUp(Vector2 position)
    {
        PowerUp* powerup = new PowerUp(
            "resources/powerup.png",
            Vector2(0.f, 0.f),
            Vector2(48.f, 48.f),
            position,
            POWERUP_SCORE
        );
        _powerups.push_back(powerup);
    }

    void ApplyPowerUpToPlayer(PowerUpType type)
    {
        if (_player == nullptr)
            return;

        switch (type)
        {
        case POWERUP_SCORE:
            _player->ApplyPowerUp(STATE_NONE);
            break;
        case POWERUP_CA1:
            _player->ApplyPowerUp(STATE_CA1);
            break;
        case POWERUP_LA1:
            _player->ApplyPowerUp(STATE_LA1);
            break;
        case POWERUP_SPEED:
            _player->ApplyPowerUp(STATE_SPEED);
            break;
        case POWERUP_CA2:
            _player->ApplyPowerUp(STATE_CA2);
            break;
        case POWERUP_LA2:
            _player->ApplyPowerUp(STATE_LA2);
            break;
        case POWERUP_SHIELD:
            _player->ApplyPowerUp(STATE_SHIELD);
            break;
        case POWERUP_TURRETS:
            _player->ApplyPowerUp(STATE_TURRETS);
            break;
        }
    }
};
#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "RenderManager.h"

class Gameplay : public Scene
{
private:
    Player* _player = nullptr;
    Enemy* _enemy = nullptr;
    Background* _background = nullptr;
    std::vector<Bullet*> _bullets;

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

        if (_enemy != nullptr && !_enemy->IsPendingDestroy())
        {
            for (Bullet* bullet : _bullets)
            {
                if (!bullet->IsPendingDestroy() &&
                    bullet->GetRigidBody()->CheckCollision(_enemy->GetRigidBody()))
                {
                    bullet->Destroy();
                    _enemy->Destroy();
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
    }
};
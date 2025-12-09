#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "BubbleEnemy.h"
#include "HorizontalMedusaEnemy.h"
#include "KillerWhaleEnemy.h"
#include "VerticalMedusaEnemy.h"
#include "Bullet.h"
#include "Background.h"
#include "PowerUp.h"
#include "TextObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

class Gameplay : public Scene
{
private:
    Player* _player = nullptr;
    Enemy* _enemy = nullptr;
    Background* _background = nullptr;
    std::vector<Bullet*> _bullets;
    std::vector<PowerUp*> _powerups;

    TextObject* _scoreText = nullptr;
    TextObject* _shieldText = nullptr;
    TextObject* _cannonAmmoText = nullptr;
    TextObject* _laserAmmoText = nullptr;
    TextObject* _powerUpInfoText = nullptr; 

    int _enemiesKilledInWave = 0;
    bool _powerUpSpawned = false;
    int _powerUpCycleIndex = 0;

    float _enemyRespawnTimer = 0.0f;
    float _enemyRespawnDelay = 3.0f;
    bool _waitingForRespawn = false;

    PowerUpType _powerUpCycle[8] = {
        POWERUP_SCORE,
        POWERUP_CA1,
        POWERUP_LA1,
        POWERUP_SPEED,
        POWERUP_CA2,
        POWERUP_LA2,
        POWERUP_SHIELD,
        POWERUP_TURRETS
    };

    std::string _powerUpNames[8] = {
        "NEXT: +1000 SCORE",
        "NEXT: CANNONS LV1",
        "NEXT: LASER LV1",
        "NEXT: SPEED BOOST",
        "NEXT: CANNONS LV2",
        "NEXT: LASER LV2",
        "NEXT: SHIELD RESTORE",
        "NEXT: TURRETS"
    };

public:
    Gameplay() = default;

    void OnEnter() override
    {
        _background = new Background("resources/background.png");

        _player = new Player("resources/player.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), &_bullets);
        _objects.push_back(_player);

        SpawnEnemy();
        CreateHUD();

        _enemiesKilledInWave = 0;
        _powerUpSpawned = false;
        _powerUpCycleIndex = 0;
        _waitingForRespawn = false;
        _enemyRespawnTimer = 0.0f;

        srand((unsigned int)time(NULL));
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

        UpdateHUD();

        if (_waitingForRespawn)
        {
            _enemyRespawnTimer += dt;
            if (_enemyRespawnTimer >= _enemyRespawnDelay)
            {
                SpawnEnemy();
                _waitingForRespawn = false;
                _enemyRespawnTimer = 0.0f;
                _powerUpSpawned = false; 
            }
        }

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

                    Vector2 enemyPos = _enemy->GetTransform()->position;

                    _enemy->Destroy();
                    _enemiesKilledInWave++;

                    if (_player != nullptr)
                        _player->AddScore(100);

                    if (!_powerUpSpawned)
                    {
                        SpawnPowerUp(enemyPos);
                        _powerUpSpawned = true;
                    }

                    _waitingForRespawn = true;

                    break;
                }
            }
        }

        for (PowerUp* powerup : _powerups)
        {
            if (!powerup->IsPendingDestroy())
            {
                for (Bullet* bullet : _bullets)
                {
                    if (!bullet->IsPendingDestroy() &&
                        bullet->GetRigidBody()->CheckCollision(powerup->GetRigidBody()))
                    {
                        bullet->Destroy();
                        powerup->Hit(); 
                    }
                }
            }
        }

        if (_player != nullptr)
        {
            for (PowerUp* powerup : _powerups)
            {
                if (!powerup->IsPendingDestroy() &&
                    _player->GetRigidBody()->CheckCollision(powerup->GetRigidBody()))
                {
                    ApplyPowerUpToPlayer(powerup->GetCurrentType());

                    _powerUpCycleIndex = (powerup->GetCycleIndex() + 1) % 8;

                    powerup->Destroy();
                }
            }
        }

        if (_enemy != nullptr && _enemy->IsPendingDestroy())
        {
            _enemy = nullptr;
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
    void SpawnEnemy()
    {
        
        int testEnemyType = 4;  

        Vector2 spawnPos = Vector2(RM.WINDOW_WIDTH - 50.f, RM.WINDOW_HEIGHT / 2.0f);
        Enemy* newEnemy = nullptr;

        switch (testEnemyType)
        {
        case 0:
        {
            spawnPos.y = (rand() % 2 == 0) ? 150.0f : RM.WINDOW_HEIGHT - 150.0f;
            newEnemy = new BubbleEnemy(spawnPos);
        }
            break;
        case 1:
        {
            spawnPos.y = 100.0f + (rand() % (int)(RM.WINDOW_HEIGHT - 200.0f));
            newEnemy = new HorizontalMedusaEnemy(spawnPos);
        }
            break;
        case 2:
        {
            bool onCeiling = (rand() % 2 == 0); 
            Vector2* playerPos = (_player != nullptr) ? &(_player->GetTransform()->position) : nullptr;
            newEnemy = new KillerWhaleEnemy(spawnPos, onCeiling, playerPos);
        }
            break;
        case 4: 
        {
            spawnPos.x = RM.WINDOW_WIDTH - 300.0f;
            spawnPos.y = RM.WINDOW_HEIGHT - 100.0f;
            newEnemy = new VerticalMedusaEnemy(spawnPos);
        }
            break;
        }

        if (newEnemy != nullptr)
        {
            _enemy = newEnemy;
            _objects.push_back(_enemy);
        }
    }

    void CreateHUD()
    {
        _scoreText = new TextObject("SCORE: 000000", "resources/fonts/arial.ttf");
        _scoreText->GetTransform()->position = Vector2(20.f, 20.f);
        _scoreText->GetTransform()->scale = Vector2(0.5f, 0.5f);
        _scoreText->SetColor({ 255, 215, 0, 255 });
        _ui.push_back(_scoreText);

        _shieldText = new TextObject("SHIELD: 100", "resources/fonts/arial.ttf");
        _shieldText->GetTransform()->position = Vector2(20.f, RM.WINDOW_HEIGHT - 120.f);
        _shieldText->GetTransform()->scale = Vector2(0.5f, 0.5f);
        _shieldText->SetColor({ 0, 255, 255, 255 }); 
        _ui.push_back(_shieldText);

        _cannonAmmoText = new TextObject("CA: 0", "resources/fonts/arial.ttf");
        _cannonAmmoText->GetTransform()->position = Vector2(20.f, RM.WINDOW_HEIGHT - 80.f);
        _cannonAmmoText->GetTransform()->scale = Vector2(0.4f, 0.4f);
        _cannonAmmoText->SetColor({ 255, 128, 0, 255 }); 
        _ui.push_back(_cannonAmmoText);

        _laserAmmoText = new TextObject("LA: 0", "resources/fonts/arial.ttf");
        _laserAmmoText->GetTransform()->position = Vector2(20.f, RM.WINDOW_HEIGHT - 50.f);
        _laserAmmoText->GetTransform()->scale = Vector2(0.4f, 0.4f);
        _laserAmmoText->SetColor({ 255, 0, 255, 255 }); 
        _ui.push_back(_laserAmmoText);

        _powerUpInfoText = new TextObject("NEXT: +1000 SCORE", "resources/fonts/arial.ttf");
        _powerUpInfoText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 200.f, 20.f);
        _powerUpInfoText->GetTransform()->scale = Vector2(0.4f, 0.4f);
        _powerUpInfoText->SetColor({ 100, 255, 100, 255 }); 
        _ui.push_back(_powerUpInfoText);
    }

    void UpdateHUD()
    {
        if (_player == nullptr)
            return;

        std::ostringstream scoreStream;
        scoreStream << "SCORE: " << std::setfill('0') << std::setw(6) << _player->GetScore();
        _scoreText->SetText(scoreStream.str());

        std::ostringstream shieldStream;
        shieldStream << "SHIELD: " << _player->GetShield();
        _shieldText->SetText(shieldStream.str());

        std::ostringstream cannonStream;
        cannonStream << "CA: " << _player->GetCannonAmmo();
        _cannonAmmoText->SetText(cannonStream.str());

        std::ostringstream laserStream;
        laserStream << "LA: " << _player->GetLaserAmmo();
        _laserAmmoText->SetText(laserStream.str());

        std::string powerUpInfo = "NO POWERUP";
        if (!_powerups.empty() && _powerups[0] != nullptr && !_powerups[0]->IsPendingDestroy())
        {
            int index = _powerups[0]->GetCycleIndex();
            powerUpInfo = "POWERUP: " + _powerUpNames[index];
        }
        _powerUpInfoText->SetText(powerUpInfo);
    }

    void SpawnPowerUp(Vector2 position)
    {
        PowerUp* powerup = new PowerUp(
            "resources/powerup.png",
            Vector2(0.f, 0.f),
            Vector2(48.f, 48.f),
            position
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
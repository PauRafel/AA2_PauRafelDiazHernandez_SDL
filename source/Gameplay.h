#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "BubbleEnemy.h"
#include "HorizontalMedusaEnemy.h"
#include "KillerWhaleEnemy.h"
#include "CirclerEnemy.h"
#include "VerticalMedusaEnemy.h"
#include "BeholderEnemy.h"
#include "ChomperEnemy.h"
#include "AmoebaEnemy.h"
#include "BioTitanBoss.h"
#include "Bullet.h"
#include "Background.h"
#include "PowerUp.h"
#include "TextObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include "WaveManager.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

class Gameplay : public Scene
{
private:
    Player* _player = nullptr;
    Background* _background = nullptr;
    std::vector<Enemy*> _enemies;  
    std::vector<Bullet*> _bullets;
    std::vector<PowerUp*> _powerups;

    TextObject* _scoreText = nullptr;
    TextObject* _shieldText = nullptr;
    TextObject* _cannonAmmoText = nullptr;
    TextObject* _laserAmmoText = nullptr;
    TextObject* _powerUpInfoText = nullptr;
    TextObject* _waveInfoText = nullptr;

    bool _powerUpSpawned = false;
    int _powerUpCycleIndex = 0;

    bool _isBossFight = false;
    bool _bossIntroStarted = false;
    bool _bossWaveStarted = false;     
    bool _bossFullyVisible = false;    
    BioTitanBoss* _boss = nullptr;   

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

    float _waveTransitionTimer = 0.0f;
    float _waveTransitionDelay = 2.0f;
    bool _waitingForNextWave = false;

public:
    Gameplay() = default;

    void OnEnter() override
    {
        _background = new Background("resources/background.png");

        _player = new Player("resources/player.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), &_bullets);
        _objects.push_back(_player);

        CreateHUD();

        WAVE_MANAGER.Initialize([this](EnemySpawnData data) {
            this->SpawnEnemy(data);
            });

        WAVE_MANAGER.LoadLevel1Waves();

        WAVE_MANAGER.StartNextWave();

        _powerUpSpawned = false;
        _powerUpCycleIndex = 0;
        _waitingForNextWave = false;
        _waveTransitionTimer = 0.0f;

        srand((unsigned int)time(NULL));
    }

    void OnExit() override
    {
        _player = nullptr;

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

        for (Enemy* enemy : _enemies)
            delete enemy;
        _enemies.clear();

        WAVE_MANAGER.Clear();

        Scene::OnExit();
    }

    void Update(float dt) override
    {
        if (_background != nullptr)
            _background->Update(dt);

        UpdateHUD();

        WAVE_MANAGER.Update(dt);

        if (WAVE_MANAGER.IsCurrentWaveBoss() && !_bossWaveStarted)
        {
            _bossWaveStarted = true;
            std::cout << "=== BOSS WAVE STARTED - Keep scrolling... ===" << std::endl;
        }

        if (_bossWaveStarted && _boss == nullptr && !_enemies.empty())
        {
            for (Enemy* enemy : _enemies)
            {
                BioTitanBoss* potentialBoss = dynamic_cast<BioTitanBoss*>(enemy);
                if (potentialBoss != nullptr)
                {
                    _boss = potentialBoss;
                    std::cout << "Boss found in enemy list!" << std::endl;
                    break;
                }
            }
        }

        if (_boss != nullptr && !_bossFullyVisible && !_isBossFight)
        {
            if (_boss->IsFullyVisible())
            {
                _bossFullyVisible = true;
                _isBossFight = true;

                if (_background != nullptr)
                {
                    _background->StopScrolling();
                    std::cout << "=== BOSS FULLY VISIBLE ===" << std::endl;
                    std::cout << "Background scroll STOPPED!" << std::endl;
                    std::cout << "=== BOSS FIGHT START ===" << std::endl;
                }
            }
        }

        if (_waitingForNextWave)
        {
            _waveTransitionTimer += dt;

            if (_waveTransitionTimer >= _waveTransitionDelay)
            {
                if (!WAVE_MANAGER.AllWavesCompleted())
                {
                    WAVE_MANAGER.StartNextWave();
                    _waitingForNextWave = false;
                    _waveTransitionTimer = 0.0f;
                    _powerUpSpawned = false; 
                }
                else
                {
                    std::cout << "LEVEL COMPLETED!" << std::endl;
                }
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

        for (int i = _enemies.size() - 1; i >= 0; i--)
        {
            Enemy* enemy = _enemies[i];

            if (enemy->IsPendingDestroy())
            {
                if (enemy->HasEscaped())
                {
                    WAVE_MANAGER.OnEnemyEscaped();
                    std::cout << "Enemy escaped off screen!" << std::endl;

                    Wave* currentWave = WAVE_MANAGER.GetCurrentWave();
                    if (currentWave != nullptr && currentWave->IsCompleted() && !_waitingForNextWave)
                    {
                        std::cout << "Wave completed after enemy escaped!" << std::endl;
                        std::cout << "Some enemies escaped - NO PowerUp" << std::endl;

                        _waitingForNextWave = true;
                        _waveTransitionTimer = 0.0f;
                    }
                }

                delete _enemies[i];
                _enemies.erase(_enemies.begin() + i);
            }
            else
            {
                enemy->Update(dt);
            }
        }

        for (Enemy* enemy : _enemies)
        {
            if (enemy->IsPendingDestroy())
                continue;

            for (Bullet* bullet : _bullets)
            {
                if (bullet->IsPendingDestroy() || !bullet->IsPlayerBullet())
                    continue;

                if (bullet->GetRigidBody()->CheckCollision(enemy->GetRigidBody()))
                {
                    bullet->Destroy();

                    Vector2 enemyPos = enemy->GetTransform()->position;
                    int scoreValue = enemy->GetScoreValue();

                    enemy->TakeDamage(1);

                    if (enemy->IsPendingDestroy())
                    {
                        if (_player != nullptr)
                            _player->AddScore(scoreValue);

                        WAVE_MANAGER.OnEnemyKilled();

                        Wave* currentWave = WAVE_MANAGER.GetCurrentWave();
                        if (currentWave != nullptr && currentWave->IsCompleted())
                        {
                            std::cout << "Wave completed!" << std::endl;

                            if (currentWave->AllEnemiesKilled() && !_powerUpSpawned)
                            {
                                std::cout << "All enemies killed! Spawning PowerUp..." << std::endl;
                                SpawnPowerUp(enemyPos);
                                _powerUpSpawned = true;
                            }
                            else if (!currentWave->AllEnemiesKilled())
                            {
                                std::cout << "Some enemies escaped - NO PowerUp" << std::endl;
                            }

                            _waitingForNextWave = true;
                            _waveTransitionTimer = 0.0f;
                        }
                    }

                    break;
                }
            }
        }

        for (PowerUp* powerup : _powerups)
        {
            if (powerup->IsPendingDestroy())
                continue;

            for (Bullet* bullet : _bullets)
            {
                if (bullet->IsPendingDestroy() || !bullet->IsPlayerBullet())
                    continue;

                if (bullet->GetRigidBody()->CheckCollision(powerup->GetRigidBody()))
                {
                    bullet->Destroy();
                    powerup->Hit();
                }
            }
        }

        if (_player != nullptr)
        {
            for (PowerUp* powerup : _powerups)
            {
                if (powerup->IsPendingDestroy())
                    continue;

                if (_player->GetRigidBody()->CheckCollision(powerup->GetRigidBody()))
                {
                    ApplyPowerUpToPlayer(powerup->GetCurrentType());
                    _powerUpCycleIndex = (powerup->GetCycleIndex() + 1) % 8;
                    powerup->Destroy();
                }
            }
        }

        if (_player != nullptr)
        {
            for (Bullet* bullet : _bullets)
            {
                if (bullet->IsPendingDestroy() || bullet->IsPlayerBullet())
                    continue;

                if (bullet->GetRigidBody()->CheckCollision(_player->GetRigidBody()))
                {
                    bullet->Destroy();
                    _player->TakeDamage(10);

                    if (_player->GetShield() <= 0)
                    {
                        std::cout << "PLAYER DIED!" << std::endl;
                    }
                }
            }
        }

        if (_player != nullptr && !_player->IsPendingDestroy())
        {
            _player->Update(dt);
        }

        for (Object* u : _ui)
        {
            if (!u->IsPendingDestroy())
                u->Update(dt);
        }
    }

    void Render() override
    {
        if (_background != nullptr)
            _background->Render();

        for (Enemy* enemy : _enemies)
            enemy->Render();

        Scene::Render();

        for (Bullet* bullet : _bullets)
            bullet->Render();

        for (PowerUp* powerup : _powerups)
            powerup->Render();
    }

private:
    void SpawnEnemy(EnemySpawnData data)
    {
        Enemy* newEnemy = nullptr;

        switch (data.type)
        {
        case ENEMY_BUBBLE:
            newEnemy = new BubbleEnemy(data.spawnPosition);
            break;

        case ENEMY_HORIZONTAL_MEDUSA:
            newEnemy = new HorizontalMedusaEnemy(data.spawnPosition);
            break;

        case ENEMY_KILLER_WHALE:
        {
            Vector2* playerPos = (_player != nullptr) ? &(_player->GetTransform()->position) : nullptr;
            newEnemy = new KillerWhaleEnemy(data.spawnPosition, data.onCeiling, playerPos);
            break;
        }

        case ENEMY_CIRCLER:
            newEnemy = new CirclerEnemy(data.spawnPosition);
            break;

        case ENEMY_VERTICAL_MEDUSA:
            newEnemy = new VerticalMedusaEnemy(data.spawnPosition);
            break;

        case ENEMY_BEHOLDER:
            newEnemy = new BeholderEnemy(data.spawnPosition);
            break;

        case ENEMY_CHOMPER:
            newEnemy = new ChomperEnemy(data.spawnPosition, data.startAngle);
            break;

        case ENEMY_AMOEBA:
            newEnemy = new AmoebaEnemy(data.spawnPosition);
            break;

        case ENEMY_BIO_TITAN_BOSS:
            newEnemy = new BioTitanBoss(data.spawnPosition, &_bullets);
            break;
        }

        if (newEnemy != nullptr)
        {
            _enemies.push_back(newEnemy);
            std::cout << "Enemy spawned at (" << data.spawnPosition.x << ", " << data.spawnPosition.y << ")" << std::endl;
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

        _waveInfoText = new TextObject("WAVE: 1", "resources/fonts/arial.ttf");
        _waveInfoText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH - 200.f, 20.f);
        _waveInfoText->GetTransform()->scale = Vector2(0.5f, 0.5f);
        _waveInfoText->SetColor({ 255, 100, 100, 255 });
        _ui.push_back(_waveInfoText);
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

        std::ostringstream waveStream;
        Wave* currentWave = WAVE_MANAGER.GetCurrentWave();
        if (currentWave != nullptr)
        {
            waveStream << "WAVE: " << currentWave->GetWaveNumber()
                << " (" << currentWave->GetEnemiesKilled() << "/" << currentWave->GetTotalEnemies() << ")";
        }
        else
        {
            waveStream << "WAVE: COMPLETE";
        }
        _waveInfoText->SetText(waveStream.str());
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
#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "TorpedoEnemy.h"
#include "TurboChainsaw.h"
#include "RoboKrabsEnemy.h"
#include "NukeEnemy.h"
#include "MissileEnemy.h"
#include "DanielsEnemy.h"
#include "UfoEnemy.h"
#include "AnnoyerEnemy.h"
#include "AngrygonsEnemy.h"
#include "SpaceBoss.h"
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

class GameplayLevel2 : public Scene
{
private:
    Player* _player = nullptr;
    Background* _background = nullptr;
    std::vector<Enemy*> _enemies;
    std::vector<Bullet*> _bullets;
    std::vector<PowerUp*> _powerups;

    TextObject* _scoreLabel = nullptr;
    TextObject* _scoreValue = nullptr;
    TextObject* _energyLabel = nullptr;
    TextObject* _energyBars = nullptr;
    TextObject* _cannonLabel = nullptr;
    TextObject* _cannonBars = nullptr;
    TextObject* _laserLabel = nullptr;
    TextObject* _laserBars = nullptr;

    TextObject* _powerUpInfoText = nullptr;
    TextObject* _waveInfoText = nullptr;

    bool _powerUpSpawned = false;
    int _powerUpCycleIndex = 0;

    bool _isBossFight = false;
    bool _bossIntroStarted = false;
    bool _bossWaveStarted = false;
    bool _bossFullyVisible = false;
    Enemy* _boss = nullptr;

    PowerUpType _powerUpCycle[9] = {
        POWERUP_SCORE,
        POWERUP_CA1,
        POWERUP_LA1,
        POWERUP_SPEED,
        POWERUP_CA2,
        POWERUP_LA2,
        POWERUP_SHIELD,
        POWERUP_TURRETS,
        POWERUP_FULL_SHIELD
    };

    std::string _powerUpNames[9] = {
        "NEXT: +1000 SCORE",
        "NEXT: CANNONS LV1",
        "NEXT: LASER LV1",
        "NEXT: SPEED BOOST",
        "NEXT: CANNONS LV2",
        "NEXT: LASER LV2",
        "NEXT: SHIELD RESTORE",
        "NEXT: TURRETS",
        "NEXT: FULL_SHIELD"
    };

    float _waveTransitionTimer = 0.0f;
    float _waveTransitionDelay = 2.0f;
    bool _waitingForNextWave = false;

public:
    GameplayLevel2() = default;

    void OnEnter() override
    {
        _background = new Background("resources/background2.png");

        _player = new Player("resources/player.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), &_bullets);
        _objects.push_back(_player);

        CreateHUD();

        WAVE_MANAGER.Initialize([this](EnemySpawnData data) {
            this->SpawnEnemy(data);
            });

        WAVE_MANAGER.LoadLevel2Waves();

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
            std::cout << "=== SPACE BOSS WAVE STARTED - Keep scrolling... ===" << std::endl;
        }

        if (_bossWaveStarted && _boss == nullptr && !_enemies.empty())
        {
            for (Enemy* enemy : _enemies)
            {
                SpaceBoss* potentialBoss = dynamic_cast<SpaceBoss*>(enemy);
                if (potentialBoss != nullptr)
                {
                    _boss = potentialBoss;
                    std::cout << "Space Boss found in enemy list!" << std::endl;
                    break;
                }
            }
        }

        if (_boss != nullptr && !_bossFullyVisible && !_isBossFight)
        {
            SpaceBoss* spaceBoss = dynamic_cast<SpaceBoss*>(_boss);
            if (spaceBoss != nullptr && spaceBoss->IsFullyVisible())
            {
                _bossFullyVisible = true;
                _isBossFight = true;

                if (_background != nullptr)
                {
                    _background->StopScrolling();
                    std::cout << "=== SPACE BOSS FULLY VISIBLE ===" << std::endl;
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
                        SM.SetNextScene("GameOver");
                        return;
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

        for (Bullet* bullet : _bullets)
            bullet->Render();

        for (PowerUp* powerup : _powerups)
            powerup->Render();

        SDL_FRect hudBackground;
        hudBackground.x = 0.f;
        hudBackground.y = RM.WINDOW_HEIGHT - 50.f;
        hudBackground.w = (float)RM.WINDOW_WIDTH;
        hudBackground.h = 50.f;

        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &hudBackground);

        Scene::Render();
    }

private:
    void SpawnEnemy(EnemySpawnData data)
    {
        Enemy* newEnemy = nullptr;

        switch (data.type)
        {
        case ENEMY_TORPEDO:
            newEnemy = new TorpedoEnemy(data.spawnPosition);
            break;

        case ENEMY_TURBO_CHAINSAW:
            newEnemy = new TurboChainsaw(data.spawnPosition);
            break;

        case ENEMY_ROBO_KRABS:
            newEnemy = new RoboKrabsEnemy(data.spawnPosition, data.onCeiling);
            break;

        case ENEMY_NUKE:
            newEnemy = new NukeEnemy(data.spawnPosition);
            break;

        case ENEMY_MISSILE:
            newEnemy = new MissileEnemy(data.spawnPosition);
            break;

        case ENEMY_DANIELS:
            newEnemy = new DanielsEnemy(data.spawnPosition);
            break;

        case ENEMY_UFO:
            newEnemy = new UfoEnemy(data.spawnPosition);
            break;

        case ENEMY_ANNOYER:
            newEnemy = new AnnoyerEnemy(data.spawnPosition);
            break;

        case ENEMY_ANGRYGONS:
            newEnemy = new AngrygonsEnemy(data.spawnPosition);
            break;

        case ENEMY_SPACE_BOSS:
            newEnemy = new SpaceBoss(data.spawnPosition, &_bullets);
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
        float hudY = RM.WINDOW_HEIGHT - 15.f;
        SDL_Color cyanColor = { 0, 255, 255, 255 };
        float textScale = 0.6f;

        _scoreLabel = new TextObject("SC:", "resources/fonts/arial.ttf");
        _scoreLabel->GetTransform()->position = Vector2(40.f, hudY);
        _scoreLabel->GetTransform()->scale = Vector2(textScale, textScale);
        _scoreLabel->SetColor(cyanColor);
        _ui.push_back(_scoreLabel);

        _scoreValue = new TextObject("000000", "resources/fonts/arial.ttf");
        _scoreValue->GetTransform()->position = Vector2(100.f, hudY);
        _scoreValue->GetTransform()->scale = Vector2(textScale, textScale);
        _scoreValue->SetColor(cyanColor);
        _ui.push_back(_scoreValue);

        _energyLabel = new TextObject("EN:", "resources/fonts/arial.ttf");
        _energyLabel->GetTransform()->position = Vector2(270.f, hudY);
        _energyLabel->GetTransform()->scale = Vector2(textScale, textScale);
        _energyLabel->SetColor(cyanColor);
        _ui.push_back(_energyLabel);

        _energyBars = new TextObject("==========", "resources/fonts/arial.ttf");
        _energyBars->GetTransform()->position = Vector2(330.f, hudY);
        _energyBars->GetTransform()->scale = Vector2(textScale, textScale);
        _energyBars->SetColor(cyanColor);
        _ui.push_back(_energyBars);

        _cannonLabel = new TextObject("CA:", "resources/fonts/arial.ttf");
        _cannonLabel->GetTransform()->position = Vector2(600.f, hudY);
        _cannonLabel->GetTransform()->scale = Vector2(textScale, textScale);
        _cannonLabel->SetColor(cyanColor);
        _ui.push_back(_cannonLabel);

        _cannonBars = new TextObject("==========", "resources/fonts/arial.ttf");
        _cannonBars->GetTransform()->position = Vector2(660.f, hudY);
        _cannonBars->GetTransform()->scale = Vector2(textScale, textScale);
        _cannonBars->SetColor(cyanColor);
        _ui.push_back(_cannonBars);

        _laserLabel = new TextObject("LA:", "resources/fonts/arial.ttf");
        _laserLabel->GetTransform()->position = Vector2(930.f, hudY);
        _laserLabel->GetTransform()->scale = Vector2(textScale, textScale);
        _laserLabel->SetColor(cyanColor);
        _ui.push_back(_laserLabel);

        _laserBars = new TextObject("==========", "resources/fonts/arial.ttf");
        _laserBars->GetTransform()->position = Vector2(990.f, hudY);
        _laserBars->GetTransform()->scale = Vector2(textScale, textScale);
        _laserBars->SetColor(cyanColor);
        _ui.push_back(_laserBars);

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
        scoreStream << std::setfill('0') << std::setw(6) << _player->GetScore();
        _scoreValue->SetText(scoreStream.str());

        int shield = _player->GetShield();
        int maxShield = 100;
        int numBars = (shield * 10) / maxShield;
        std::string energyBars = "";
        for (int i = 0; i < numBars; i++)
            energyBars += "=";
        _energyBars->SetText(energyBars);

        int cannonAmmo = _player->GetCannonAmmo();
        int maxCannonAmmo = 100;
        int cannonBars = (cannonAmmo * 10) / maxCannonAmmo;
        std::string cannonBarsStr = "";
        for (int i = 0; i < cannonBars; i++)
            cannonBarsStr += "=";
        _cannonBars->SetText(cannonBarsStr);

        int laserAmmo = _player->GetLaserAmmo();
        int maxLaserAmmo = 60;
        int laserBars = (laserAmmo * 10) / maxLaserAmmo;
        std::string laserBarsStr = "";
        for (int i = 0; i < laserBars; i++)
            laserBarsStr += "=";
        _laserBars->SetText(laserBarsStr);

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
            "resources/powerups/powerup_score.png",
            Vector2(0.f, 0.f),
            Vector2(60.f, 32.f),
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
        case POWERUP_FULL_SHIELD:
            _player->ApplyPowerUp(STATE_FULL_SHIELD);
            break;
        }
    }
};
#pragma once
#include "GameplayBase.h"
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

class GameplayLevel2 : public GameplayBase
{
public:
    GameplayLevel2() = default;
    ~GameplayLevel2() = default;

protected:
    void LoadLevel() override
    {
        WAVE_MANAGER.LoadLevel2Waves();
    }

    void SpawnEnemy(EnemySpawnData data) override
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

    std::string GetNextSceneOnComplete() override
    {
        return "MainMenu"; 
    }

    std::string GetBackgroundPath() override
    {
        return "resources/background2.png";
    }

    bool IsBossFullyVisible() override
    {
        if (_boss != nullptr)
        {
            SpaceBoss* spaceBoss = dynamic_cast<SpaceBoss*>(_boss);
            if (spaceBoss != nullptr)
            {
                return spaceBoss->IsFullyVisible();
            }
        }
        return false;
    }

    void CheckBossConditions(float dt) override
    {
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
            if (IsBossFullyVisible())
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
    }
};
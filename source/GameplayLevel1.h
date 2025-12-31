#pragma once
#include "GameplayBase.h"
#include "BubbleEnemy.h"
#include "HorizontalMedusaEnemy.h"
#include "KillerWhaleEnemy.h"
#include "CirclerEnemy.h"
#include "VerticalMedusaEnemy.h"
#include "BeholderEnemy.h"
#include "ChomperEnemy.h"
#include "AmoebaEnemy.h"
#include "BioTitanBoss.h"

class GameplayLevel1 : public GameplayBase
{
public:
    GameplayLevel1() = default;
    ~GameplayLevel1() = default;

protected:
    void LoadLevel() override
    {
        WAVE_MANAGER.LoadLevel1Waves();
    }

    void SpawnEnemy(EnemySpawnData data) override
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

    std::string GetNextSceneOnComplete() override
    {
        return "Level2Intro";
    }

    std::string GetBackgroundPath() override
    {
        return "resources/background.png";
    }

    bool IsBossFullyVisible() override
    {
        if (_boss != nullptr)
        {
            BioTitanBoss* bioTitanBoss = dynamic_cast<BioTitanBoss*>(_boss);
            if (bioTitanBoss != nullptr)
            {
                return bioTitanBoss->IsFullyVisible();
            }
        }
        return false;
    }

    void CheckBossConditions(float dt) override
    {
        if (WAVE_MANAGER.IsCurrentWaveBoss() && !_bossWaveStarted)
        {
            _bossWaveStarted = true;
            std::cout << "=== BIO TITAN BOSS WAVE STARTED - Keep scrolling... ===" << std::endl;
        }

        if (_bossWaveStarted && _boss == nullptr && !_enemies.empty())
        {
            for (Enemy* enemy : _enemies)
            {
                BioTitanBoss* potentialBoss = dynamic_cast<BioTitanBoss*>(enemy);
                if (potentialBoss != nullptr)
                {
                    _boss = potentialBoss;
                    std::cout << "Bio Titan Boss found in enemy list!" << std::endl;
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
                    std::cout << "=== BIO TITAN BOSS FULLY VISIBLE ===" << std::endl;
                    std::cout << "Background scroll STOPPED!" << std::endl;
                    std::cout << "=== BOSS FIGHT START ===" << std::endl;
                }
            }
        }
    }
};
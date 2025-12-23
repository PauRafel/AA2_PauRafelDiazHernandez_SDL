#pragma once
#include "Enemy.h"
#include "Vector2.h"
#include <vector>
#include <string>
#include <functional>

#define WAVE_MANAGER WaveManager::Instance()

enum EnemyType
{
    ENEMY_BUBBLE,
    ENEMY_HORIZONTAL_MEDUSA,
    ENEMY_KILLER_WHALE,
    ENEMY_CIRCLER,
    ENEMY_VERTICAL_MEDUSA,
    ENEMY_BEHOLDER,
    ENEMY_CHOMPER,
    ENEMY_AMOEBA,
    ENEMY_BIO_TITAN_BOSS,


    ENEMY_TORPEDO,
    ENEMY_TURBO_CHAINSAW,
    ENEMY_ROBO_KRABS,
    ENEMY_NUKE,
    ENEMY_MISSILE,
    ENEMY_DANIELS,
    ENEMY_UFO,
    ENEMY_ANNOYER,
    ENEMY_ANGRYGONS,
    ENEMY_SPACE_BOSS
};

struct EnemySpawnData
{
    EnemyType type;
    Vector2 spawnPosition;
    float spawnDelay; 

    bool onCeiling;       
    float startAngle;    
    Vector2* playerPos;    

    EnemySpawnData()
        : type(ENEMY_BUBBLE),
        spawnPosition(Vector2(0.f, 0.f)),
        spawnDelay(0.f),
        onCeiling(false),
        startAngle(0.f),
        playerPos(nullptr)
    {
    }
};

class Wave
{
private:
    int _waveNumber;
    std::vector<EnemySpawnData> _enemySpawnList;
    int _enemiesSpawned;
    int _enemiesKilled;
    int _enemiesEscaped;  
    float _waveTimer;
    bool _isActive;
    bool _isCompleted;
    bool _allEnemiesKilled;  

public:
    Wave(int waveNumber);

    void AddEnemy(EnemySpawnData enemyData);
    void Start();
    void Update(float dt);
    void OnEnemyKilled();
    void OnEnemyEscaped();  

    bool IsActive() const { return _isActive; }
    bool IsCompleted() const { return _isCompleted; }
    bool AllEnemiesKilled() const { return _allEnemiesKilled; }  
    int GetWaveNumber() const { return _waveNumber; }
    int GetTotalEnemies() const { return _enemySpawnList.size(); }
    int GetEnemiesKilled() const { return _enemiesKilled; }
    int GetEnemiesEscaped() const { return _enemiesEscaped; }  
    float GetWaveTimer() const { return _waveTimer; }

    std::vector<EnemySpawnData> GetEnemiesToSpawn(float currentTime);
};

typedef std::function<void(EnemySpawnData)> SpawnEnemyCallback;

class WaveManager
{
private:
    WaveManager() = default;
    WaveManager(WaveManager&) = delete;
    WaveManager& operator=(const WaveManager&) = delete;
    ~WaveManager();

    std::vector<Wave*> _waves;
    int _currentWaveIndex;
    SpawnEnemyCallback _spawnCallback;

public:
    bool IsCurrentWaveBoss() const
    {
        Wave* currentWave = GetCurrentWave();
        if (currentWave == nullptr)
            return false;

        return currentWave->GetWaveNumber() == 13 || currentWave->GetWaveNumber() == 21;
    }

    bool IsCurrentWaveLevel2Boss() const
    {
        Wave* currentWave = GetCurrentWave();
        if (currentWave == nullptr)
            return false;

        return currentWave->GetWaveNumber() == 21;
    }

    static WaveManager& Instance()
    {
        static WaveManager instance;
        return instance;
    }

    void Initialize(SpawnEnemyCallback spawnCallback);
    void LoadLevel1Waves();
    void LoadLevel2Waves();
    void StartNextWave();
    void Update(float dt);
    void OnEnemyKilled();
    void OnEnemyEscaped(); 
    void Clear();

    bool AllWavesCompleted() const;
    Wave* GetCurrentWave() const;
    int GetCurrentWaveNumber() const;
    int GetTotalWaves() const { return _waves.size(); }
};
#include "WaveManager.h"
#include "RenderManager.h"
#include <iostream>

Wave::Wave(int waveNumber)
    : _waveNumber(waveNumber),
    _enemiesSpawned(0),
    _enemiesKilled(0),
    _enemiesEscaped(0),
    _waveTimer(0.f),
    _isActive(false),
    _isCompleted(false),
    _allEnemiesKilled(false)
{
}

void Wave::AddEnemy(EnemySpawnData enemyData)
{
    _enemySpawnList.push_back(enemyData);
}

void Wave::Start()
{
    _isActive = true;
    _waveTimer = 0.f;
    _enemiesSpawned = 0;
    _enemiesKilled = 0;
    _enemiesEscaped = 0;
    _isCompleted = false;
    _allEnemiesKilled = false;

    std::cout << "Wave " << _waveNumber << " started! Total enemies: " << _enemySpawnList.size() << std::endl;
}

void Wave::Update(float dt)
{
    if (!_isActive || _isCompleted)
        return;

    _waveTimer += dt;
}

void Wave::OnEnemyKilled()
{
    _enemiesKilled++;

    std::cout << "Enemy killed! (" << _enemiesKilled << "/" << _enemySpawnList.size() << ")" << std::endl;

    int totalCompleted = _enemiesKilled + _enemiesEscaped;

    if (totalCompleted >= _enemySpawnList.size())
    {
        _isCompleted = true;
        _isActive = false;

        if (_enemiesEscaped == 0)
        {
            _allEnemiesKilled = true;
            std::cout << "Wave " << _waveNumber << " completed! ALL ENEMIES KILLED - PowerUp spawned!" << std::endl;
        }
        else
        {
            std::cout << "Wave " << _waveNumber << " completed! (" << _enemiesKilled << " killed, " << _enemiesEscaped << " escaped - NO PowerUp)" << std::endl;
        }
    }
}

void Wave::OnEnemyEscaped()
{
    _enemiesEscaped++;

    std::cout << "Enemy escaped! (" << _enemiesEscaped << " escaped)" << std::endl;

    int totalCompleted = _enemiesKilled + _enemiesEscaped;

    if (totalCompleted >= _enemySpawnList.size())
    {
        _isCompleted = true;
        _isActive = false;

        _allEnemiesKilled = false;
        std::cout << "Wave " << _waveNumber << " completed! (" << _enemiesKilled << " killed, " << _enemiesEscaped << " escaped - NO PowerUp)" << std::endl;
    }
}

std::vector<EnemySpawnData> Wave::GetEnemiesToSpawn(float currentTime)
{
    std::vector<EnemySpawnData> toSpawn;

    for (int i = _enemiesSpawned; i < _enemySpawnList.size(); i++)
    {
        if (_enemySpawnList[i].spawnDelay <= currentTime)
        {
            toSpawn.push_back(_enemySpawnList[i]);
            _enemiesSpawned++;
        }
        else
        {
            break;
        }
    }

    return toSpawn;
}

WaveManager::~WaveManager()
{
    Clear();
}

void WaveManager::Initialize(SpawnEnemyCallback spawnCallback)
{
    _spawnCallback = spawnCallback;
    _currentWaveIndex = -1;
    Clear();
}

void WaveManager::LoadLevel1Waves()
{
    Clear();

    std::cout << "Loading Level 1 Waves: SEA OF KARNAUGH..." << std::endl;

    Wave* wave1 = new Wave(1);

    float topY = 150.0f;
    float bottomY = RM.WINDOW_HEIGHT - 150.0f;

    for (int i = 0; i < 4; i++)
    {
        EnemySpawnData bubbleTop;
        bubbleTop.type = ENEMY_BUBBLE;
        bubbleTop.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, topY);
        bubbleTop.spawnDelay = i * 0.3f; 
        wave1->AddEnemy(bubbleTop);

        EnemySpawnData bubbleBottom;
        bubbleBottom.type = ENEMY_BUBBLE;
        bubbleBottom.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, bottomY);
        bubbleBottom.spawnDelay = (i * 0.3f) + 0.3f;
        wave1->AddEnemy(bubbleBottom);
    }

    _waves.push_back(wave1);

    Wave* wave2 = new Wave(2);

    EnemySpawnData whale1;
    whale1.type = ENEMY_KILLER_WHALE;
    whale1.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, 50.f);
    whale1.spawnDelay = 0.f;
    whale1.onCeiling = true;
    wave2->AddEnemy(whale1);

    EnemySpawnData whale2;
    whale2.type = ENEMY_KILLER_WHALE;
    whale2.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, RM.WINDOW_HEIGHT - 50.f);
    whale2.spawnDelay = 2.0f;
    whale2.onCeiling = false;
    wave2->AddEnemy(whale2);

    _waves.push_back(wave2);

    Wave* wave3 = new Wave(3);

    float medusaPositions[8] = {
        200.f,
        380.f,
        560.f,
        320.f,
        200.f,
        300.f,
        260.f,
        520.f
    };

    for (int i = 0; i < 8; i++)
    {
        EnemySpawnData medusa;
        medusa.type = ENEMY_HORIZONTAL_MEDUSA;
        medusa.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, medusaPositions[i]);
        medusa.spawnDelay = i * 0.3f;
        wave3->AddEnemy(medusa);
    }

    _waves.push_back(wave3);

    Wave* wave4 = new Wave(4);

    EnemySpawnData circler;
    circler.type = ENEMY_CIRCLER;
    circler.spawnPosition = Vector2(RM.WINDOW_WIDTH / 2.f, RM.WINDOW_HEIGHT / 2.f);
    circler.spawnDelay = 0.f;
    wave4->AddEnemy(circler);

    _waves.push_back(wave4);

    Wave* wave5 = new Wave(5);

    float verticalMedusaPositionsX[8] = {
        200.f,
        400.f,
        600.f,
        800.f,
        1000.f,
        300.f,
        700.f,
        900.f
    };

    for (int i = 0; i < 8; i++)
    {
        EnemySpawnData vMedusa;
        vMedusa.type = ENEMY_VERTICAL_MEDUSA;
        vMedusa.spawnPosition = Vector2(verticalMedusaPositionsX[i], RM.WINDOW_HEIGHT - 80.f);
        vMedusa.spawnDelay = i * 0.5f;
        wave5->AddEnemy(vMedusa);
    }

    _waves.push_back(wave5);

    Wave* wave6 = new Wave(6);

    float cornerOffset = 100.f;
    float pairSpacing = 80.f;

    for (int i = 0; i < 2; i++)
    {
        EnemySpawnData beholder;
        beholder.type = ENEMY_BEHOLDER;
        beholder.spawnPosition = Vector2(cornerOffset + (i * pairSpacing), cornerOffset);
        beholder.spawnDelay = i * 0.3f;
        wave6->AddEnemy(beholder);
    }

    for (int i = 0; i < 2; i++)
    {
        EnemySpawnData beholder;
        beholder.type = ENEMY_BEHOLDER;
        beholder.spawnPosition = Vector2(RM.WINDOW_WIDTH - cornerOffset - (i * pairSpacing), cornerOffset);
        beholder.spawnDelay = 0.8f + (i * 0.3f);
        wave6->AddEnemy(beholder);
    }

    for (int i = 0; i < 2; i++)
    {
        EnemySpawnData beholder;
        beholder.type = ENEMY_BEHOLDER;
        beholder.spawnPosition = Vector2(cornerOffset + (i * pairSpacing), RM.WINDOW_HEIGHT - cornerOffset);
        beholder.spawnDelay = 1.6f + (i * 0.3f);
        wave6->AddEnemy(beholder);
    }

    for (int i = 0; i < 2; i++)
    {
        EnemySpawnData beholder;
        beholder.type = ENEMY_BEHOLDER;
        beholder.spawnPosition = Vector2(RM.WINDOW_WIDTH - cornerOffset - (i * pairSpacing), RM.WINDOW_HEIGHT - cornerOffset);
        beholder.spawnDelay = 2.4f + (i * 0.3f);
        wave6->AddEnemy(beholder);
    }

    _waves.push_back(wave6);

    Wave* wave7 = new Wave(7);

    EnemySpawnData whale7_1;
    whale7_1.type = ENEMY_KILLER_WHALE;
    whale7_1.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, 50.f);
    whale7_1.spawnDelay = 0.f;
    whale7_1.onCeiling = true;
    wave7->AddEnemy(whale7_1);

    EnemySpawnData whale7_2;
    whale7_2.type = ENEMY_KILLER_WHALE;
    whale7_2.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, RM.WINDOW_HEIGHT - 50.f);
    whale7_2.spawnDelay = 2.0f;
    whale7_2.onCeiling = false;
    wave7->AddEnemy(whale7_2);

    _waves.push_back(wave7);

    Wave* wave8 = new Wave(8);

    float chomperSpacing = 90.0f;
    float chomperStartY = 50.0f;

    for (int i = 0; i < 8; i++)
    {
        EnemySpawnData chomper;
        chomper.type = ENEMY_CHOMPER;
        chomper.spawnPosition = Vector2(RM.WINDOW_WIDTH + 50.f, chomperStartY + (i * chomperSpacing));
        chomper.spawnDelay = i * 0.2f;
        chomper.startAngle = (i * 3.14159f / 4.0f); 
        wave8->AddEnemy(chomper);
    }

    _waves.push_back(wave8);

    Wave* wave9 = new Wave(9);

    EnemySpawnData amoeba;
    amoeba.type = ENEMY_AMOEBA;
    amoeba.spawnPosition = Vector2(-100.f, RM.WINDOW_HEIGHT / 2.f);
    amoeba.spawnDelay = 0.f;
    wave9->AddEnemy(amoeba);

    _waves.push_back(wave9);

    Wave* wave10 = new Wave(10);

    for (int i = 0; i < 4; i++)
    {
        EnemySpawnData bubbleTop;
        bubbleTop.type = ENEMY_BUBBLE;
        bubbleTop.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, topY);
        bubbleTop.spawnDelay = i * 0.3f;
        wave10->AddEnemy(bubbleTop);

        EnemySpawnData bubbleBottom;
        bubbleBottom.type = ENEMY_BUBBLE;
        bubbleBottom.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, bottomY);
        bubbleBottom.spawnDelay = (i * 0.3f) + 0.3f;
        wave10->AddEnemy(bubbleBottom);
    }

    _waves.push_back(wave10);

    Wave* wave11 = new Wave(11);

    EnemySpawnData whale11_1;
    whale11_1.type = ENEMY_KILLER_WHALE;
    whale11_1.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, 50.f);
    whale11_1.spawnDelay = 0.f;
    whale11_1.onCeiling = true;
    wave11->AddEnemy(whale11_1);

    EnemySpawnData whale11_2;
    whale11_2.type = ENEMY_KILLER_WHALE;
    whale11_2.spawnPosition = Vector2(RM.WINDOW_WIDTH - 100.f, RM.WINDOW_HEIGHT - 50.f);
    whale11_2.spawnDelay = 2.0f;
    whale11_2.onCeiling = false;
    wave11->AddEnemy(whale11_2);

    _waves.push_back(wave11);

    Wave* wave12 = new Wave(12);

    for (int i = 0; i < 8; i++)
    {
        EnemySpawnData medusa;
        medusa.type = ENEMY_HORIZONTAL_MEDUSA;
        medusa.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, medusaPositions[i]);
        medusa.spawnDelay = i * 0.3f;
        wave12->AddEnemy(medusa);
    }

    _waves.push_back(wave12);

    Wave* waveBoss = new Wave(13);

    EnemySpawnData boss;
    boss.type = ENEMY_BIO_TITAN_BOSS;
    boss.spawnPosition = Vector2(RM.WINDOW_WIDTH - 300.f, RM.WINDOW_HEIGHT / 2.f);
    boss.spawnDelay = 0.f;
    waveBoss->AddEnemy(boss);

    _waves.push_back(waveBoss);

    std::cout << "Loaded " << _waves.size() << " waves for Level 1: SEA OF KARNAUGH" << std::endl;
}

void WaveManager::StartNextWave()
{
    _currentWaveIndex++;

    if (_currentWaveIndex >= _waves.size())
    {
        std::cout << "No more waves to start!" << std::endl;
        return;
    }

    _waves[_currentWaveIndex]->Start();
}

void WaveManager::Update(float dt)
{
    if (_currentWaveIndex < 0 || _currentWaveIndex >= _waves.size())
        return;

    Wave* currentWave = _waves[_currentWaveIndex];
    currentWave->Update(dt);

    std::vector<EnemySpawnData> toSpawn = currentWave->GetEnemiesToSpawn(currentWave->GetWaveTimer());

    for (EnemySpawnData& data : toSpawn)
    {
        if (_spawnCallback)
        {
            _spawnCallback(data);
        }
    }
}

void WaveManager::OnEnemyKilled()
{
    if (_currentWaveIndex < 0 || _currentWaveIndex >= _waves.size())
        return;

    _waves[_currentWaveIndex]->OnEnemyKilled();
}

void WaveManager::OnEnemyEscaped()
{
    if (_currentWaveIndex < 0 || _currentWaveIndex >= _waves.size())
        return;

    _waves[_currentWaveIndex]->OnEnemyEscaped();
}

void WaveManager::Clear()
{
    for (Wave* wave : _waves)
        delete wave;

    _waves.clear();
    _currentWaveIndex = -1;
}

bool WaveManager::AllWavesCompleted() const
{
    if (_waves.empty())
        return false;

    return _currentWaveIndex >= _waves.size() - 1 &&
        _waves[_currentWaveIndex]->IsCompleted();
}

Wave* WaveManager::GetCurrentWave() const
{
    if (_currentWaveIndex < 0 || _currentWaveIndex >= _waves.size())
        return nullptr;

    return _waves[_currentWaveIndex];
}

int WaveManager::GetCurrentWaveNumber() const
{
    Wave* current = GetCurrentWave();
    return current ? current->GetWaveNumber() : 0;
}
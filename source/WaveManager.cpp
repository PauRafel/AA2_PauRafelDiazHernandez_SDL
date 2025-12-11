#include "WaveManager.h"
#include "RenderManager.h"
#include <iostream>

Wave::Wave(int waveNumber)
    : _waveNumber(waveNumber),
    _enemiesSpawned(0),
    _enemiesKilled(0),
    _waveTimer(0.f),
    _isActive(false),
    _isCompleted(false)
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
    _isCompleted = false;

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

    if (_enemiesKilled >= _enemySpawnList.size())
    {
        _isCompleted = true;
        _isActive = false;
        std::cout << "Wave " << _waveNumber << " completed!" << std::endl;
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

    std::cout << "Loading Level 1 Waves..." << std::endl;

    Wave* wave1 = new Wave(1);

    EnemySpawnData bubble1;
    bubble1.type = ENEMY_BUBBLE;
    bubble1.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, 150.f);
    bubble1.spawnDelay = 0.f;
    wave1->AddEnemy(bubble1);

    EnemySpawnData bubble2;
    bubble2.type = ENEMY_BUBBLE;
    bubble2.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, RM.WINDOW_HEIGHT / 2.f);
    bubble2.spawnDelay = 1.5f;
    wave1->AddEnemy(bubble2);

    EnemySpawnData bubble3;
    bubble3.type = ENEMY_BUBBLE;
    bubble3.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, RM.WINDOW_HEIGHT - 150.f);
    bubble3.spawnDelay = 3.0f;
    wave1->AddEnemy(bubble3);

    _waves.push_back(wave1);

    Wave* wave2 = new Wave(2);

    for (int i = 0; i < 4; i++)
    {
        EnemySpawnData medusa;
        medusa.type = ENEMY_HORIZONTAL_MEDUSA;
        medusa.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, 100.f + i * 200.f);
        medusa.spawnDelay = i * 1.0f;
        wave2->AddEnemy(medusa);
    }

    _waves.push_back(wave2);

    Wave* wave3 = new Wave(3);

    EnemySpawnData whale1;
    whale1.type = ENEMY_KILLER_WHALE;
    whale1.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, 50.f);
    whale1.spawnDelay = 0.f;
    whale1.onCeiling = true;
    wave3->AddEnemy(whale1);

    EnemySpawnData whale2;
    whale2.type = ENEMY_KILLER_WHALE;
    whale2.spawnPosition = Vector2(RM.WINDOW_WIDTH - 50.f, RM.WINDOW_HEIGHT - 50.f);
    whale2.spawnDelay = 2.0f;
    whale2.onCeiling = false;
    wave3->AddEnemy(whale2);

    _waves.push_back(wave3);

    Wave* wave4 = new Wave(4);

    EnemySpawnData circler;
    circler.type = ENEMY_CIRCLER;
    circler.spawnPosition = Vector2(RM.WINDOW_WIDTH / 2.f, RM.WINDOW_HEIGHT / 2.f);
    circler.spawnDelay = 0.f;
    wave4->AddEnemy(circler);

    _waves.push_back(wave4);

    Wave* waveBoss = new Wave(5);

    EnemySpawnData boss;
    boss.type = ENEMY_BIO_TITAN_BOSS;
    boss.spawnPosition = Vector2(RM.WINDOW_WIDTH - 300.f, RM.WINDOW_HEIGHT / 2.f);
    boss.spawnDelay = 0.f;
    waveBoss->AddEnemy(boss);

    _waves.push_back(waveBoss);

    std::cout << "Loaded " << _waves.size() << " waves for Level 1" << std::endl;
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
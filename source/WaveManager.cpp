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
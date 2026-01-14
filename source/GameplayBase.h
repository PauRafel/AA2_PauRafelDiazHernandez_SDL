#pragma once
#include "Scene.h"
#include "GameplayState.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "PowerUp.h"
#include "TextObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include "WaveManager.h"
#include "Button.h"
#include "BackgroundDecorations.h"
#include <vector>
#include <string>

class GameplayBase : public Scene
{
protected:
    GameplayState _currentState;

    float _deathTimer;
    float _deathAnimationDuration;
    float _blackScreenDuration;
    bool _showingDeathAnimation;
    bool _showingBlackScreen;

    int _extraLives;
    int _lastWaveIndex; 

    Player* _player;
    Background* _background;
    BackgroundDecorations* _backgroundDecorations;
    std::vector<Enemy*> _enemies;
    std::vector<Bullet*> _bullets;
    std::vector<PowerUp*> _powerups;

    TextObject* _scoreLabel;
    TextObject* _scoreValue;
    TextObject* _energyLabel;
    TextObject* _energyBars;
    TextObject* _cannonLabel;
    TextObject* _cannonBars;
    TextObject* _laserLabel;
    TextObject* _laserBars;
    TextObject* _powerUpInfoText;
    TextObject* _waveInfoText;

    TextObject* _pauseText;
    TextObject* _pauseInstructions;
    Button* _pauseBackButton;

    TextObject* _finishStageText;
    TextObject* _finishStageInstructions;

    bool _powerUpSpawned;
    int _powerUpCycleIndex;
    PowerUpType _powerUpCycle[9];
    std::string _powerUpNames[9];

    bool _isBossFight;
    bool _bossIntroStarted;
    bool _bossWaveStarted;
    bool _bossFullyVisible;
    Enemy* _boss;

    float _waveTransitionTimer;
    float _waveTransitionDelay;
    bool _waitingForNextWave;

public:
    GameplayBase();
    virtual ~GameplayBase();

    void OnEnter() override;
    void OnExit() override;
    void Update(float dt) override;
    void Render() override;

protected:
    virtual void LoadLevel() = 0;
    virtual void SpawnEnemy(EnemySpawnData data) = 0;
    virtual std::string GetNextSceneOnComplete() = 0;
    virtual std::string GetBackgroundPath() = 0;
    virtual int GetCurrentLevel() = 0;
    virtual void CheckBossConditions(float dt);
    virtual bool IsBossFullyVisible();

    void ChangeState(GameplayState newState);
    void UpdateGameplayState(float dt);
    void UpdatePausedState(float dt);
    void UpdateFinishStageState(float dt);
    void UpdateDeathState(float dt);

    void CreateHUD();
    void UpdateHUD();
    void CreatePauseUI();
    void ShowPauseUI(bool show);
    void CreateFinishStageUI();
    void ShowFinishStageUI(bool show);

    void SpawnPowerUp(Vector2 position);
    void ApplyPowerUpToPlayer(PowerUpType type);

    void HandleBulletUpdates(float dt);
    void HandlePowerUpUpdates(float dt);
    void HandleEnemyUpdates(float dt);
    void HandleCollisions();

    void HandlePlayerDeath();
    void RespawnPlayer();
    void RestartFromLastWave();
};
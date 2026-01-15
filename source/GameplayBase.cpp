#include "GameplayBase.h"
#include "NameInputScene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <iostream>

GameplayBase::GameplayBase()
{
    _currentState = GAMEPLAY_STATE_GAMEPLAY;

    _player = nullptr;
    _background = nullptr;
    _backgroundDecorations = nullptr;
    _boss = nullptr;

    _scoreLabel = nullptr;
    _scoreValue = nullptr;
    _energyLabel = nullptr;
    _energyBars = nullptr;
    _cannonLabel = nullptr;
    _cannonBars = nullptr;
    _laserLabel = nullptr;
    _laserBars = nullptr;
    _powerUpInfoText = nullptr;
    _waveInfoText = nullptr;

    _pauseText = nullptr;
    _pauseInstructions = nullptr;
    _pauseBackButton = nullptr;

    _finishStageText = nullptr;
    _finishStageInstructions = nullptr;

    _powerUpSpawned = false;
    _powerUpCycleIndex = 0;

    _powerUpCycle[0] = POWERUP_SCORE;
    _powerUpCycle[1] = POWERUP_CA1;
    _powerUpCycle[2] = POWERUP_LA1;
    _powerUpCycle[3] = POWERUP_SPEED;
    _powerUpCycle[4] = POWERUP_CA2;
    _powerUpCycle[5] = POWERUP_LA2;
    _powerUpCycle[6] = POWERUP_SHIELD;
    _powerUpCycle[7] = POWERUP_TURRETS;
    _powerUpCycle[8] = POWERUP_FULL_SHIELD;

    _powerUpNames[0] = "NEXT: +1000 SCORE";
    _powerUpNames[1] = "NEXT: CANNONS LV1";
    _powerUpNames[2] = "NEXT: LASER LV1";
    _powerUpNames[3] = "NEXT: SPEED BOOST";
    _powerUpNames[4] = "NEXT: CANNONS LV2";
    _powerUpNames[5] = "NEXT: LASER LV2";
    _powerUpNames[6] = "NEXT: SHIELD RESTORE";
    _powerUpNames[7] = "NEXT: TURRETS";
    _powerUpNames[8] = "NEXT: FULL_SHIELD";

    _isBossFight = false;
    _bossIntroStarted = false;
    _bossWaveStarted = false;
    _bossFullyVisible = false;

    _waveTransitionTimer = 0.0f;
    _waveTransitionDelay = 2.0f;
    _waitingForNextWave = false;

    _deathTimer = 0.0f;
    _deathAnimationDuration = 1.0f;
    _blackScreenDuration = 2.0f;
    _showingDeathAnimation = false;
    _showingBlackScreen = false;

    _extraLives = 3; 
    _lastWaveIndex = 0;
}

GameplayBase::~GameplayBase()
{
}

void GameplayBase::OnEnter()
{
    _currentState = GAMEPLAY_STATE_GAMEPLAY;

    _background = new Background(GetBackgroundPath());
    _backgroundDecorations = new BackgroundDecorations(GetCurrentLevel());

    _player = new Player("resources/player.png", Vector2(0.f, 0.f), Vector2(64.f, 64.f), &_bullets);
    _objects.push_back(_player);

    CreateHUD();
    CreatePauseUI();
    CreateFinishStageUI();

    ShowPauseUI(false);
    ShowFinishStageUI(false);

    WAVE_MANAGER.Initialize([this](EnemySpawnData data) {
        this->SpawnEnemy(data);
        });

    LoadLevel();

    WAVE_MANAGER.StartNextWave();

    _powerUpSpawned = false;
    _powerUpCycleIndex = 0;
    _waitingForNextWave = false;
    _waveTransitionTimer = 0.0f;
    _isBossFight = false;
    _bossIntroStarted = false;
    _bossWaveStarted = false;
    _bossFullyVisible = false;
    _boss = nullptr;

    _extraLives = 3;
    _lastWaveIndex = 0;

    srand((unsigned int)time(NULL));
}

void GameplayBase::OnExit()
{
    _player = nullptr;

    if (_background != nullptr)
    {
        delete _background;
        _background = nullptr;
    }

    if (_backgroundDecorations != nullptr)
    {
        delete _backgroundDecorations;
        _backgroundDecorations = nullptr;
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

void GameplayBase::Update(float dt)
{
    if (_background != nullptr && _currentState != GAMEPLAY_STATE_PAUSED)
        _background->Update(dt);

    if (_backgroundDecorations != nullptr && _currentState != GAMEPLAY_STATE_PAUSED)
        _backgroundDecorations->Update(dt);

    switch (_currentState)
    {
    case GAMEPLAY_STATE_GAMEPLAY:
        UpdateGameplayState(dt);
        break;

    case GAMEPLAY_STATE_PAUSED:
        UpdatePausedState(dt);
        break;

    case GAMEPLAY_STATE_FINISH_STAGE:
        UpdateFinishStageState(dt);
        break;

    case GAMEPLAY_STATE_DEATH:
        UpdateDeathState(dt);
        break;
    }

    for (Object* u : _ui)
    {
        if (!u->IsPendingDestroy())
            u->Update(dt);
    }
}

void GameplayBase::UpdateGameplayState(float dt)
{
    if (IM.GetEvent(SDLK_ESCAPE, DOWN) || IM.GetEvent(SDLK_P, DOWN))
    {
        ChangeState(GAMEPLAY_STATE_PAUSED);
        return;
    }

    UpdateHUD();

    WAVE_MANAGER.Update(dt);

    CheckBossConditions(dt);

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

                _lastWaveIndex = WAVE_MANAGER.GetCurrentWaveNumber() - 1;
            }
            else
            {
                std::cout << "LEVEL COMPLETED!" << std::endl;
            }
        }
    }

    HandleBulletUpdates(dt);
    HandlePowerUpUpdates(dt);
    HandleEnemyUpdates(dt);

    HandleCollisions();

    if (_player != nullptr && !_player->IsPendingDestroy())
    {
        _player->Update(dt);
    }
}

void GameplayBase::UpdatePausedState(float dt)
{
    if (IM.GetEvent(SDLK_ESCAPE, DOWN) || IM.GetEvent(SDLK_P, DOWN))
    {
        ChangeState(GAMEPLAY_STATE_GAMEPLAY);
        return;
    }
}

void GameplayBase::UpdateFinishStageState(float dt)
{
    if (IM.GetEvent(SDLK_RETURN, DOWN))
    {
        if (!WAVE_MANAGER.AllWavesCompleted())
        {
            ChangeState(GAMEPLAY_STATE_GAMEPLAY);
            WAVE_MANAGER.StartNextWave();
            _waitingForNextWave = false;
            _waveTransitionTimer = 0.0f;
            _powerUpSpawned = false;

            _lastWaveIndex = WAVE_MANAGER.GetCurrentWaveNumber() - 1;
        }
        else
        {
            int finalScore = _player->GetScore();

            if (RANKING.IsTopScore(finalScore))
            {
                std::cout << "Level completed! Score qualifies for top 10!" << std::endl;
                NameInputScene::SetPendingScore(finalScore);
                SM.SetNextScene("NameInput");
            }
            else
            {
                std::cout << "Level completed! Moving to next level..." << std::endl;
                SM.SetNextScene(GetNextSceneOnComplete());
            }
        }
    }
}

void GameplayBase::UpdateDeathState(float dt)
{
    _deathTimer += dt;

    if (_showingDeathAnimation)
    {
        if (_deathTimer >= _deathAnimationDuration)
        {
            _showingDeathAnimation = false;
            _showingBlackScreen = true;
            _deathTimer = 0.0f;

            for (Bullet* bullet : _bullets)
                delete bullet;
            _bullets.clear();

            for (PowerUp* powerup : _powerups)
                delete powerup;
            _powerups.clear();

            for (Enemy* enemy : _enemies)
                delete enemy;
            _enemies.clear();
        }
    }
    else if (_showingBlackScreen)
    {
        if (_deathTimer >= _blackScreenDuration)
        {
            _showingBlackScreen = false;
            _deathTimer = 0.0f;

            if (_extraLives > 0)
            {
                std::cout << "Respawning player! Extra lives left: " << _extraLives << std::endl;
                RespawnPlayer();
                RestartFromLastWave();
                ChangeState(GAMEPLAY_STATE_GAMEPLAY);
            }
            else
            {
                std::cout << "GAME OVER - No extra lives left!" << std::endl;
                HandlePlayerDeath();
            }
        }
    }
}

void GameplayBase::ChangeState(GameplayState newState)
{
    switch (_currentState)
    {
    case GAMEPLAY_STATE_PAUSED:
        ShowPauseUI(false);
        break;

    case GAMEPLAY_STATE_FINISH_STAGE:
        ShowFinishStageUI(false);
        break;
    }

    _currentState = newState;

    switch (_currentState)
    {
    case GAMEPLAY_STATE_GAMEPLAY:
        std::cout << "=== STATE: GAMEPLAY ===" << std::endl;
        break;

    case GAMEPLAY_STATE_PAUSED:
        std::cout << "=== STATE: PAUSED ===" << std::endl;
        ShowPauseUI(true);
        break;

    case GAMEPLAY_STATE_FINISH_STAGE:
        std::cout << "=== STATE: FINISH_STAGE ===" << std::endl;
        ShowFinishStageUI(true);
        break;

    case GAMEPLAY_STATE_DEATH:
        std::cout << "=== STATE: DEATH ===" << std::endl;
        _deathTimer = 0.0f;
        _showingDeathAnimation = true;
        _showingBlackScreen = false;
        _extraLives--;
        std::cout << "Extra lives remaining: " << _extraLives << std::endl;
        break;
    }
}

void GameplayBase::Render()
{
    if (_background != nullptr)
        _background->Render();

    if (_backgroundDecorations != nullptr)
        _backgroundDecorations->Render();

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

    if (_currentState == GAMEPLAY_STATE_DEATH)
    {
        if (_showingBlackScreen)
        {
            SDL_FRect blackScreen;
            blackScreen.x = 0.0f;
            blackScreen.y = 0.0f;
            blackScreen.w = (float)RM.WINDOW_WIDTH;
            blackScreen.h = (float)RM.WINDOW_HEIGHT;

            SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
            SDL_RenderFillRect(RM.GetRenderer(), &blackScreen);
        }
    }
}

void GameplayBase::CheckBossConditions(float dt)
{
    if (WAVE_MANAGER.IsCurrentWaveBoss() && !_bossWaveStarted)
    {
        _bossWaveStarted = true;
        std::cout << "=== BOSS WAVE STARTED - Keep scrolling... ===" << std::endl;
    }

    if (_bossWaveStarted && _boss == nullptr && !_enemies.empty())
    {
        for (Enemy* enemy : _enemies)
        {
            if (IsBossFullyVisible())
            {
                _boss = enemy;
                std::cout << "Boss found in enemy list!" << std::endl;
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

                if (_backgroundDecorations != nullptr) {
                    _backgroundDecorations->StopScrolling();
                }

                std::cout << "=== BOSS FULLY VISIBLE ===" << std::endl;
                std::cout << "Background scroll STOPPED!" << std::endl;
                std::cout << "=== BOSS FIGHT START ===" << std::endl;
            }
        }
    }
}

bool GameplayBase::IsBossFullyVisible()
{
    if (_boss != nullptr)
    {
        return _boss->GetTransform()->position.x <= RM.WINDOW_WIDTH - 200.f;
    }
    return false;
}

void GameplayBase::CreateHUD()
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

void GameplayBase::UpdateHUD()
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

void GameplayBase::CreatePauseUI()
{
    _pauseText = new TextObject("PAUSED", "resources/fonts/arial.ttf");
    _pauseText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.1f - 100.f, RM.WINDOW_HEIGHT / 2.0f - 100.f);
    _pauseText->GetTransform()->scale = Vector2(2.0f, 2.0f);
    _pauseText->SetColor({ 255, 255, 0, 255 });
    _ui.push_back(_pauseText);

    _pauseInstructions = new TextObject("Press ESC or P to resume", "resources/fonts/arial.ttf");
    _pauseInstructions->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.1f - 200.f, RM.WINDOW_HEIGHT / 2.0f - 20.f);
    _pauseInstructions->GetTransform()->scale = Vector2(0.8f, 0.8f);
    _pauseInstructions->SetColor({ 200, 200, 200, 255 });
    _ui.push_back(_pauseInstructions);

    _pauseBackButton = new Button(
        "MAIN MENU",
        "resources/fonts/arial.ttf",
        Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f + 80.f),
        [this]() {
            std::cout << "Returning to Main Menu from pause..." << std::endl;
            SM.SetNextScene("MainMenu");
        }
    );
    dynamic_cast<TextRenderer*>(_pauseBackButton->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
    _ui.push_back(_pauseBackButton);
}

void GameplayBase::ShowPauseUI(bool show)
{
    if (_pauseText != nullptr)
    {
        _pauseText->GetTransform()->scale = show ? Vector2(2.0f, 2.0f) : Vector2(0.0f, 0.0f);
    }

    if (_pauseInstructions != nullptr)
    {
        _pauseInstructions->GetTransform()->scale = show ? Vector2(0.8f, 0.8f) : Vector2(0.0f, 0.0f);
    }

    if (_pauseBackButton != nullptr)
    {
        _pauseBackButton->GetTransform()->scale = show ? Vector2(1.0f, 1.0f) : Vector2(0.0f, 0.0f);

        if (!show)
        {
            _pauseBackButton->GetTransform()->position = Vector2(-1000.f, -1000.f);
        }
        else
        {
            _pauseBackButton->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f, RM.WINDOW_HEIGHT / 2.0f + 80.f);
        }
    }
}

void GameplayBase::CreateFinishStageUI()
{
    _finishStageText = new TextObject("WAVE COMPLETED!", "resources/fonts/arial.ttf");
    _finishStageText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.4f - 200.f, RM.WINDOW_HEIGHT / 2.0f - 50.f);
    _finishStageText->GetTransform()->scale = Vector2(1.5f, 1.5f);
    _finishStageText->SetColor({ 0, 255, 0, 255 });
    _ui.push_back(_finishStageText);

    _finishStageInstructions = new TextObject("Press ENTER to continue", "resources/fonts/arial.ttf");
    _finishStageInstructions->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 1.9f - 250.f, RM.WINDOW_HEIGHT / 2.0f + 50.f);
    _finishStageInstructions->GetTransform()->scale = Vector2(0.8f, 0.8f);
    _finishStageInstructions->SetColor({ 200, 200, 200, 255 });
    _ui.push_back(_finishStageInstructions);
}

void GameplayBase::ShowFinishStageUI(bool show)
{
    if (_finishStageText != nullptr)
    {
        _finishStageText->GetTransform()->scale = show ? Vector2(1.5f, 1.5f) : Vector2(0.0f, 0.0f);
    }

    if (_finishStageInstructions != nullptr)
    {
        _finishStageInstructions->GetTransform()->scale = show ? Vector2(0.8f, 0.8f) : Vector2(0.0f, 0.0f);
    }
}

void GameplayBase::HandleBulletUpdates(float dt)
{
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
}

void GameplayBase::HandlePowerUpUpdates(float dt)
{
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
}
void GameplayBase::HandleEnemyUpdates(float dt)
{
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

                    ChangeState(GAMEPLAY_STATE_FINISH_STAGE);
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
}

void GameplayBase::HandleCollisions()
{
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

                        if (_boss != nullptr && enemy == _boss)
                        {
                            std::cout << "=== BOSS DEFEATED! ===" << std::endl;
                            SM.SetNextScene(GetNextSceneOnComplete());
                            return;
                        }

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

                        ChangeState(GAMEPLAY_STATE_FINISH_STAGE);
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
                    ChangeState(GAMEPLAY_STATE_DEATH);
                    return;
                }
            }
        }
    }

    if (_player != nullptr && !_player->IsInvulnerable())
    {
        for (Enemy* enemy : _enemies)
        {
            if (enemy->IsPendingDestroy())
                continue;

            if (_player->GetRigidBody()->CheckCollision(enemy->GetRigidBody()))
            {
                _player->TakeDamageFromEnemy(10);

                if (_player->GetShield() <= 0)
                {
                    std::cout << "Player died from enemy collision!" << std::endl;
                    ChangeState(GAMEPLAY_STATE_DEATH);
                    return;
                }
            }
        }
    }
}

void GameplayBase::HandlePlayerDeath()
{
    std::cout << "GAME OVER - PLAYER DIED!" << std::endl;
    int finalScore = _player->GetScore();

    if (RANKING.IsTopScore(finalScore))
    {
        std::cout << "Score qualifies for top 10! Going to name input..." << std::endl;
        NameInputScene::SetPendingScore(finalScore);
        SM.SetNextScene("NameInput");
    }
    else
    {
        std::cout << "Score doesn't qualify for top 10. Game Over." << std::endl;
        SM.SetNextScene("GameOver");
    }
}
void GameplayBase::RespawnPlayer()
{
    if (_player != nullptr)
    {
        _player->GetTransform()->position = Vector2(200.f, RM.WINDOW_HEIGHT / 2.0f);
        _player->ApplyPowerUp(STATE_FULL_SHIELD);

        _player->GetRigidBody()->SetVelocity(Vector2(0.f, 0.f));

        std::cout << "Player respawned at starting position!" << std::endl;
    }
}
void GameplayBase::RestartFromLastWave()
{
    for (Enemy* enemy : _enemies)
        delete enemy;
    _enemies.clear();
    for (Bullet* bullet : _bullets)
        delete bullet;
    _bullets.clear();

    for (PowerUp* powerup : _powerups)
        delete powerup;
    _powerups.clear();

    WAVE_MANAGER.Clear();
    LoadLevel();

    for (int i = 0; i < _lastWaveIndex; i++)
    {
        WAVE_MANAGER.StartNextWave();
        Wave* wave = WAVE_MANAGER.GetCurrentWave();
        if (wave != nullptr)
        {
            for (int j = 0; j < wave->GetTotalEnemies(); j++)
            {
                wave->OnEnemyKilled();
            }
        }
    }

    WAVE_MANAGER.StartNextWave();

    _waitingForNextWave = false;
    _waveTransitionTimer = 0.0f;
    _powerUpSpawned = false;

    std::cout << "Restarting from wave " << (_lastWaveIndex + 1) << std::endl;
}
void GameplayBase::SpawnPowerUp(Vector2 position)
{
    PowerUp* powerup = new PowerUp(
        "resources/powerups/powerup_score.png",
        Vector2(0.f, 0.f),
        Vector2(60.f, 32.f),
        position
    );
    _powerups.push_back(powerup);
}
void GameplayBase::ApplyPowerUpToPlayer(PowerUpType type)
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
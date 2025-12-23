#include "Game.h"
#include "MainMenu.h"
#include "Gameplay.h"
#include "IntroSequence.h"
#include "GameOverScreen.h"
#include <cassert>
#include <iostream>

void Game::Init()
{
    RM.Init();

    RM.LoadFont("resources/fonts/arial.ttf", 48);


    RM.LoadTexture("resources/intro/splash_title.png");
    RM.LoadTexture("resources/intro/splash_art.png");
    RM.LoadTexture("resources/intro/panel_settings.png");
    RM.LoadTexture("resources/intro/panel_scoreboard.png");
    RM.LoadTexture("resources/intro/panel_powerups.png");
    RM.LoadTexture("resources/intro/splash_level1.png");

    RM.LoadTexture("resources/gameover_splash.png");

    RM.LoadTexture("resources/player.png");                    
    RM.LoadTexture("resources/player_cannon.png");              
    RM.LoadTexture("resources/player_laser.png");               
    RM.LoadTexture("resources/player_turrets.png");             
    RM.LoadTexture("resources/player_cannon_laser.png");        
    RM.LoadTexture("resources/player_cannon_turrets.png");       
    RM.LoadTexture("resources/player_laser_turrets.png");       
    RM.LoadTexture("resources/player_full.png"); 

    RM.LoadTexture("resources/bullet.png");

    RM.LoadTexture("resources/shield_effect.png");

    RM.LoadTexture("resources/enemy.png");
    RM.LoadTexture("resources/background.png");


    RM.LoadTexture("resources/powerup.png");
    RM.LoadTexture("resources/powerups/powerup_score.png");
    RM.LoadTexture("resources/powerups/powerup_ca1.png");
    RM.LoadTexture("resources/powerups/powerup_la1.png");
    RM.LoadTexture("resources/powerups/powerup_speed.png");
    RM.LoadTexture("resources/powerups/powerup_ca2.png");
    RM.LoadTexture("resources/powerups/powerup_la2.png");
    RM.LoadTexture("resources/powerups/powerup_shield.png");
    RM.LoadTexture("resources/powerups/powerup_turrets.png");
    RM.LoadTexture("resources/powerups/powerup_full_shield.png");


    RM.LoadTexture("resources/BubbleEnemy.png");
    RM.LoadTexture("resources/KillerWhaleEnemy.png");
    RM.LoadTexture("resources/HorizontalMedusaEnemy.png");
    RM.LoadTexture("resources/VerticalMedusaEnemy.png");
    RM.LoadTexture("resources/CirclerEnemy_Head.png");
    RM.LoadTexture("resources/CirclerEnemy_Body.png");
    RM.LoadTexture("resources/BeholderEnemy.png");
    RM.LoadTexture("resources/ChomperEnemy.png");
    RM.LoadTexture("resources/AmoebaEnemy.png");
    RM.LoadTexture("resources/BioTitanBoss_Bullet.png");
    RM.LoadTexture("resources/BioTitanBossEnemy.png");
    

    assert(SM.AddScene("IntroSequence", new IntroSequence()));
    assert(SM.AddScene("GameOver", new GameOverScreen()));
    assert(SM.AddScene("MainMenu", new MainMenu()));
    assert(SM.AddScene("Gameplay", new Gameplay()));

    assert(SM.InitFirstScene("IntroSequence"));

    _isRunning = true;

    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::HandleEvents()
{
    _isRunning = !IM.Listen();
}

void Game::Update()
{
    SM.UpdateCurrentScene(TM.GetDeltaTime());
}

void Game::Render()
{
    RM.ClearScreen();
    SM.GetCurrentScene()->Render();
    RM.RenderScreen();
}

void Game::Release()
{
    RM.Release();
    std::cout << "Game released successfully!" << std::endl;
}
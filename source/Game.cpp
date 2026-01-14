#include "Game.h"
#include "SplashScreen.h"
#include "MainMenu.h"
#include "LevelSelector.h"
#include "RankingScene.h"
#include "GameplayBase.h"
#include "LevelIntroScene.h"  
#include "GameplayLevel1.h"  
#include "GameplayLevel2.h"  
#include "GameOverScreen.h" 
#include "NameInputScene.h"
#include <cassert>
#include <iostream>

void Game::Init()
{
    RM.Init();

    RM.LoadFont("resources/fonts/arial.ttf", 48);


    RM.LoadTexture("resources/intro/splash_title.png");
    RM.LoadTexture("resources/intro/splash_art.png");


    RM.LoadTexture("resources/intro/splash_level1.png");
    RM.LoadTexture("resources/intro/splash_level2.png");
    RM.LoadTexture("resources/gameover_splash.png");


    RM.LoadTexture("resources/player.png");                    
    RM.LoadTexture("resources/player_cannon.png");              
    RM.LoadTexture("resources/player_laser.png");               
    RM.LoadTexture("resources/player_turrets.png");             
    RM.LoadTexture("resources/player_cannon_laser.png");        
    RM.LoadTexture("resources/player_cannon_turrets.png");       
    RM.LoadTexture("resources/player_laser_turrets.png");       
    RM.LoadTexture("resources/player_full.png"); 
    RM.LoadTexture("resources/shield_effect.png");


    RM.LoadTexture("resources/bullet.png");


    RM.LoadTexture("resources/background.png");
    RM.LoadTexture("resources/background2.png");


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

    RM.LoadTexture("resources/TorpedoEnemy.png");             
    RM.LoadTexture("resources/TurboChainsawEnemy.png");       
    RM.LoadTexture("resources/RoboKrabsEnemy.png");          
    RM.LoadTexture("resources/NukeEnemy.png");                
    RM.LoadTexture("resources/MissileEnemy.png");          
    RM.LoadTexture("resources/DanielsEnemy.png");           
    RM.LoadTexture("resources/UfoEnemy.png");                 
    RM.LoadTexture("resources/AnnoyerEnemy.png");             
    RM.LoadTexture("resources/AngrygonsEnemy.png");           
    RM.LoadTexture("resources/SpaceBossEnemy.png");          
    RM.LoadTexture("resources/SpaceBoss_Bullet.png");

    RM.LoadTexture("resources/background_decos/deco_1_lvl1.png");
    RM.LoadTexture("resources/background_decos/deco_2_lvl1.png");
    RM.LoadTexture("resources/background_decos/deco_3_lvl1.png");
    RM.LoadTexture("resources/background_decos/deco_4_lvl1.png");
    RM.LoadTexture("resources/background_decos/deco_5_lvl1.png");
    RM.LoadTexture("resources/background_decos/deco_1_lvl2.png");
    RM.LoadTexture("resources/background_decos/deco_2_lvl2.png");
    RM.LoadTexture("resources/background_decos/deco_3_lvl2.png");
    RM.LoadTexture("resources/background_decos/deco_4_lvl2.png");
    RM.LoadTexture("resources/background_decos/deco_5_lvl2.png");
    

    assert(SM.AddScene("SplashScreen", new SplashScreen()));
    assert(SM.AddScene("MainMenu", new MainMenu()));
    assert(SM.AddScene("LevelSelector", new LevelSelector()));
    assert(SM.AddScene("Ranking", new RankingScene()));

    LevelIntroScene* level1Intro = new LevelIntroScene();
    level1Intro->Configure("resources/intro/splash_level1.png", "GameplayLevel1");
    assert(SM.AddScene("Level1Intro", level1Intro));

    LevelIntroScene* level2Intro = new LevelIntroScene();
    level2Intro->Configure("resources/intro/splash_level2.png", "GameplayLevel2");
    assert(SM.AddScene("Level2Intro", level2Intro));

    assert(SM.AddScene("GameplayLevel1", new GameplayLevel1()));
    assert(SM.AddScene("GameplayLevel2", new GameplayLevel2()));  
    assert(SM.AddScene("GameOver", new GameOverScreen()));
    assert(SM.AddScene("NameInput", new NameInputScene()));

    assert(SM.InitFirstScene("SplashScreen"));

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
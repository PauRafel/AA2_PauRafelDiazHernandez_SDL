#include "Game.h"
#include "MainMenu.h"
#include "Gameplay.h"
#include <cassert>
#include <iostream>

void Game::Init()
{
    RM.Init();

    RM.LoadFont("resources/fonts/arial.ttf", 48);
    RM.LoadTexture("resources/player.png");
    RM.LoadTexture("resources/bullet.png");
    RM.LoadTexture("resources/enemy.png");
    RM.LoadTexture("resources/background.png");
    RM.LoadTexture("resources/powerup.png"); 

    assert(SM.AddScene("MainMenu", new MainMenu()));
    assert(SM.AddScene("Gameplay", new Gameplay()));

    assert(SM.InitFirstScene("MainMenu"));

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
#pragma once
#include "RenderManager.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"

class Game
{
public:
    Game() = default;

    void Init();
    void HandleEvents();
    void Update();
    void Render();
    void Release();

    bool IsRunning() const { return _isRunning; }

private:
    bool _isRunning = false;
};
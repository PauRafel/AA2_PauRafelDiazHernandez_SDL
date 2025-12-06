#pragma once
#include "Scene.h"
#include "InputManager.h"
#include "SceneManager.h"

class MainMenu : public Scene
{
public:
    MainMenu() = default;

    void OnEnter() override
    {}

    void OnExit() override
    {
        Scene::OnExit();
    }

    void Update(float dt) override
    {
        if (IM.GetEvent(SDLK_SPACE, DOWN))
        {
            SM.SetNextScene("Gameplay");
        }

        Scene::Update(dt);
    }

    void Render() override
    {
        Scene::Render();
    }
};
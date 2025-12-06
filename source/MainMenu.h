#pragma once
#include "Scene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "SimpleTextObject.h"
#include "RenderManager.h"

class MainMenu : public Scene
{
public:
    MainMenu() = default;

    void OnEnter() override
    {
        SimpleTextObject* menuText = new SimpleTextObject(
            "MENU",
            Vector2(RM.WINDOW_WIDTH / 2.0f - 200.f, RM.WINDOW_HEIGHT / 2.0f - 60.f),
            SDL_Color{ 255, 255, 255, 255 },
            2.0f
        );
        _ui.push_back(menuText);

        SimpleTextObject* instructionText = new SimpleTextObject(
            "PRESS SPACE",
            Vector2(RM.WINDOW_WIDTH / 2.0f - 300.f, RM.WINDOW_HEIGHT / 2.0f + 100.f),
            SDL_Color{ 200, 200, 200, 255 },
            1.0f
        );
        _ui.push_back(instructionText);
    }

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
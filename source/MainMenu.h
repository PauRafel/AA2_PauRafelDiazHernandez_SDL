#pragma once
#include "Scene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "TextObject.h"
#include "RenderManager.h"

class MainMenu : public Scene
{
public:
    MainMenu() = default;

    void OnEnter() override
    {
        TextObject* menuText = new TextObject("MENU", "resources/fonts/arial.ttf");
        menuText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 100.f, RM.WINDOW_HEIGHT / 2.0f - 100.f);
        menuText->GetTransform()->scale = Vector2(2.0f, 2.0f);
        menuText->SetColor({ 255, 255, 0, 255 });
        _ui.push_back(menuText);

        TextObject* instructionText = new TextObject("Press SPACE to start", "resources/fonts/arial.ttf");
        instructionText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 200.f, RM.WINDOW_HEIGHT / 2.0f + 50.f);
        instructionText->GetTransform()->scale = Vector2(1.0f, 1.0f);
        instructionText->SetColor({ 200, 200, 200, 255 });
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
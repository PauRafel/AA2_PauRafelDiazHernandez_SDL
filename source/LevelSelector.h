#pragma once
#include "Scene.h"
#include "Button.h"
#include "TextObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include <iostream>

class LevelSelector : public Scene
{
private:
    TextObject* _titleText;
    Button* _level1Button;
    Button* _level2Button;
    Button* _level3Button;
    Button* _backButton;

public:
    LevelSelector() = default;

    void OnEnter() override
    {
        _titleText = new TextObject("SELECT LEVEL", "resources/fonts/arial.ttf");
        _titleText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.4f - 120.f, 120.f);
        _titleText->GetTransform()->scale = Vector2(1.5f, 1.5f);
        _titleText->SetColor({ 255, 255, 0, 255 }); 
        _ui.push_back(_titleText);

        float centerX = RM.WINDOW_WIDTH / 2.0f;  
        float startY = 280.f;
        float buttonSpacing = 100.f;

        _level1Button = new Button(
            "LEVEL 1",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY),
            [this]() {
                std::cout << "Level 1 selected!" << std::endl;
                SM.SetNextScene("Gameplay"); 
            }
        );
        dynamic_cast<TextRenderer*>(_level1Button->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
        _ui.push_back(_level1Button);

        _level2Button = new Button(
            "LEVEL 2",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing),
            [this]() {
                std::cout << "Level 2 selected!" << std::endl;
                SM.SetNextScene("GameplayLevel2"); 
            }
        );
        dynamic_cast<TextRenderer*>(_level2Button->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
        _ui.push_back(_level2Button);

        _level3Button = new Button(
            "LEVEL 3",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing * 2),
            [this]() {
                std::cout << "Level 3 selected!" << std::endl;
                //SM.SetNextScene("GameplayLevel3");
            }
        );
        dynamic_cast<TextRenderer*>(_level3Button->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
        _ui.push_back(_level3Button);

        _backButton = new Button(
            "MAIN MENU",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing * 3 + 50.f), 
            [this]() {
                std::cout << "Back to Main Menu!" << std::endl;
                SM.SetNextScene("MainMenu");
            }
        );
        dynamic_cast<TextRenderer*>(_backButton->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
        _ui.push_back(_backButton);

        std::cout << "LevelSelector scene loaded!" << std::endl;
    }

    void OnExit() override
    {
        _titleText = nullptr;
        _level1Button = nullptr;
        _level2Button = nullptr;
        _level3Button = nullptr;
        _backButton = nullptr;

        Scene::OnExit();
    }

    void Update(float dt) override
    {
        Scene::Update(dt);
    }

    void Render() override
    {
        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(RM.GetRenderer());

        Scene::Render();
    }
};
#pragma once
#include "Scene.h"
#include "Button.h"
#include "TextObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "AudioManager.h"
#include <iostream>

class MainMenu : public Scene
{
private:
    TextObject* _titleText;
    Button* _playButton;
    Button* _rankingButton;
    Button* _audioButton;
    Button* _exitButton;

    bool _audioEnabled;

public:
    MainMenu() : _audioEnabled(true) {}

    void OnEnter() override
    {
        _titleText = new TextObject("MAIN MENU", "resources/fonts/arial.ttf");
        _titleText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.1f - 120.f, 120.f);
        _titleText->GetTransform()->scale = Vector2(1.5f, 1.5f);
        _titleText->SetColor({ 255, 255, 0, 255 }); 
        _ui.push_back(_titleText);

        float centerX = RM.WINDOW_WIDTH / 2.0f;  
        float startY = 280.f;
        float buttonSpacing = 100.f;

        _playButton = new Button(
            "PLAY",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY),
            [this]() {
                std::cout << "Play button clicked!" << std::endl;
                SM.SetNextScene("LevelSelector");
            }
        );
        _ui.push_back(_playButton);

        _rankingButton = new Button(
            "RANKING",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing),
            [this]() {
                std::cout << "Ranking button clicked!" << std::endl;
                //SM.SetNextScene("Ranking");
            }
        );
        _ui.push_back(_rankingButton);

        _audioButton = new Button(
            _audioEnabled ? "AUDIO" : "AUDIO",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing * 2),
            [this]() {
                ToggleAudio();
            }
        );
        _ui.push_back(_audioButton);

        _exitButton = new Button(
            "EXIT",
            "resources/fonts/arial.ttf",
            Vector2(centerX, startY + buttonSpacing * 3),
            [this]() {
                std::cout << "Exit button clicked! Closing game..." << std::endl;
                SDL_Event quitEvent;
                quitEvent.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quitEvent);
            }
        );
        _ui.push_back(_exitButton);

        std::cout << "MainMenu scene loaded!" << std::endl;
    }

    void OnExit() override
    {
        _titleText = nullptr;
        _playButton = nullptr;
        _rankingButton = nullptr;
        _audioButton = nullptr;
        _exitButton = nullptr;

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

private:
    void ToggleAudio()
    {
        _audioEnabled = !_audioEnabled;

        if (_audioEnabled)
        {
            std::cout << "Audio ENABLED" << std::endl;
            _audioButton->SetText("AUDIO ON");
        }
        else
        {
            std::cout << "Audio DISABLED" << std::endl;
            _audioButton->SetText("AUDIO OFF");
        }
    }
};
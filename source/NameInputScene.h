#pragma once
#include "Scene.h"
#include "TextObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "RankingManager.h"
#include <iostream>
#include <string>

class NameInputScene : public Scene
{
private:
    static int _pendingScore;

    TextObject* _titleText;
    TextObject* _instructionText;
    TextObject* _nameText;
    TextObject* _cursorText;

    std::string _playerName;
    int _playerScore;
    const int MAX_NAME_LENGTH = 8;

    float _cursorBlinkTimer;
    float _cursorBlinkSpeed;
    bool _cursorVisible;

public:
    static void SetPendingScore(int score)
    {
        _pendingScore = score;
    }
    NameInputScene() : _playerScore(0) {}

    void OnEnter() override
    {
        _playerName = "";
        _cursorBlinkTimer = 0.0f;
        _cursorBlinkSpeed = 0.5f;
        _cursorVisible = true;

        _playerScore = _pendingScore;
        _pendingScore = 0;

        _titleText = new TextObject("YOU HAVE QUALIFIED!", "resources/fonts/arial.ttf");
        _titleText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.8f - 180.f, 150.f);
        _titleText->GetTransform()->scale = Vector2(1.5f, 1.5f);
        _titleText->SetColor({ 255, 255, 0, 255 }); 
        _ui.push_back(_titleText);

        _instructionText = new TextObject("Type your name and press ENTER", "resources/fonts/arial.ttf");
        _instructionText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.4f - 200.f, 250.f);
        _instructionText->GetTransform()->scale = Vector2(0.8f, 0.8f);
        _instructionText->SetColor({ 200, 200, 200, 255 });
        _ui.push_back(_instructionText);

        _nameText = new TextObject("_", "resources/fonts/arial.ttf");
        _nameText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 150.f, 440.f);
        _nameText->GetTransform()->scale = Vector2(1.5f, 1.5f);
        _nameText->SetColor({ 0, 255, 255, 255 }); 
        _ui.push_back(_nameText);

        _cursorText = new TextObject(" ", "resources/fonts/arial.ttf");
        _cursorText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 150.f, 440.f);
        _cursorText->GetTransform()->scale = Vector2(0.0f, 0.0f);
        _cursorText->SetColor({ 0, 255, 255, 255 }); 
        _ui.push_back(_cursorText);

        std::cout << "NameInputScene loaded! Player score: " << _playerScore << std::endl;
    }

    void OnExit() override
    {
        _titleText = nullptr;
        _instructionText = nullptr;
        _nameText = nullptr;
        _cursorText = nullptr;

        Scene::OnExit();
    }

    void Update(float dt) override
    {
        _cursorBlinkTimer += dt;
        if (_cursorBlinkTimer >= _cursorBlinkSpeed)
        {
            _cursorVisible = !_cursorVisible;
            _cursorBlinkTimer = 0.0f;
        }

        if (_cursorVisible)
            _cursorText->SetText("_");
        else
            _cursorText->SetText(" ");

        HandleTextInput();

        float nameWidth = _playerName.length() * 35.0f; 
        _cursorText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 150.f + nameWidth, 400.f);

        Scene::Update(dt);
    }

    void Render() override
    {
        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(RM.GetRenderer());

        SDL_FRect nameBox;
        nameBox.x = RM.WINDOW_WIDTH / 2.0f - 300.f;
        nameBox.y = 370.f;
        nameBox.w = 600.f;
        nameBox.h = 80.f;

        SDL_SetRenderDrawColor(RM.GetRenderer(), 40, 40, 40, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &nameBox);

        SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 255, 255, 255);
        SDL_RenderRect(RM.GetRenderer(), &nameBox);

        Scene::Render();
    }

    void SetPlayerScore(int score)
    {
        _playerScore = score;
    }

private:
    void HandleTextInput()
    {
        if (IM.GetEvent(SDLK_RETURN, DOWN))
        {
            if (_playerName.length() > 0)
            {
                SubmitName();
            }
        }

        if (IM.GetEvent(SDLK_BACKSPACE, DOWN))
        {
            if (_playerName.length() > 0)
            {
                _playerName.pop_back();
                UpdateNameDisplay();
            }
        }

        if (IM.GetEvent(SDLK_SPACE, DOWN))
        {
            if (_playerName.length() < MAX_NAME_LENGTH)
            {
                _playerName += " ";
                UpdateNameDisplay();
            }
        }

        for (int key = SDLK_A; key <= SDLK_Z; key++)
        {
            if (IM.GetEvent((SDL_Keycode)key, DOWN))
            {
                if (_playerName.length() < MAX_NAME_LENGTH)
                {
                    char letter = 'A' + (key - SDLK_A);
                    _playerName += letter;
                    UpdateNameDisplay();
                }
            }
        }

        for (int key = SDLK_0; key <= SDLK_9; key++)
        {
            if (IM.GetEvent((SDL_Keycode)key, DOWN))
            {
                if (_playerName.length() < MAX_NAME_LENGTH)
                {
                    char number = '0' + (key - SDLK_0);
                    _playerName += number;
                    UpdateNameDisplay();
                }
            }
        }
    }

    void UpdateNameDisplay()
    {
        if (_playerName.length() > 0)
            _nameText->SetText(_playerName);
        else
            _nameText->SetText(" ");
    }

    void SubmitName()
    {
        std::cout << "Name submitted: " << _playerName << " with score: " << _playerScore << std::endl;

        RANKING.AddScore(_playerName, _playerScore);
        SM.SetNextScene("Ranking");
    }
};

int NameInputScene::_pendingScore = 0;
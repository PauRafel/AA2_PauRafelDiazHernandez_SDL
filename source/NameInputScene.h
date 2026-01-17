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
    static void SetPendingScore(int score);

    NameInputScene();

    void OnEnter() override;
    void OnExit() override;
    void Update(float dt) override;
    void Render() override;
    void SetPlayerScore(int score);

private:
    void HandleTextInput();
    void UpdateNameDisplay();
    void SubmitName();
};
#pragma once
#include "Scene.h"
#include "Button.h"
#include "TextObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "RankingManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>

class RankingScene : public Scene
{
private:
    TextObject* _titleText;
    std::vector<TextObject*> _nameTexts;
    std::vector<TextObject*> _scoreTexts;
    Button* _backButton;

public:
    RankingScene() = default;

    void OnEnter() override
    {
        _titleText = new TextObject("TOP 10 RANKING", "resources/fonts/arial.ttf");
        _titleText->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.3f - 180.f, 80.f);
        _titleText->GetTransform()->scale = Vector2(1.5f, 1.5f);
        _titleText->SetColor({ 255, 255, 0, 255 }); 
        _ui.push_back(_titleText);

        std::vector<RankingEntry> topScores = RANKING.GetTopScores();

        float startY = 210.f;
        float lineSpacing = 45.f;

        float nameColumnX = RM.WINDOW_WIDTH / 2.0f - 260.f;
        float scoreColumnX = RM.WINDOW_WIDTH / 2.0f + 140.f;

        for (int i = 0; i < 10; i++)
        {
            std::string nameText;
            std::string scoreText;

            if (i < topScores.size())
            {
                nameText = topScores[i].GetName();
                scoreText = std::to_string(topScores[i].score);
            }
            else
            {
                nameText = "....";
                scoreText = "0";
            }

            TextObject* name = new TextObject(nameText, "resources/fonts/arial.ttf");
            TextObject* score = new TextObject(scoreText, "resources/fonts/arial.ttf");

            name->GetTransform()->position = Vector2(nameColumnX, startY + i * lineSpacing);
            score->GetTransform()->position = Vector2(scoreColumnX, startY + i * lineSpacing);

            name->GetTransform()->scale = Vector2(0.9f, 0.9f);
            score->GetTransform()->scale = Vector2(0.9f, 0.9f);

            SDL_Color color;
            if (i == 0)      color = { 255, 215, 0, 255 };
            else if (i == 1) color = { 192, 192, 192, 255 };
            else if (i == 2) color = { 205, 127, 50, 255 };
            else             color = { 200, 200, 200, 255 };

            name->SetColor(color);
            score->SetColor(color);

            _nameTexts.push_back(name);
            _scoreTexts.push_back(score);

            _ui.push_back(name);
            _ui.push_back(score);
        }

        float centerX = RM.WINDOW_WIDTH / 2.0f;
        _backButton = new Button(
            "MAIN MENU",
            "resources/fonts/arial.ttf",
            Vector2(centerX, 670.f),
            [this]() {
                std::cout << "Back to Main Menu!" << std::endl;
                SM.SetNextScene("MainMenu");
            }
        );
        dynamic_cast<TextRenderer*>(_backButton->GetRenderer())->SetRenderOffset(Vector2(-100.f, 20.f));
        _ui.push_back(_backButton);

        std::cout << "RankingScene loaded!" << std::endl;
        RANKING.PrintRanking(); 
    }

    void OnExit() override
    {
        _titleText = nullptr;
        _nameTexts.clear();
        _scoreTexts.clear();
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

        SDL_FRect rankingBox;
        rankingBox.x = RM.WINDOW_WIDTH / 2.0f - 320.f;
        rankingBox.y = 150.f;
        rankingBox.w = 640.f;
        rankingBox.h = 480.f;

        SDL_SetRenderDrawColor(RM.GetRenderer(), 20, 20, 30, 255);
        SDL_RenderFillRect(RM.GetRenderer(), &rankingBox);

        SDL_SetRenderDrawColor(RM.GetRenderer(), 255, 215, 0, 255); 
        SDL_RenderRect(RM.GetRenderer(), &rankingBox);

        Scene::Render();
    }
};
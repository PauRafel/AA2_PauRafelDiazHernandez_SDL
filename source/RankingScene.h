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
    std::vector<TextObject*> _rankingTexts;
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

        for (int i = 0; i < 10; i++)
        {
            std::ostringstream oss;

            if (i < topScores.size())
            {
                oss << std::right << std::setw(15) << topScores[i].GetName();
                oss << std::left << std::setw(6) << topScores[i].score;
            }
            else
            {
                oss << std::left << std::setw(6) << "000000";
                oss << std::right << std::setw(15) << "------";
            }

            TextObject* rankingLine = new TextObject(oss.str(), "resources/fonts/arial.ttf");
            rankingLine->GetTransform()->position = Vector2(RM.WINDOW_WIDTH / 2.0f - 250.f, startY + (i * lineSpacing));
            rankingLine->GetTransform()->scale = Vector2(1.2f, 1.2f);

            if (i == 0)
                rankingLine->SetColor({ 255, 215, 0, 255 }); 
            else if (i == 1)
                rankingLine->SetColor({ 192, 192, 192, 255 });
            else if (i == 2)
                rankingLine->SetColor({ 205, 127, 50, 255 }); 
            else
                rankingLine->SetColor({ 200, 200, 200, 255 }); 

            _rankingTexts.push_back(rankingLine);
            _ui.push_back(rankingLine);
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
        _rankingTexts.clear();
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
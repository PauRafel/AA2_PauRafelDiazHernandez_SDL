#pragma once
#include "Scene.h"
#include "ImageObject.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"

class GameOverScreen : public Scene
{
private:
    ImageObject* _gameOverImage;

public:
    GameOverScreen() = default;

    void OnEnter() override
    {
        _gameOverImage = new ImageObject(
            "resources/gameover_splash.png",
            Vector2(0.0f, 0.0f),
            Vector2((float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT)
        );

        _gameOverImage->GetTransform()->position = Vector2(
            RM.WINDOW_WIDTH / 2.0f,
            RM.WINDOW_HEIGHT / 2.0f
        );
        _gameOverImage->GetTransform()->size = Vector2(
            (float)RM.WINDOW_WIDTH,
            (float)RM.WINDOW_HEIGHT
        );
        _gameOverImage->GetTransform()->scale = Vector2(1.0f, 1.0f);

        _objects.push_back(_gameOverImage);
    }

    void OnExit() override
    {
        _gameOverImage = nullptr;
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
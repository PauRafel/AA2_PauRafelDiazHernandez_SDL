#pragma once
#include "Scene.h"
#include "ImageObject.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include <string>

class LevelIntroScene : public Scene
{
private:
    ImageObject* _splashImage;
    std::string _splashTexturePath;
    std::string _nextSceneName;

    float _timer;
    float _duration;
    float _alpha;
    float _fadeSpeed;
    bool _isFadingIn;
    bool _isFadingOut;

public:
    LevelIntroScene() = default;

    void Configure(std::string splashTexturePath, std::string nextSceneName, float duration = 3.0f)
    {
        _splashTexturePath = splashTexturePath;
        _nextSceneName = nextSceneName;
        _duration = duration;
    }

    void OnEnter() override
    {
        _timer = 0.0f;
        _alpha = 0.0f;
        _fadeSpeed = 2.0f;
        _isFadingIn = true;
        _isFadingOut = false;

        _splashImage = new ImageObject(
            _splashTexturePath,
            Vector2(0.0f, 0.0f),
            Vector2((float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT)
        );

        _splashImage->GetTransform()->position = Vector2(
            RM.WINDOW_WIDTH / 2.0f,
            RM.WINDOW_HEIGHT / 2.0f
        );
        _splashImage->GetTransform()->size = Vector2(
            (float)RM.WINDOW_WIDTH,
            (float)RM.WINDOW_HEIGHT
        );
        _splashImage->GetTransform()->scale = Vector2(1.0f, 1.0f);

        _objects.push_back(_splashImage);
    }

    void OnExit() override
    {
        _splashImage = nullptr;
        Scene::OnExit();
    }

    void Update(float dt) override
    {
        _timer += dt;

        if (_isFadingIn)
        {
            _alpha += _fadeSpeed * dt;
            if (_alpha >= 1.0f)
            {
                _alpha = 1.0f;
                _isFadingIn = false;
            }
        }
        else if (_isFadingOut)
        {
            _alpha -= _fadeSpeed * dt;
            if (_alpha <= 0.0f)
            {
                _alpha = 0.0f;
                SM.SetNextScene(_nextSceneName);
            }
        }
        else if (_timer >= _duration)
        {
            _isFadingOut = true;
        }

        Scene::Update(dt);
    }

    void Render() override
    {
        Scene::Render();

        if (_alpha < 1.0f)
        {
            SDL_FRect fadeRect;
            fadeRect.x = 0.0f;
            fadeRect.y = 0.0f;
            fadeRect.w = (float)RM.WINDOW_WIDTH;
            fadeRect.h = (float)RM.WINDOW_HEIGHT;

            Uint8 fadeAlpha = (Uint8)((1.0f - _alpha) * 255);
            SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, fadeAlpha);
            SDL_RenderFillRect(RM.GetRenderer(), &fadeRect);
        }
    }
};
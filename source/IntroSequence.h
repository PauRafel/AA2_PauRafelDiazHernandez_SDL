#pragma once
#include "Scene.h"
#include "ImageObject.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include <vector>

class IntroSequence : public Scene
{
private:
    struct IntroScreen
    {
        std::string texturePath;
        float duration;
        ImageObject* imageObject;
        float alpha;

        IntroScreen(std::string path, float dur)
            : texturePath(path), duration(dur), imageObject(nullptr), alpha(0.0f) {
        }
    };

    std::vector<IntroScreen> _screens;
    int _currentScreenIndex;
    float _screenTimer;
    float _fadeSpeed;
    bool _isFadingIn;
    bool _isFadingOut;

public:
    IntroSequence() = default;

    void OnEnter() override
    {
        _currentScreenIndex = 0;
        _screenTimer = 0.0f;
        _fadeSpeed = 2.0f; 
        _isFadingIn = true;
        _isFadingOut = false;

        _screens.push_back(IntroScreen("resources/intro/splash_title.png", 3.0f));
        _screens.push_back(IntroScreen("resources/intro/splash_art.png", 5.0f));
        _screens.push_back(IntroScreen("resources/intro/panel_settings.png", 5.0f));
        _screens.push_back(IntroScreen("resources/intro/panel_scoreboard.png", 5.0f));
        _screens.push_back(IntroScreen("resources/intro/panel_powerups.png", 5.0f));
        _screens.push_back(IntroScreen("resources/intro/splash_level1.png", 5.0f));

        LoadCurrentScreen();
    }

    void OnExit() override
    {
        for (IntroScreen& screen : _screens)
        {
            if (screen.imageObject != nullptr)
            {
                delete screen.imageObject;
                screen.imageObject = nullptr;
            }
        }
        _screens.clear();

        Scene::OnExit();
    }

    void Update(float dt) override
    {
        if (_currentScreenIndex >= _screens.size())
        {
            SM.SetNextScene("Gameplay");
            return;
        }

        IntroScreen& currentScreen = _screens[_currentScreenIndex];
        _screenTimer += dt;

        if (_isFadingIn)
        {
            currentScreen.alpha += _fadeSpeed * dt;
            if (currentScreen.alpha >= 1.0f)
            {
                currentScreen.alpha = 1.0f;
                _isFadingIn = false;
            }
        }
        else if (_isFadingOut)
        {
            currentScreen.alpha -= _fadeSpeed * dt;
            if (currentScreen.alpha <= 0.0f)
            {
                currentScreen.alpha = 0.0f;
                _isFadingOut = false;

                if (currentScreen.imageObject != nullptr)
                {
                    delete currentScreen.imageObject;
                    currentScreen.imageObject = nullptr;
                }

                _currentScreenIndex++;
                _screenTimer = 0.0f;

                if (_currentScreenIndex < _screens.size())
                {
                    LoadCurrentScreen();
                    _isFadingIn = true;
                }
            }
        }
        else if (_screenTimer >= currentScreen.duration)
        {
            _isFadingOut = true;
        }

        if (currentScreen.imageObject != nullptr)
        {
            currentScreen.imageObject->Update(dt);
        }

        Scene::Update(dt);
    }

    void Render() override
    {
        if (_currentScreenIndex >= _screens.size())
            return;

        IntroScreen& currentScreen = _screens[_currentScreenIndex];

        if (currentScreen.imageObject != nullptr)
        {
            currentScreen.imageObject->Render();

            if (currentScreen.alpha < 1.0f)
            {
                SDL_FRect fadeRect;
                fadeRect.x = 0.0f;
                fadeRect.y = 0.0f;
                fadeRect.w = (float)RM.WINDOW_WIDTH;
                fadeRect.h = (float)RM.WINDOW_HEIGHT;

                Uint8 fadeAlpha = (Uint8)((1.0f - currentScreen.alpha) * 255);
                SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, fadeAlpha);
                SDL_RenderFillRect(RM.GetRenderer(), &fadeRect);
            }
        }

        Scene::Render();
    }

private:
    void LoadCurrentScreen()
    {
        if (_currentScreenIndex >= _screens.size())
            return;

        IntroScreen& screen = _screens[_currentScreenIndex];

        screen.imageObject = new ImageObject(
            screen.texturePath,
            Vector2(0.0f, 0.0f),
            Vector2((float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT)
        );

        screen.imageObject->GetTransform()->position = Vector2(
            RM.WINDOW_WIDTH / 2.0f,
            RM.WINDOW_HEIGHT / 2.0f
        );
        screen.imageObject->GetTransform()->size = Vector2(
            (float)RM.WINDOW_WIDTH,
            (float)RM.WINDOW_HEIGHT
        );
        screen.imageObject->GetTransform()->scale = Vector2(1.0f, 1.0f);

        screen.alpha = 0.0f;
    }
};
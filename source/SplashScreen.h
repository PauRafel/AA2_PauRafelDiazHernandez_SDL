#pragma once
#pragma once
#include "Scene.h"
#include "ImageObject.h"
#include "RenderManager.h"
#include "SceneManager.h"

class SplashScreen : public Scene
{
private:
    enum SplashPhase
    {
        PHASE_TITLE,
        PHASE_ART
    };

    SplashPhase _currentPhase;
    ImageObject* _currentImage;
    float _phaseTimer;
    float _phaseDuration;
    float _fadeAlpha;
    float _fadeSpeed;
    bool _isFadingIn;
    bool _isFadingOut;

public:
    SplashScreen() = default;

    void OnEnter() override
    {
        _currentPhase = PHASE_TITLE;
        _phaseTimer = 0.0f;
        _phaseDuration = 3.0f;
        _fadeAlpha = 0.0f;
        _fadeSpeed = 2.0f;
        _isFadingIn = true;
        _isFadingOut = false;

        LoadTitleScreen();
    }

    void OnExit() override
    {
        _currentImage = nullptr;
        Scene::OnExit();
    }

    void Update(float dt) override
    {
        _phaseTimer += dt;

        if (_isFadingIn)
        {
            _fadeAlpha += _fadeSpeed * dt;
            if (_fadeAlpha >= 1.0f)
            {
                _fadeAlpha = 1.0f;
                _isFadingIn = false;
            }
        }
        else if (_isFadingOut)
        {
            _fadeAlpha -= _fadeSpeed * dt;
            if (_fadeAlpha <= 0.0f)
            {
                _fadeAlpha = 0.0f;
                _isFadingOut = false;

                if (_currentPhase == PHASE_TITLE)
                {
                    _currentPhase = PHASE_ART;
                    _phaseTimer = 0.0f;

                    if (_currentImage != nullptr)
                    {
                        delete _currentImage;
                        _currentImage = nullptr;
                    }
                    _objects.clear();

                    LoadArtScreen();
                    _isFadingIn = true;
                }
                else if (_currentPhase == PHASE_ART)
                {
                    SM.SetNextScene("MainMenu");
                }
            }
        }
        else if (_phaseTimer >= _phaseDuration)
        {
            _isFadingOut = true;
        }

        Scene::Update(dt);
    }

    void Render() override
    {
        Scene::Render();

        if (_fadeAlpha < 1.0f)
        {
            SDL_FRect fadeRect;
            fadeRect.x = 0.0f;
            fadeRect.y = 0.0f;
            fadeRect.w = (float)RM.WINDOW_WIDTH;
            fadeRect.h = (float)RM.WINDOW_HEIGHT;

            Uint8 fadeValue = (Uint8)((1.0f - _fadeAlpha) * 255);
            SDL_SetRenderDrawColor(RM.GetRenderer(), 0, 0, 0, fadeValue);
            SDL_RenderFillRect(RM.GetRenderer(), &fadeRect);
        }
    }

private:
    void LoadTitleScreen()
    {
        _currentImage = new ImageObject(
            "resources/intro/splash_title.png",
            Vector2(0.0f, 0.0f),
            Vector2((float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT)
        );

        _currentImage->GetTransform()->position = Vector2(
            RM.WINDOW_WIDTH / 2.0f,
            RM.WINDOW_HEIGHT / 2.0f
        );
        _currentImage->GetTransform()->size = Vector2(
            (float)RM.WINDOW_WIDTH,
            (float)RM.WINDOW_HEIGHT
        );
        _currentImage->GetTransform()->scale = Vector2(1.0f, 1.0f);

        _objects.push_back(_currentImage);
    }

    void LoadArtScreen()
    {
        _currentImage = new ImageObject(
            "resources/intro/splash_art.png",
            Vector2(0.0f, 0.0f),
            Vector2((float)RM.WINDOW_WIDTH, (float)RM.WINDOW_HEIGHT)
        );

        _currentImage->GetTransform()->position = Vector2(
            RM.WINDOW_WIDTH / 2.0f,
            RM.WINDOW_HEIGHT / 2.0f
        );
        _currentImage->GetTransform()->size = Vector2(
            (float)RM.WINDOW_WIDTH,
            (float)RM.WINDOW_HEIGHT
        );
        _currentImage->GetTransform()->scale = Vector2(1.0f, 1.0f);

        _objects.push_back(_currentImage);
    }
};
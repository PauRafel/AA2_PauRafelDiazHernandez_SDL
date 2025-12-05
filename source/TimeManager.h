#pragma once
#include <SDL3/SDL.h>
#include <cmath>

#define TM TimeManager::Instance()

class TimeManager
{
private:
    TimeManager();
    ~TimeManager() = default;

    TimeManager(const TimeManager& t) = delete;
    TimeManager& operator=(const TimeManager& t) = delete;

    float _deltaTime = 0.0f;
    float _elapsedTime = 0.0f;
    float _previousElapsedTime = 0.0f;

    const int _FPS = 60;
    float _frameTime;

public:
    static TimeManager& Instance()
    {
        static TimeManager instance;
        return instance;
    }

    float GetDeltaTime() const { return _deltaTime; }
    float GetElapsedTime() const { return _elapsedTime; }

    bool ShouldUpdateGame() const { return _deltaTime >= _frameTime; }
    void ResetDeltaTime();
    void Update();
};
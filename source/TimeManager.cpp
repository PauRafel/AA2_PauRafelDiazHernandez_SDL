#include "TimeManager.h"

TimeManager::TimeManager()
{
    _previousElapsedTime = (double)SDL_GetTicks() / 1000.f;
    _frameTime = 1.0f / (float)_FPS;
}

void TimeManager::ResetDeltaTime()
{
    _deltaTime = _deltaTime - (float)(floor(_deltaTime / _frameTime) * _frameTime);
}

void TimeManager::Update()
{
    _elapsedTime = (double)SDL_GetTicks() / 1000.0;
    _deltaTime += _elapsedTime - _previousElapsedTime;
    _previousElapsedTime = _elapsedTime;
}
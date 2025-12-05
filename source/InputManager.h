#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>

#define IM InputManager::Instance()

enum KeyState { EMPTY, DOWN, UP, HOLD, RELEASED };

class InputManager
{
public:
    static InputManager& Instance()
    {
        static InputManager instance;
        return instance;
    }

    bool Listen();

    float GetMouseX() const { return _mouseX; }
    float GetMouseY() const { return _mouseY; }
    bool GetLeftClick() const { return _leftClick; }
    bool GetEvent(SDL_Keycode input, KeyState inputValue);

private:
    InputManager() = default;
    InputManager(InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    ~InputManager() {}

    float _mouseX = 0.0f;
    float _mouseY = 0.0f;
    bool _leftClick = false;

    std::unordered_map<SDL_Keycode, KeyState> _keyReference;
};
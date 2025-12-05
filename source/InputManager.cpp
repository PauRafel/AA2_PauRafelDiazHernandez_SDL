#include "InputManager.h"

bool InputManager::Listen()
{
    for (std::unordered_map<SDL_Keycode, KeyState>::iterator it = _keyReference.begin();
        it != _keyReference.end();
        it++)
    {
        if (it->second == DOWN)
        {
            it->second = HOLD;
        }
        else if (it->second == UP)
        {
            it->second = RELEASED;
        }
    }

    SDL_GetMouseState(&_mouseX, &_mouseY);

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            return true;

        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                _leftClick = true;
        }

        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                _leftClick = false;
        }

        else if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (_keyReference[event.key.key] != HOLD)
                _keyReference[event.key.key] = DOWN;
        }

        else if (event.type == SDL_EVENT_KEY_UP)
        {
            _keyReference[event.key.key] = UP;
        }
    }

    return false;
}

bool InputManager::GetEvent(SDL_Keycode input, KeyState inputValue)
{
    return _keyReference[input] == inputValue;
}
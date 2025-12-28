#pragma once
#include "TextObject.h"
#include "InputManager.h"
#include "RenderManager.h"
#include <functional>
#include <string>
#include <iostream>

typedef std::function<void()> ButtonCallback;

enum ButtonState
{
    BUTTON_NORMAL,
    BUTTON_HOVER,
    BUTTON_CLICKED
};

class Button : public TextObject
{
private:
    ButtonState _state;
    ButtonCallback _onClick;

    SDL_Color _normalColor;
    SDL_Color _hoverColor;
    SDL_Color _clickedColor;

    SDL_Color _normalBgColor;
    SDL_Color _hoverBgColor;
    SDL_Color _clickedBgColor;

    bool _wasHovered;
    bool _wasMousePressed;
    float _clickTimer;
    float _clickDuration;

    Vector2 _normalScale;
    Vector2 _hoverScale;

    Vector2 _buttonSize;
    SDL_FRect _backgroundRect;
    AABB* _buttonCollider; 

public:
    Button(std::string text, std::string fontPath, Vector2 position, ButtonCallback onClick)
        : TextObject(text, fontPath)
    {
        _onClick = onClick;
        _state = BUTTON_NORMAL;
        _wasHovered = false;
        _wasMousePressed = false;
        _clickTimer = 0.0f;
        _clickDuration = 0.15f;

        _normalColor = { 200, 200, 200, 255 };   
        _hoverColor = { 0, 255, 255, 255 };      
        _clickedColor = { 255, 255, 0, 255 };     

        _normalBgColor = { 40, 40, 40, 255 };     
        _hoverBgColor = { 60, 60, 80, 255 };     
        _clickedBgColor = { 80, 80, 40, 255 }; 

        _normalScale = Vector2(1.0f, 1.0f);
        _hoverScale = Vector2(1.15f, 1.15f);

        _buttonSize = Vector2(400.f, 70.f);

        _transform->position = position;
        _transform->scale = _normalScale;

        SetColor(_normalColor);

        dynamic_cast<TextRenderer*>(_renderer)->SetRenderOffset(Vector2(-100.f, 20.f));

        _buttonCollider = new AABB(Vector2(0.f, 0.f), _buttonSize);

        UpdateCollider();
    }

    ~Button()
    {
        if (_buttonCollider != nullptr)
        {
            delete _buttonCollider;
            _buttonCollider = nullptr;
        }
    }

    void Update(float dt) override
    {
        Vector2 mousePos(IM.GetMouseX(), IM.GetMouseY());

        bool isHovering = _buttonCollider->CheckOverlappingPoint(mousePos);
        bool isMousePressed = IM.GetLeftClick();

        if (_state == BUTTON_CLICKED)
        {
            _clickTimer += dt;
            if (_clickTimer >= _clickDuration)
            {
                _state = BUTTON_NORMAL;
                _clickTimer = 0.0f;
                _transform->scale = _normalScale;
                SetColor(_normalColor);
            }
        }
        else
        {
            if (isHovering && !_wasHovered)
            {
                OnHoverEnter();
            }
            else if (!isHovering && _wasHovered)
            {
                OnHoverExit();
            }

            if (isHovering && !isMousePressed && _wasMousePressed)
            {
                OnClick();
            }
        }

        _wasHovered = isHovering;
        _wasMousePressed = isMousePressed;

        UpdateCollider();

        if (_renderer != nullptr)
            _renderer->Update(dt);
    }

    void Render() override
    {
        SDL_Color bgColor = _normalBgColor;
        if (_state == BUTTON_HOVER)
            bgColor = _hoverBgColor;
        else if (_state == BUTTON_CLICKED)
            bgColor = _clickedBgColor;

        SDL_SetRenderDrawColor(RM.GetRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(RM.GetRenderer(), &_backgroundRect);

        SDL_Color borderColor = { 100, 100, 100, 255 };
        if (_state == BUTTON_HOVER)
            borderColor = { 0, 200, 200, 255 };
        else if (_state == BUTTON_CLICKED)
            borderColor = { 200, 200, 0, 255 };

        SDL_SetRenderDrawColor(RM.GetRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderRect(RM.GetRenderer(), &_backgroundRect);

        if (_renderer != nullptr)
            _renderer->Render();
    }

    void SetNormalColor(SDL_Color color) { _normalColor = color; }
    void SetHoverColor(SDL_Color color) { _hoverColor = color; }
    void SetClickedColor(SDL_Color color) { _clickedColor = color; }

    void SetNormalBgColor(SDL_Color color) { _normalBgColor = color; }
    void SetHoverBgColor(SDL_Color color) { _hoverBgColor = color; }
    void SetClickedBgColor(SDL_Color color) { _clickedBgColor = color; }

private:
    void UpdateCollider()
    {
        Vector2 offset = _buttonSize / -2.0f;

        _buttonCollider->SetTopLeft(_transform->position + offset);
        _buttonCollider->SetSize(_buttonSize);

        _backgroundRect.x = _transform->position.x + offset.x;
        _backgroundRect.y = _transform->position.y + offset.y;
        _backgroundRect.w = _buttonSize.x;
        _backgroundRect.h = _buttonSize.y;
    }

    void OnHoverEnter()
    {
        if (_state != BUTTON_CLICKED)
        {
            _state = BUTTON_HOVER;
            _transform->scale = _hoverScale;
            SetColor(_hoverColor);
        }
    }

    void OnHoverExit()
    {
        if (_state != BUTTON_CLICKED)
        {
            _state = BUTTON_NORMAL;
            _transform->scale = _normalScale;
            SetColor(_normalColor);
        }
    }

    void OnClick()
    {
        _state = BUTTON_CLICKED;
        _clickTimer = 0.0f;
        SetColor(_clickedColor);

        std::cout << "Button clicked!" << std::endl;

        if (_onClick)
        {
            _onClick();
        }
    }
};
#pragma once
#include "Scene.h"

class Gameplay : public Scene
{
public:
    Gameplay() = default;

    void OnEnter() override
    {}

    void OnExit() override
    {
        Scene::OnExit();
    }

    void Update(float dt) override
    {
        Scene::Update(dt);
    }

    void Render() override
    {
        Scene::Render();
    }
};
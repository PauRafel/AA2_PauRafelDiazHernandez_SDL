#pragma once
#include "Object.h"
#include <vector>

class Scene
{
protected:
    std::vector<Object*> _ui;
    std::vector<Object*> _objects;

public:
    Scene() = default;
    virtual ~Scene() {}

    virtual void OnEnter() = 0;
    virtual void OnExit();
    virtual void Update(float dt);
    virtual void Render();
};
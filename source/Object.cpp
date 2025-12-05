#include "Object.h"

Object::Object()
{
    _transform = new Transform();
    _physics = new RigidBody(_transform);
}

Object::~Object()
{
    delete _transform;
    _transform = nullptr;

    delete _physics;
    _physics = nullptr;

    delete _renderer;
    _renderer = nullptr;
}

void Object::Update(float dt)
{
    if (_physics != nullptr)
        _physics->Update(dt);

    if (_renderer != nullptr)
        _renderer->Update(dt);
}

void Object::Render()
{
    if (_renderer != nullptr)
        _renderer->Render();
}
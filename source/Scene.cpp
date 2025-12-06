#include "Scene.h"

void Scene::OnExit()
{
    for (Object* o : _objects)
        delete o;

    _objects.clear();

    for (Object* u : _ui)
        delete u;

    _ui.clear();
}

void Scene::Update(float dt)
{
    for (int i = _objects.size() - 1; i >= 0; i--)
    {
        if (_objects[i]->IsPendingDestroy())
        {
            delete _objects[i];
            _objects.erase(_objects.begin() + i);
        }
    }

    for (int i = _ui.size() - 1; i >= 0; i--)
    {
        if (_ui[i]->IsPendingDestroy())
        {
            delete _ui[i];
            _ui.erase(_ui.begin() + i);
        }
    }

    for (Object* o : _objects)
        o->Update(dt);

    for (Object* u : _ui)
        u->Update(dt);

    int size = _objects.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = i + 1; j < _objects.size(); j++)
        {
            if (_objects[i]->GetRigidBody()->CheckCollision(_objects[j]->GetRigidBody()))
            {}
        }
    }
}

void Scene::Render()
{
    for (Object* o : _objects)
        o->Render();

    for (Object* u : _ui)
        u->Render();
}
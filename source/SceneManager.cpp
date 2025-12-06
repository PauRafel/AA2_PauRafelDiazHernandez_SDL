#include "SceneManager.h"

bool SceneManager::AddScene(std::string name, Scene* scene)
{
    if (_scenes.find(name) == _scenes.end())
    {
        _scenes.emplace(name, scene);
        return true;
    }

    return false;
}

Scene* SceneManager::GetScene(std::string name)
{
    if (_scenes.find(name) != _scenes.end())
        return _scenes[name];

    return nullptr;
}

bool SceneManager::InitFirstScene(std::string name)
{
    if (_scenes.find(name) != _scenes.end())
    {
        _currentScene = _scenes[name];
        _currentScene->OnEnter();
        return true;
    }

    return false;
}

bool SceneManager::SetNextScene(std::string name)
{
    if (_scenes.find(name) == _scenes.end())
        return false;

    _nextScene = name;
    return true;
}

void SceneManager::UpdateCurrentScene(float dt)
{
    if (_nextScene != "")
    {
        _currentScene->OnExit();
        _currentScene = _scenes[_nextScene];
        _currentScene->OnEnter();
        _nextScene = "";
    }

    _currentScene->Update(dt);
}
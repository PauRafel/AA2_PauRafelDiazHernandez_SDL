#pragma once
#include "Scene.h"
#include <unordered_map>
#include <string>

#define SM SceneManager::Instance()

class SceneManager
{
private:
    SceneManager() = default;
    SceneManager(SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::unordered_map<std::string, Scene*> _scenes;
    Scene* _currentScene = nullptr;
    std::string _nextScene = "";

public:
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    bool AddScene(std::string name, Scene* scene);
    Scene* GetScene(std::string name);
    bool InitFirstScene(std::string name);
    Scene* GetCurrentScene() const { return _currentScene; }
    bool SetNextScene(std::string name);
    void UpdateCurrentScene(float dt);
};
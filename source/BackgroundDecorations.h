#pragma once
#include "ImageObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include <vector>
#include <string>

enum DecorationType
{
    DECO_FLOOR,  
    DECO_CEILING 
};

struct DecorationData
{
    std::string texturePath;
    DecorationType type;
    Vector2 size;
};

class BackgroundDecoration : public ImageObject
{
private:
    float _scrollSpeed;
    DecorationType _type;

public:
    BackgroundDecoration(std::string texturePath, Vector2 spawnPosition, Vector2 size, DecorationType type, float scrollSpeed);

    void Update(float dt) override;

    bool IsOffScreen() const;
};

class BackgroundDecorations
{
private:
    std::vector<BackgroundDecoration*> _decorations;
    std::vector<DecorationData> _availableDecorations;

    float _spawnTimer;
    float _spawnInterval; 
    float _scrollSpeed;
    bool _isScrolling;

    int _currentLevel; 

public:
    BackgroundDecorations(int level);
    ~BackgroundDecorations();

    void Update(float dt);
    void Render();

    void StopScrolling();
    void StartScrolling();
    void Clear();

private:
    void LoadDecorationsForLevel(int level);
    void SpawnRandomDecoration();
    Vector2 GetSpawnPosition(DecorationType type, Vector2 size);
};
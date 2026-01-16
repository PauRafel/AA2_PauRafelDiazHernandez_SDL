#include "BackgroundDecorations.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

BackgroundDecoration::BackgroundDecoration(
    std::string texturePath,
    Vector2 spawnPosition,
    Vector2 size,
    DecorationType type,
    float scrollSpeed)
    : ImageObject(texturePath, Vector2(0.f, 0.f), size)
{
    _transform->position = spawnPosition;
    _transform->size = size;
    _transform->scale = Vector2(1.f, 1.f);

    _scrollSpeed = scrollSpeed;
    _type = type;
}

void BackgroundDecoration::Update(float dt)
{
    _transform->position.x -= _scrollSpeed * dt;

    if (_renderer != nullptr)
        _renderer->Update(dt);
}

bool BackgroundDecoration::IsOffScreen() const
{
    return (_transform->position.x + _transform->size.x) < 0.0f;
}

BackgroundDecorations::BackgroundDecorations(int level)
{
    _spawnTimer = 0.0f;
    _spawnInterval = 3.0f; 
    _scrollSpeed = 200.0f; 
    _isScrolling = true;
    _currentLevel = level;

    LoadDecorationsForLevel(level);

    static bool seeded = false;
    if (!seeded)
    {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
}

BackgroundDecorations::~BackgroundDecorations()
{
    Clear();
}

void BackgroundDecorations::LoadDecorationsForLevel(int level)
{
    _availableDecorations.clear();

    if (level == 1)
    {
        for (int i = 1; i <= 3; i++)
        {
            DecorationData deco;
            deco.texturePath = "resources/background_decos/deco_" + std::to_string(i) + "_lvl1.png";
            deco.type = DECO_FLOOR;
            deco.size = Vector2(240.f, 240.f); 
            _availableDecorations.push_back(deco);
        }

        for (int i = 4; i <= 5; i++)
        {
            DecorationData deco;
            deco.texturePath = "resources/background_decos/deco_" + std::to_string(i) + "_lvl1.png";
            deco.type = DECO_CEILING;
            deco.size = Vector2(240.f, 240.f); 
            _availableDecorations.push_back(deco);
        }

        std::cout << "Loaded " << _availableDecorations.size() << " decorations for Level 1" << std::endl;
    }
    else if (level == 2)
    {
        for (int i = 1; i <= 5; i++)
        {
            DecorationData deco;
            deco.texturePath = "resources/background_decos/deco_" + std::to_string(i) + "_lvl2.png";
            deco.type = DECO_FLOOR;
            deco.size = Vector2(240.f, 240.f);
            _availableDecorations.push_back(deco);
        }

        std::cout << "Loaded " << _availableDecorations.size() << " decorations for Level 2" << std::endl;
    }
}

void BackgroundDecorations::Update(float dt)
{
    if (_isScrolling)
    {
        _spawnTimer += dt;

        if (_spawnTimer >= _spawnInterval)
        {
            SpawnRandomDecoration();
            _spawnTimer = 0.0f;
        }
    }

    for (int i = _decorations.size() - 1; i >= 0; i--)
    {
        if (_isScrolling)
        {
            _decorations[i]->Update(dt);
        }

        if (_decorations[i]->IsOffScreen())
        {
            delete _decorations[i];
            _decorations.erase(_decorations.begin() + i);
        }
    }
}

void BackgroundDecorations::Render()
{
    for (BackgroundDecoration* deco : _decorations)
    {
        deco->Render();
    }
}

void BackgroundDecorations::SpawnRandomDecoration()
{
    if (_availableDecorations.empty())
        return;

    int randomIndex = rand() % _availableDecorations.size();
    DecorationData& chosenDeco = _availableDecorations[randomIndex];

    Vector2 spawnPos = GetSpawnPosition(chosenDeco.type, chosenDeco.size);

    BackgroundDecoration* newDeco = new BackgroundDecoration(
        chosenDeco.texturePath,
        spawnPos,
        chosenDeco.size,
        chosenDeco.type,
        _scrollSpeed
    );

    _decorations.push_back(newDeco);

    std::cout << "Spawned decoration: " << chosenDeco.texturePath
        << " at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
}

Vector2 BackgroundDecorations::GetSpawnPosition(DecorationType type, Vector2 size)
{
    float x = RM.WINDOW_WIDTH + 50.f;
    float y = 0.f;

    if (type == DECO_FLOOR)
    {
        y = RM.WINDOW_HEIGHT - size.y + 80.f; 
    }
    else if (type == DECO_CEILING)
    {
        y = 180.f; 
    }

    return Vector2(x, y);
}

void BackgroundDecorations::StopScrolling()
{
    _isScrolling = false;
}

void BackgroundDecorations::StartScrolling()
{
    _isScrolling = true;
}

void BackgroundDecorations::Clear()
{
    for (BackgroundDecoration* deco : _decorations)
        delete deco;

    _decorations.clear();
    _spawnTimer = 0.0f;
}
#pragma once
#include "ImageObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include "EnemyStateMachine.h"
#include <cmath>

class Enemy : public ImageObject
{
protected:
    EnemyStateMachine* _stateMachine;
    int _health;
    int _scoreValue;
    bool _escapedOffScreen;

public:
    Enemy(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition);
    virtual ~Enemy();
    void Update(float dt) override;
    void TakeDamage(int damage);
    int GetHealth() const { return _health; }
    int GetScoreValue() const { return _scoreValue; }
    bool HasEscaped() const { return _escapedOffScreen; } 

    EnemyStateMachine* GetStateMachine() { return _stateMachine; }
};
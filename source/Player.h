#pragma once
#include "ImageObject.h"
#include "InputManager.h"
#include "RenderManager.h"
#include <vector>

class Bullet;

enum PowerUpState
{
    STATE_NONE,
    STATE_CA1,      
    STATE_LA1,     
    STATE_SPEED,   
    STATE_CA2,      
    STATE_LA2,      
    STATE_SHIELD,   
    STATE_TURRETS,
    STATE_FULL_SHIELD
};

class Player : public ImageObject
{
private:
    float _moveSpeed = 300.0f;
    float _baseSpeed = 300.0f;
    std::vector<Bullet*>* _bulletsVector;

    PowerUpState _currentState = STATE_NONE;
    bool _hasTurrets = false;
    int _cannonAmmo = 0;    
    int _laserAmmo = 0;   
    int _shield = 100;    
    int _score = 0;

    bool _isInvulnerable = false;
    float _invulnerabilityTimer = 0.0f;
    float _invulnerabilityDuration = 10.0f;

    float _distanceTraveled = 0.0f;
    float _turretAngle = 0.0f;  

public:
    Player(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, std::vector<Bullet*>* bulletsVector);

    void Update(float dt) override;
    void Shoot();

    void ApplyPowerUp(PowerUpState newState);
    PowerUpState GetCurrentState() const { return _currentState; }
    int GetScore() const { return _score; }
    int GetShield() const { return _shield; }
    int GetCannonAmmo() const { return _cannonAmmo; }
    int GetLaserAmmo() const { return _laserAmmo; }
    bool IsInvulnerable() const { return _isInvulnerable; }

    void AddScore(int amount) { _score += amount; }
    void TakeDamage(int damage);
};
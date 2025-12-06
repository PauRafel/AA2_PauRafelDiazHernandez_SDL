#pragma once
#include "ImageObject.h"
#include "RenderManager.h"

enum PowerUpType
{
    POWERUP_SCORE,   
    POWERUP_CA1,        
    POWERUP_LA1,     
    POWERUP_SPEED,      
    POWERUP_CA2,        
    POWERUP_LA2,      
    POWERUP_SHIELD, 
    POWERUP_TURRETS  
};

class PowerUp : public ImageObject
{
private:
    PowerUpType _type;
    int _hitsRequired;  
    int _currentHits;  
    bool _activated; 

public:
    PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition, PowerUpType type);

    void Update(float dt) override;
    void Hit(); 
    bool IsActivated() const { return _activated; }
    PowerUpType GetType() const { return _type; }
};
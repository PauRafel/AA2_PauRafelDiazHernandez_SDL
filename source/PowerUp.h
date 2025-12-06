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
    PowerUpType _currentType;
    int _cycleIndex;       
    int _hitsToNextState;   
    int _currentHits;      
    float _scrollSpeed;    

    PowerUpType _powerUpCycle[8] = {
        POWERUP_SCORE,
        POWERUP_CA1,
        POWERUP_LA1,
        POWERUP_SPEED,
        POWERUP_CA2,
        POWERUP_LA2,
        POWERUP_SHIELD,
        POWERUP_TURRETS
    };

public:
    PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition);

    void Update(float dt) override;
    void Hit();
    PowerUpType GetCurrentType() const { return _currentType; }
    int GetCycleIndex() const { return _cycleIndex; }
};
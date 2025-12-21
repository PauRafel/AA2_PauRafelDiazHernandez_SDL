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
    POWERUP_TURRETS,
    POWERUP_FULL_SHIELD
};

class PowerUp : public ImageObject
{
private:
    PowerUpType _currentType;
    int _cycleIndex;       
    int _hitsToNextState;   
    int _currentHits;      
    float _scrollSpeed;    

    PowerUpType _powerUpCycle[9] = {
        POWERUP_SCORE,
        POWERUP_CA1,
        POWERUP_LA1,
        POWERUP_SPEED,
        POWERUP_CA2,
        POWERUP_LA2,
        POWERUP_SHIELD,
        POWERUP_TURRETS,
        POWERUP_FULL_SHIELD
    };

    std::string _powerUpTextures[9] = {
       "resources/powerups/powerup_score.png",
       "resources/powerups/powerup_ca1.png",
       "resources/powerups/powerup_la1.png",
       "resources/powerups/powerup_speed.png",
       "resources/powerups/powerup_ca2.png",
       "resources/powerups/powerup_la2.png",
       "resources/powerups/powerup_shield.png",
       "resources/powerups/powerup_turrets.png",
       "resources/powerups/powerup_full_shield.png"
    };

    void UpdateSprite();

public:
    PowerUp(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition);

    void Update(float dt) override;
    void Hit();
    PowerUpType GetCurrentType() const { return _currentType; }
    int GetCycleIndex() const { return _cycleIndex; }
};
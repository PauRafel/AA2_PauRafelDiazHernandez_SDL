#pragma once
#include "ImageObject.h"
#include "RenderManager.h"
#include "TimeManager.h"
#include <cmath>

class Enemy : public ImageObject
{
private:
    Vector2 _centerPoint;  
    float _radius;        
    float _angle;          
    float _angularSpeed;   

public:
    Enemy(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize, Vector2 spawnPosition);

    void Update(float dt) override;
};
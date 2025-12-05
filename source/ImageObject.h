#pragma once
#include "Object.h"
#include "ImageRenderer.h"

class ImageObject : public Object
{
public:
    ImageObject(std::string texturePath, Vector2 sourceOffset, Vector2 sourceSize)
        : Object()
    {
        _renderer = new ImageRenderer(_transform, texturePath, sourceOffset, sourceSize);
    }
};
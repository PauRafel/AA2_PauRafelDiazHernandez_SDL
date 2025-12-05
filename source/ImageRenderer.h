#pragma once
#include "Renderer.h"
#include "RenderManager.h"

class ImageRenderer : public Renderer
{
public:
    ImageRenderer(Transform* transform, std::string resourcePath, Vector2 sourceOffset, Vector2 sourceSize);

    virtual void Update(float dt) override;
    virtual void Render() override;
};
#include "ImageRenderer.h"

ImageRenderer::ImageRenderer(Transform* transform, std::string resourcePath, Vector2 sourceOffset, Vector2 sourceSize)
    : Renderer(transform, resourcePath)
{
    _sourceRect = SDL_FRect
    {
        sourceOffset.x,
        sourceOffset.y,
        sourceSize.x,
        sourceSize.y
    };

    _destRect = SDL_FRect
    {
        transform->position.x,
        transform->position.y,
        transform->scale.x,
        transform->scale.y
    };
}

void ImageRenderer::Update(float dt)
{
    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;

    _destRect.x = _transform->position.x + offset.x;
    _destRect.y = _transform->position.y + offset.y;

    _destRect.w = _transform->size.x * _transform->scale.x;
    _destRect.h = _transform->size.y * _transform->scale.y;
}

void ImageRenderer::Render()
{
    SDL_RenderTextureRotated(
        RM.GetRenderer(),
        RM.GetTexture(_resourcePath),
        &_sourceRect,
        &_destRect,
        _transform->rotation,
        NULL,
        SDL_FLIP_NONE
    );
}
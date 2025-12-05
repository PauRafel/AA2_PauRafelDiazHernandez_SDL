#include "RigidBody.h"

RigidBody::RigidBody(Transform* transform)
    : _transform(transform),
    _velocity(Vector2()),
    _angularVelocity(0.0f),
    _acceleration(Vector2()),
    _angularAcceleration(0.0f),
    _linearDrag(0.0f),
    _angularDrag(0.0f)
{
}

RigidBody::~RigidBody()
{
    for (AABB* collider : _colliders)
        delete collider;

    _colliders.clear();
}

bool RigidBody::CheckCollision(const RigidBody* other)
{
    for (AABB* ownCollider : _colliders)
    {
        for (AABB* otherCollider : other->_colliders)
        {
            if (ownCollider->CheckOverlappingAABB(otherCollider))
                return true;
        }
    }
    return false;
}

bool RigidBody::CheckOverlappingPoint(Vector2 point)
{
    for (AABB* ownCollider : _colliders)
    {
        if (ownCollider->CheckOverlappingPoint(point))
            return true;
    }
    return false;
}

void RigidBody::Update(float dt)
{
    _velocity = _velocity + _acceleration * dt;
    _angularVelocity = _angularVelocity + _angularAcceleration * dt;

    _velocity = _velocity * (1.0f / (1.0f + dt * _linearDrag));
    _angularVelocity = _angularVelocity * (1.0f / (1.0f + dt * _angularDrag));

    _transform->position = _transform->position + _velocity * dt;
    _transform->rotation = _transform->rotation + _angularVelocity * dt;

    _acceleration = Vector2();
    _angularAcceleration = 0.0f;

    Vector2 offset = (Vector2(-_transform->size.x, -_transform->size.y) / 2.0f) * _transform->scale;

    for (AABB* collider : _colliders)
    {
        collider->SetTopLeft(_transform->position + offset);
        collider->SetSize(_transform->size * _transform->scale);
    }
}
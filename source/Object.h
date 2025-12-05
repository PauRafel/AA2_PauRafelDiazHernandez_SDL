#pragma once
#include "Transform.h"
#include "Renderer.h"
#include "RigidBody.h"

class Object
{
private:
    bool _isPendingDestroy = false;

protected:
    Renderer* _renderer = nullptr;
    Transform* _transform;
    RigidBody* _physics;

public:
    Object();
    virtual ~Object();

    virtual void Update(float dt);
    virtual void Render();

    Transform* GetTransform() { return _transform; }
    RigidBody* GetRigidBody() { return _physics; }

    bool IsPendingDestroy() const { return _isPendingDestroy; }
    virtual void Destroy() { _isPendingDestroy = true; }
};

#pragma once
#include "ECS/System.h"

class Transform;

template <typename T>
class ComponentHandle;

class TransformSystem :	public System
{
private:
    // deprecated
    void UpdateTransform(Transform * transform);
public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;

    TransformSystem();
    virtual ~TransformSystem();
};

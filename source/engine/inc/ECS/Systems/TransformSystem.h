
#pragma once
#include "ECS/System.h"
namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Transform;
        }
        template <typename T>
        class ComponentHandle;
    }
}

class TransformSystem :	public Core::ECS::System
{
private:
    // deprecated
    void UpdateTransform(Core::ECS::Components::Transform * transform);
public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo) override;

    TransformSystem();
    virtual ~TransformSystem();
};

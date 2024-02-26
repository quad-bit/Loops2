
#pragma once
#include "ECS/System.h"
#include "Utility/ShaderResourceDescription.h"
#include "RenderData.h"

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Bound;
            class Transform;
        }

        namespace Events
        {
            class BoundAdditionEvent;
        }
    }
}


class BoundSystem : public Core::ECS::System
{
private:
    Core::Utility::GlobalResourceAllocationConfig allocConfig;
    Core::Utility::GlobalResourceSharingConfig resourceSharingConfig;

    Core::Wrappers::SetWrapper * boundSetWrapper;
    std::vector<Core::Utility::DescriptorSetInfo> resDescriptionList;
    std::map<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo> transformBoundToBindDescMap;
    std::map<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo> lightBoundToBindDescMap;
    std::map<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo> cameraBoundToBindDescMap;

    uint32_t numDescriptorsPerBinding;
    size_t memoryAlignedUniformSize;;
    std::vector<Core::Utility::BoundData>& m_boundDataList;

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo) override;

    BoundSystem(std::vector<Core::Utility::BoundData>& boundDataList);
    virtual ~BoundSystem();

    void HandleBoundAddition(Core::ECS::Events::BoundAdditionEvent * inputEvent);
};

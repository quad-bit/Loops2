
#pragma once
#include "ECS/System.h"
#include "Utility/ShaderResourceDescription.h"
#include "RenderData.h"
//#include "DrawGraphNode.h"

template <typename T>
class GraphNode;

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class MeshRenderer;
            class Transform;
        }

        namespace Events
        {
            class MeshRendererAdditionEvent;
        }
    }
}


class MeshRendererSystem : public Core::ECS::System
{
private:
    uint32_t idCounter = 0;
    uint32_t GenerateId();

    Core::Utility::GlobalResourceAllocationConfig allocConfig;
    Core::Utility::GlobalResourceSharingConfig resourceSharingConfig;

    Core::Wrappers::SetWrapper * transformSetWrapper;
    std::vector<Core::Utility::DescriptorSetInfo> resDescriptionList;
    std::map<Core::ECS::Components::Transform *, Core::Utility::DescriptorSetInfo> transformToBindDescMap;

    uint32_t numDescriptorsPerBinding;
    size_t memoryAlignedUniformSize;;
    std::vector<Core::Utility::TransformData>& m_transformDataList;
    std::map<std::string, std::vector<Core::Utility::TransformData>>& m_perEffectTransformData;

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo) override;

    MeshRendererSystem(std::vector<Core::Utility::TransformData>&, std::map<std::string, std::vector<Core::Utility::TransformData>>&);
    virtual ~MeshRendererSystem();

    void HandleMeshRendererAddition(Core::ECS::Events::MeshRendererAdditionEvent * inputEvent);
};

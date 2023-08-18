
#pragma once
#include "ECS/System.h"
#include "Utility/ShaderResourceDescription.h"
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
    std::vector<Core::Utility::ShaderBindingDescription *> resDescriptionList;
    std::map<Core::ECS::Components::Transform *, Core::Utility::ShaderBindingDescription *> transformToBindDescMap;

    uint32_t numDescriptorsPerBinding;
/*
    std::map<uint32_t, DrawGraphNode*> meshIdToGraphNode;

    std::vector<GraphNode<DrawGraphNode> * >  meshNodeList;
    std::vector<GraphNode<DrawGraphNode> * >  transformNodeList;
    std::vector<GraphNode<DrawGraphNode> * >  drawingNodeList;
*/
    size_t memoryAlignedDataSize;

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;

    MeshRendererSystem();
    virtual ~MeshRendererSystem();

    //void HandleMeshRendererAddition(MeshRendererAdditionEvent * inputEvent);
};

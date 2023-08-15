
#pragma once
#include "ECS/System.h"
#include "Utility/ShaderResourceDescription.h"
//#include "DrawGraphNode.h"

template <typename T>
class GraphNode;

namespace ECS
{
    namespace Components
    {
        class MeshRenderer;
        class Transform;
    }
}

class MeshRendererAdditionEvent;

class MeshRendererSystem : public System
{
private:

    uint32_t idCounter = 0;
    uint32_t GenerateId();

    GlobalResourceAllocationConfig allocConfig;
    GlobalResourceSharingConfig resourceSharingConfig;

    SetWrapper * transformSetWrapper;
    std::vector<ShaderBindingDescription *> resDescriptionList;
    std::map<ECS::Components::Transform *, ShaderBindingDescription *> transformToBindDescMap;

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

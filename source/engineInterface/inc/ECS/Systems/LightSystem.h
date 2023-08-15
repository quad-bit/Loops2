#pragma once
#include "ECS/System.h"
#include <glm/glm.hpp>
#include <Utility/ShaderResourceDescription.h>
//#include "DrawGraphNode.h"
#include <ECS/Events/MeshAdditionEvent.h>
#include <ECS/Events/ComponentAdditionEvent.h>

#if (RENDERING_API == VULKAN)
class VulkanInterface;
#elif (RENDERING_API == DX)
#endif

template <typename T>
class GraphNode;

namespace ECS
{
    template <typename T>
    class ComponentHandle;

    namespace Components
    {
        class Light;
        class Camera;
        class Material;
        class Transform;
    }
}

class LightSystem : public System
{
private:
    std::vector<ECS::Components::Light *> lightlist;
    std::map<ECS::Components::Light *, ECS::Components::Camera *> lightToCamList;
    std::vector<ShaderBindingDescription *> resDescriptionList;
    GlobalResourceAllocationConfig lightUniformAllocConfig, shadowMapUniformAllocConfig;
    GlobalResourceSharingConfig lightBufferSharingConfig;

    uint32_t idCounter = 0;
    uint32_t GeneratedLightId();

    SetWrapper * lightSetWrapper;

    //std::vector<GraphNode<DrawGraphNode> *> lightGraphNodeList;
    //std::map<DrawGraphNode *, ShaderBindingDescription *> nodeToDescriptionMap;
    std::map<ECS::Components::Light *, ShaderBindingDescription *> lightToDescriptionMap;

    size_t memoryAlignedUniformSize;
    
    ECS::Components::Material * shadowMappingMat = nullptr;
    System * cameraSystem;

    void CreateLightUniformDescription(ShaderBindingDescription * desc, ECS::Components::Light * light);
    void CreateLightUniformBuffer(ShaderBindingDescription * desc, ECS::Components::Light * light, ECS::Components::Camera * cam);
    ECS::Components::Camera * CreateLightCamera(ECS::Components::Transform * transform);
    void CreateShadowMap(ShaderBindingDescription * desc);

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;

    //void HandleLightAddition(LightAdditionEvent * lightAdditionEvent);
    //void HandleMeshAddition(MeshToMatAdditionEvent *  meshAdditionEvent);
    //void HandleRendererAddition(MeshRendererAdditionEvent *  rendererAdditionEvent);
    //void HandleDepthPrepassCreation(DepthPassAttachmentCreationEvent * evt);
    void AssignCameraSystem(System * camSystem);

    LightSystem();
    virtual ~LightSystem();
};

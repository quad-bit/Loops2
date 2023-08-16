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

namespace Core
{
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
}

class LightSystem : public System
{
private:
    std::vector<Core::ECS::Components::Light *> lightlist;
    std::map<Core::ECS::Components::Light *, Core::ECS::Components::Camera *> lightToCamList;
    std::vector<ShaderBindingDescription *> resDescriptionList;
    GlobalResourceAllocationConfig lightUniformAllocConfig, shadowMapUniformAllocConfig;
    GlobalResourceSharingConfig lightBufferSharingConfig;

    uint32_t idCounter = 0;
    uint32_t GeneratedLightId();

    SetWrapper * lightSetWrapper;

    //std::vector<GraphNode<DrawGraphNode> *> lightGraphNodeList;
    //std::map<DrawGraphNode *, ShaderBindingDescription *> nodeToDescriptionMap;
    std::map<Core::ECS::Components::Light *, ShaderBindingDescription *> lightToDescriptionMap;

    size_t memoryAlignedUniformSize;
    
    Core::ECS::Components::Material * shadowMappingMat = nullptr;
    System * cameraSystem;

    void CreateLightUniformDescription(ShaderBindingDescription * desc, Core::ECS::Components::Light * light);
    void CreateLightUniformBuffer(ShaderBindingDescription * desc, Core::ECS::Components::Light * light, Core::ECS::Components::Camera * cam);
    Core::ECS::Components::Camera * CreateLightCamera(Core::ECS::Components::Transform * transform);
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

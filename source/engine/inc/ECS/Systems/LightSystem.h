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

class LightSystem : public Core::ECS::System
{
private:
    std::vector<Core::ECS::Components::Light *> lightlist;
    std::map<Core::ECS::Components::Light *, Core::ECS::Components::Camera *> lightToCamList;
    std::vector<Core::Utility::DescriptorSetDescription> resDescriptionList;
    Core::Utility::GlobalResourceAllocationConfig lightUniformAllocConfig, shadowMapUniformAllocConfig;
    Core::Utility::GlobalResourceSharingConfig lightBufferSharingConfig;

    uint32_t idCounter = 0;
    uint32_t GeneratedLightId();

    Core::Wrappers::SetWrapper * lightSetWrapper;

    //std::vector<GraphNode<DrawGraphNode> *> lightGraphNodeList;
    //std::map<DrawGraphNode *, ShaderBindingDescription *> nodeToDescriptionMap;
    std::map<Core::ECS::Components::Light *, Core::Utility::DescriptorSetDescription> lightToDescriptionMap;

    size_t memoryAlignedUniformSize;
    
    Core::ECS::Components::Material * shadowMappingMat = nullptr;
    System * cameraSystem;

    void CreateLightUniformDescription(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light);
    void CreateLightUniformBuffer(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light, Core::ECS::Components::Camera * cam);
    Core::ECS::Components::Camera * CreateLightCamera(Core::ECS::Components::Transform * transform);
    void CreateShadowMap(Core::Utility::ShaderBindingDescription * desc);

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;

    void HandleLightAddition(Core::ECS::Events::LightAdditionEvent * lightAdditionEvent);
    //void HandleMeshAddition(MeshToMatAdditionEvent *  meshAdditionEvent);
    //void HandleRendererAddition(MeshRendererAdditionEvent *  rendererAdditionEvent);
    //void HandleDepthPrepassCreation(DepthPassAttachmentCreationEvent * evt);
    void AssignCameraSystem(Core::ECS::System * camSystem);

    LightSystem();
    virtual ~LightSystem();
};

#pragma once
#include "System.h"
#include <glm\glm.hpp>
#include "ShaderResourceDescription.h"
#include "DrawGraphNode.h"
#include "MeshAdditionEvent.h"
#include "ComponentAdditionEvent.h"

#if (RENDERING_API == VULKAN)
class VulkanInterface;
#elif (RENDERING_API == DX)

#endif
template <typename T>
class GraphNode;

template <typename T>
class ComponentHandle;

class Light;
class Camera;
class Material;
class Transform;

class LightSystem : public System
{
private:
    std::vector<Light *> lightlist;
    std::map<Light *, Camera *> lightToCamList;
    std::vector<ShaderBindingDescription *> resDescriptionList;
    GlobalResourceAllocationConfig lightUniformAllocConfig, shadowMapUniformAllocConfig;
    GlobalResourceSharingConfig lightBufferSharingConfig;

    uint32_t idCounter = 0;
    uint32_t GeneratedLightId();

    SetWrapper * lightSetWrapper;

    std::vector<GraphNode<DrawGraphNode> *> lightGraphNodeList;
    std::map<DrawGraphNode *, ShaderBindingDescription *> nodeToDescriptionMap;
    std::map<Light *, ShaderBindingDescription *> lightToDescriptionMap;

    size_t memoryAlignedUniformSize;
    
    Material * shadowMappingMat = nullptr;
    System * cameraSystem;

    void CreateLightUniformDescription(ShaderBindingDescription * desc, Light * light);
    void CreateLightUniformBuffer(ShaderBindingDescription * desc, Light * light, Camera * cam);
    Camera * CreateLightCamera(Transform * transform);
    void CreateShadowMap(ShaderBindingDescription * desc);

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;

    void HandleLightAddition(LightAdditionEvent * lightAdditionEvent);
    void HandleMeshAddition(MeshToMatAdditionEvent *  meshAdditionEvent);
    void HandleRendererAddition(MeshRendererAdditionEvent *  rendererAdditionEvent);
    void HandleDepthPrepassCreation(DepthPassAttachmentCreationEvent * evt);
    void AssignCameraSystem(System * camSystem);

    LightSystem();
    virtual ~LightSystem();
};

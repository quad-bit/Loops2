#pragma once
#include "ECS/System.h"
#include <glm/glm.hpp>
#include <Utility/ShaderResourceDescription.h>
//#include "DrawGraphNode.h"
#include <ECS/Events/MeshAdditionEvent.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <RenderData.h>
#include <array>

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

struct PointLightUniform
{
    glm::vec3 lightPos;
    float lightRadius;
    glm::vec4 lightColor;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

#define MAX_POINT_LIGHTS 20

class LightSystem : public Core::ECS::System
{
private:
    std::vector<Core::ECS::Components::Light *> lightlist;
    std::vector<Core::Utility::DescriptorSetInfo> pointLightDescriptorInfoList;
    Core::Utility::GlobalResourceAllocationConfig pointLightUniformAllocConfig;
    Core::Utility::GlobalResourceSharingConfig pointLightBufferSharingConfig;

    uint32_t idCounter = 0;
    uint32_t GeneratedLightId();

    Core::Wrappers::SetWrapper * lightSetWrapper;

    std::map<Core::ECS::Components::Light *, Core::Utility::DescriptorSetInfo> lightToDescriptionMap;

    size_t pointUniformSize;
    size_t memoryAlignedPointUniformSize;
    
    void CreateLightUniformDescription(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light);
    void CreateLightUniformBuffer(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light, Core::ECS::Components::Camera * cam);

    Core::Utility::LightData& m_lightData;

    uint32_t m_pointLightCount = 0;
    std::vector<std::array<PointLightUniform, MAX_POINT_LIGHTS>> m_pointUniformListPerFrame;

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo) override;

    void HandleLightAddition(Core::ECS::Events::LightAdditionEvent * lightAdditionEvent);

    LightSystem(Core::Utility::LightData& lightData);
    virtual ~LightSystem();
};

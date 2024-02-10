#pragma once
#include "ECS/System.h"
#include <glm/glm.hpp>
#include <ECS/Events/MeshAdditionEvent.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <RenderData.h>

class VulkanInterface;

namespace Core
{
    namespace ECS
    {
        template <typename T>
        class ComponentHandle;

        namespace Components
        {
            class Material;
            class Transform;
        }
    }
}

class MaterialSystem : public Core::ECS::System
{
private:
    std::vector<Core::ECS::Components::Material*> m_materialList;
    std::map<uint32_t, std::vector<uint32_t>> m_materialDescriptorList;

    std::vector<Core::Utility::MaterialData>& m_materialData;
    Core::Utility::MaterialData& m_skyboxData;

    Core::ECS::Components::Material* m_skyboxMaterial;
    std::vector<uint32_t> m_skyboxDescriptorIds;

public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo) override;

    void HandleMaterialAddition(Core::ECS::Events::MaterialAdditionEvent* materialAdditionEvent);

    /// <summary>
    /// Create descriptor sets
    /// </summary>
    /// <param name="materialCreationEvent"></param>
    void HandleMaterialCreation(Core::ECS::Events::MaterialCreationEvent* materialCreationEvent);

    MaterialSystem(std::vector<Core::Utility::MaterialData>& materialData, Core::Utility::MaterialData& skyboxData);
    virtual ~MaterialSystem();
};

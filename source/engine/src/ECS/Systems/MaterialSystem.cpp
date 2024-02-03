#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <ECS/Components/Transform.h>
#include <ECS/World.h>
#include <RendererSettings.h>
#include "ECS/Systems/MaterialSystem.h"

#include "resourceManagement/UniformFactory.h"

const uint32_t g_numDescriptorSets = 1;
void MaterialSystem::Init()
{
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<MaterialSystem, Core::ECS::Events::MaterialAdditionEvent>(this, &MaterialSystem::HandleMaterialAddition);
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<MaterialSystem, Core::ECS::Events::MaterialCreationEvent>(this, &MaterialSystem::HandleMaterialCreation);
}

void MaterialSystem::DeInit()
{

}

void MaterialSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    m_materialData.clear();

    const uint32_t descriptorIndex = 0; // as for now we are creating one descriptor for all frames.

    for (auto& mat : m_materialList)
    {
        Core::Utility::MaterialData data{};

        // opaqueUnlit doesnt has material (set 3) descriptor
        if(m_materialDescriptorList.find(mat->componentId) != m_materialDescriptorList.end())
            data.m_descriptorSetId = m_materialDescriptorList[mat->componentId].at(descriptorIndex);

        data.m_effectType = mat->m_effectType.first;
        data.m_techniqueName = mat->m_effectType.second;
        data.m_materialComponentId = mat->componentId;

        //m_materialData.insert({ mat->componentId, data });
        m_materialData.push_back(data);
    }
}

void MaterialSystem::HandleMaterialAddition(Core::ECS::Events::MaterialAdditionEvent* materialAdditionEvent)
{

}

void MaterialSystem::HandleMaterialCreation(Core::ECS::Events::MaterialCreationEvent* materialCreationEvent)
{
    if (materialCreationEvent->material->m_baseColorTextureId.has_value() &&
        materialCreationEvent->material->m_baseColorSamplerId.has_value() &&
        !materialCreationEvent->material->m_normalTextureId.has_value())
    {
        Core::Utility::ImageBindingInfo imageInfo{};
        imageInfo.imageId.push_back(materialCreationEvent->material->m_baseColorTextureId.value());
        imageInfo.samplerId = materialCreationEvent->material->m_baseColorSamplerId.value();

        Core::Utility::DescriptorSetBindingInfo bindingDescription;
        bindingDescription.m_bindingName = "diffuseSampler";
        bindingDescription.m_bindingNumber = 0;
        bindingDescription.m_numElements = 0;
        bindingDescription.m_resourceType = Core::Enums::DescriptorType::COMBINED_IMAGE_SAMPLER;
        bindingDescription.m_bindingInfo = imageInfo;

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_numBindings = 1;
        setDescription.m_setNumber = (uint32_t)Core::Enums::ResourceSets::MATERIAL;
        setDescription.m_setBindings.push_back(bindingDescription);

        auto setWrapper = UniFactAlias::GetInstance()->GetSetWrapper(setDescription);
        UniFactAlias::GetInstance()->AllocateDescriptorSets(setWrapper, setDescription, g_numDescriptorSets);

        //materialCreationEvent->material->m_descriptorSetIds = setDescription.m_descriptorSetIds;
        m_materialDescriptorList[materialCreationEvent->material->componentId] = setDescription.m_descriptorSetIds;
    }

    m_materialList.push_back(materialCreationEvent->material);
}

MaterialSystem::MaterialSystem(std::vector<Core::Utility::MaterialData>& materialData) : m_materialData(materialData)
{
    signature.AddComponent<Core::ECS::Components::Material>();
    signature.AddComponent<Core::ECS::Components::Transform>();
}

MaterialSystem::~MaterialSystem()
{
}


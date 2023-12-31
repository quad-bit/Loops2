#include "ECS/Systems/MeshRendererSystem.h"
#include <ECS/Components/Mesh.h>
#include <ECS/Components/MeshRenderer.h>
#include <ECS/Components/Material.h>
#include <ECS/Components/Transform.h>
#include <ECS/World.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <Utility/ResourceAllocationHelper.h>
#include <plog/Log.h> 

#include <resourceManagement/UniformFactory.h>
//#include "DrawGraphManager.h"
//#include "Graph.h"
//#include "DrawCommandBuffer.h"
//#include "DrawGraphNode.h"

uint32_t MeshRendererSystem::GenerateId()
{
    return idCounter++;
}

void MeshRendererSystem::Init()
{
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<MeshRendererSystem, Core::ECS::Events::MeshRendererAdditionEvent>(this, &MeshRendererSystem::HandleMeshRendererAddition);

    allocConfig.numDescriptorSets = Core::Settings::m_maxFramesInFlight;
    allocConfig.numMemories = 1;
    allocConfig.numResources = 1;

    resourceSharingConfig.maxUniformPerResource = 5;
    resourceSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(Core::ECS::Components::TransformUniform);
    memoryAlignedUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);
}

void MeshRendererSystem::DeInit()
{
}

void MeshRendererSystem::Update(float dt)
{
    m_transformDataList.clear();
    m_perEffectTransformData.clear();
    for (auto & entity : registeredEntities)
    {
        Core::ECS::ComponentHandle<Core::ECS::Components::MeshRenderer> * renderer;
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform> * transform;
        worldObj->Unpack(entity, &renderer, &transform);
        
        Core::ECS::Components::Transform * transformObj = transform->GetComponent();
        Core::ECS::Components::TransformUniform obj = {};
        obj.modelMat = transformObj->GetGlobalModelMatrix();

        Core::Utility::DescriptorSetInfo desc = transformToBindDescMap[transformObj];
        UniFactAlias::GetInstance()->UploadDataToBuffers(std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).bufferIdList[0],
            sizeof(Core::ECS::Components::TransformUniform), memoryAlignedUniformSize,
            &obj, std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[Core::Settings::m_currentFrameInFlight], false);

        Core::Utility::TransformData data;
        data.m_descriptorSetId = desc.m_descriptorSetIds[Core::Settings::m_currentFrameInFlight];
        data.m_position = transformObj->GetGlobalPosition();
        data.m_renderLayers = entity->GetEntityLayers();
        
        for (uint32_t i = 0; i < renderer->GetComponent()->geometry->vertexBufferCount; i++)
            data.m_vertexBufferId.push_back(renderer->GetComponent()->geometry->vertexBuffersIds[i]);
        
        if(renderer->GetComponent()->geometry->indicies.size() > 0)
            data.m_indexBufferId = renderer->GetComponent()->geometry->indexBufferId;

        m_transformDataList.push_back(data);

        if (renderer->GetComponent()->material->m_effectName.has_value())
        {
            const std::string effectName = renderer->GetComponent()->material->m_effectName.value();
            if(m_perEffectTransformData.find(effectName) == m_perEffectTransformData.end())
            {
                m_perEffectTransformData.insert(std::pair<std::string, std::vector<Core::Utility::TransformData>>(effectName, std::vector{ data }));
            }
            else
            {
                m_perEffectTransformData[effectName].push_back(data);
            }
        }
    }
}

MeshRendererSystem::MeshRendererSystem(std::vector<Core::Utility::TransformData>& transformData, std::map<std::string,std::vector<Core::Utility::TransformData>>& perEffectTrfData) : 
    m_transformDataList(transformData), m_perEffectTransformData(perEffectTrfData)
{
    signature.AddComponent<Core::ECS::Components::MeshRenderer>();
    signature.AddComponent<Core::ECS::Components::Mesh>();
    signature.AddComponent<Core::ECS::Components::Material>();
    signature.AddComponent<Core::ECS::Components::Transform>();
}

MeshRendererSystem::~MeshRendererSystem()
{
}

void MeshRendererSystem::HandleMeshRendererAddition(Core::ECS::Events::MeshRendererAdditionEvent * inputEvent)
{
    // Create descriptor for transform set
    inputEvent->renderer->componentId = GenerateId();

    //size_t uniformSize = sizeof(Core::ECS::Components::TransformUniform);

    Core::Utility::BufferBindingInfo bufInfo{};
    bufInfo.info.allocationConfig = allocConfig;
    bufInfo.info.dataSizePerDescriptor = sizeof(Core::ECS::Components::TransformUniform);
    bufInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    bufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
    bufInfo.info.sharingConfig = resourceSharingConfig;
    bufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);

    Core::Utility::DescriptorSetBindingInfo bindingDescription;
    bindingDescription.m_bindingName = "Transform";
    bindingDescription.m_bindingNumber = 0;
    bindingDescription.m_numElements = 1;
    bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
    bindingDescription.m_bindingInfo = bufInfo;

    Core::Utility::DescriptorSetInfo setDescription;
    setDescription.m_numBindings = 1;
    setDescription.m_setNumber = (uint32_t)Core::Enums::ResourceSets::TRANSFORM;
    setDescription.m_setBindings.push_back(bindingDescription);

    // Check if it can be fit into an existing buffer
    if (Core::Utility::IsNewAllocationRequired(resourceSharingConfig))
    {
        // True : Allocate new buffer
        transformSetWrapper = UniFactAlias::GetInstance()->AllocateSetResources(setDescription);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0] = std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferIdList[0];
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferMemoryId[0] = std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferMemoryId[0];
    }

    resourceSharingConfig.allocatedUniformCount += 1;

    Core::ECS::Components::Transform * transform = inputEvent->renderer->transform;

    Core::ECS::Components::TransformUniform obj = {};
    obj.modelMat = transform->GetGlobalModelMatrix();

    //upload data to buffers
    for (uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        uint32_t bufferId = 0;
        if (allocConfig.numDescriptorSets != std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.size())
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0];
        else
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[i];

        UniFactAlias::GetInstance()->UploadDataToBuffers(bufferId,
            sizeof(Core::ECS::Components::TransformUniform), memoryAlignedUniformSize, &obj,
            std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[i], false);
    }

    // allocate descriptor sets
    UniFactAlias::GetInstance()->AllocateDescriptorSets(transformSetWrapper, setDescription, allocConfig.numDescriptorSets);
    resDescriptionList.push_back(setDescription);

    transformToBindDescMap.insert(std::pair<Core::ECS::Components::Transform *, Core::Utility::DescriptorSetInfo>(
    {transform, setDescription}));
}


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

#include <shaderResource/UniformFactory.h>
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
    //for (auto & entity : registeredEntities)
    //{
    //    ComponentHandle<MeshRenderer> * renderer;
    //    ComponentHandle<Transform> * transform;
    //    worldObj->Unpack(entity, &renderer, &transform);
    //    
    //    Transform * transformObj = transform->GetComponent();
    //    TransformUniform obj = {};
    //    obj.modelMat = transformObj->GetGlobalModelMatrix();

    //    ShaderBindingDescription * desc = transformToBindDescMap[transformObj];
    //    UniformFactory::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0], memoryAlignedUniformSize, memoryAlignedUniformSize,
    //        &obj, desc->bufferBindingInfo.info.offsetsForEachDescriptor[Settings::currentFrameInFlight], false);

    //}
}

MeshRendererSystem::MeshRendererSystem()
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

    Core::Utility::DescriptorSetBindingDescription bindingDescription;
    bindingDescription.m_bindingName = "Transform";
    bindingDescription.m_bindingNumber = 0;
    bindingDescription.m_numElements = 1;
    bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
    bindingDescription.m_bindingInfo = bufInfo;

    Core::Utility::DescriptorSetDescription setDescription;
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

    transformToBindDescMap.insert(std::pair<Core::ECS::Components::Transform *, Core::Utility::DescriptorSetDescription>(
    {transform, setDescription}));
}


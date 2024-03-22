#include "ECS/Systems/BoundSystem.h"
#include <ECS/Components/Material.h>
#include <ECS/Components/Transform.h>
#include <ECS/Components/Bounds.h>
#include <ECS/World.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <Utility/ResourceAllocationHelper.h>
#include <plog/Log.h> 

#include <resourceManagement/ShaderResourceManager.h>

void BoundSystem::Init()
{
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<BoundSystem, Core::ECS::Events::BoundAdditionEvent>(this, &BoundSystem::HandleBoundAddition);

    allocConfig.numDescriptorSets = Core::Settings::m_maxFramesInFlight;
    allocConfig.numMemories = 1;
    allocConfig.numResources = 1;

    resourceSharingConfig.maxUniformPerResource = 6; // keep it a multiple of numdesccriptorSets of allocConfig
    resourceSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(Core::ECS::Components::BoundUniform);
    memoryAlignedUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);
}

void BoundSystem::DeInit()
{
}

void BoundSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    m_boundDataList.clear();
    for (auto & entity : registeredEntities)
    {
        //Core::ECS::ComponentHandle<Core::ECS::Components::Transform>* transform;
        Core::ECS::ComponentHandle<Core::ECS::Components::Bound> * bound;

        worldObj->Unpack(entity, &bound);

        bound->GetComponent()->GetBoundsCategory()->UpdateTransform();

        Core::ECS::Components::BoundUniform uniform{};
        uniform.m_modelMat = bound->GetComponent()->GetBoundsCategory()->m_transform;
        uniform.m_parentType = (uint32_t)bound->GetComponent()->m_parentType;

        Core::Utility::DescriptorSetInfo desc{};

        switch (bound->GetComponent()->m_parentType)
        {
        case Core::ECS::COMPONENT_TYPE::CAMERA:
            desc = cameraBoundToBindDescMap[bound->GetComponent()];
            break;
        case Core::ECS::COMPONENT_TYPE::LIGHT:
            desc = lightBoundToBindDescMap[bound->GetComponent()];
            break;
        case Core::ECS::COMPONENT_TYPE::MESH_RENDERER:
            desc = transformBoundToBindDescMap[bound->GetComponent()];
            break;
        default:
            ASSERT_MSG_DEBUG(0, "invalid case");
            break;
        }

        ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).bufferIdList[0],
            sizeof(Core::ECS::Components::BoundUniform), memoryAlignedUniformSize,
            &uniform, std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[frameInfo.m_frameInFlightIndex], false);

        Core::Utility::BoundData data{};
        data.m_descriptorSetId = desc.m_descriptorSetIds[frameInfo.m_frameInFlightIndex];
        data.m_indexBufferId = bound->GetComponent()->m_indexBufferId;
        data.m_indexCount = (uint32_t)bound->GetComponent()->m_indicies.size();
        data.m_modelMat = bound->GetComponent()->GetBoundsCategory()->m_transform;
        data.m_positionBufferId = bound->GetComponent()->m_vertexBufferId;
        data.m_vertexCount = bound->GetComponent()->m_positions.size();

        m_boundDataList.push_back(data);
    }
}

BoundSystem::BoundSystem(std::vector<Core::Utility::BoundData>& boundDataList) : m_boundDataList(boundDataList)
{
    signature.AddComponent<Core::ECS::Components::Bound>();
}

BoundSystem::~BoundSystem()
{
}

void BoundSystem::HandleBoundAddition(Core::ECS::Events::BoundAdditionEvent * inputEvent)
{
    Core::Utility::BufferBindingInfo bufInfo{};
    bufInfo.info.allocationConfig = allocConfig;
    bufInfo.info.dataSizePerDescriptor = sizeof(Core::ECS::Components::BoundUniform);
    bufInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    bufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
    bufInfo.info.sharingConfig = resourceSharingConfig;
    bufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);

    Core::Utility::DescriptorSetBindingInfo bindingDescription;
    bindingDescription.m_bindingName = "Transform";
    bindingDescription.m_bindingNumber = 0;
    bindingDescription.m_numElements = 2;
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
        boundSetWrapper = ShdrResMgrAlias::GetInstance()->AllocateSetResources(setDescription);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.push_back(std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferIdList[0]);
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferMemoryId.push_back(std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferMemoryId[0]);
    }

    resourceSharingConfig.allocatedUniformCount += 1;

    Core::ECS::Components::Bound * bound = inputEvent->bounds;

    Core::ECS::Components::BoundUniform obj = {};
    obj.m_modelMat = bound->GetBoundsCategory()->m_transform;
    obj.m_parentType = (uint32_t)inputEvent->boundParentType;

    //upload data to buffers
    for (uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        uint32_t bufferId = 0;
        if (allocConfig.numDescriptorSets != std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.size())
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0];
        else
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[i];

        auto dataSize = sizeof(Core::ECS::Components::BoundUniform);
        auto& bufferLayoutInfo = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).info;
        auto offset = bufferLayoutInfo.offsetsForEachDescriptor[i];

        ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(bufferId, dataSize, memoryAlignedUniformSize, &obj, offset, false);
    }

    // allocate descriptor sets
    ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(boundSetWrapper, setDescription, allocConfig.numDescriptorSets);
    resDescriptionList.push_back(setDescription);

    switch (inputEvent->boundParentType)
    {
    case Core::ECS::COMPONENT_TYPE::CAMERA:
        bound->m_parentType = Core::ECS::COMPONENT_TYPE::CAMERA;
        cameraBoundToBindDescMap.insert(std::pair<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo>(
            { bound, setDescription }));
        break;
    case Core::ECS::COMPONENT_TYPE::LIGHT:
        bound->m_parentType = Core::ECS::COMPONENT_TYPE::LIGHT;
        lightBoundToBindDescMap.insert(std::pair<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo>(
            { bound, setDescription }));
        break;
    case Core::ECS::COMPONENT_TYPE::MESH_RENDERER:
        bound->m_parentType = Core::ECS::COMPONENT_TYPE::MESH_RENDERER;
        transformBoundToBindDescMap.insert(std::pair<Core::ECS::Components::Bound*, Core::Utility::DescriptorSetInfo>(
            { bound, setDescription }));
        break;
    default:
        ASSERT_MSG_DEBUG(0, "invalid case");
        break;
    }
}


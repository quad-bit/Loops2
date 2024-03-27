#include "ECS/Systems/LightSystem.h"
#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <ECS/Components/Light.h>
#include <Utility/ResourceAllocationHelper.h>
#include <ECS/Components/Transform.h>
#include <ECS/World.h>
#include <ECS/Components/Mesh.h>
#include <ECS/Components/Camera.h>
#include <ECS/Components/Bounds.h>
#include <RendererSettings.h>
#include "ECS/Systems/CameraSystem.h"

//#include "MaterialFactory.h"
//#include "DrawGraphManager.h"
#include "resourceManagement/ShaderResourceManager.h"
//#include "GraphicsPipelineManager.h"




uint32_t LightSystem::GeneratedLightId()
{
    return idCounter++;
}

void LightSystem::Init()
{
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<LightSystem, Core::ECS::Events::LightAdditionEvent>(this, &LightSystem::HandleLightAddition);

    m_pointUniformListPerFrame.resize(Core::Settings::m_maxFramesInFlight);

    pointLightUniformAllocConfig.numDescriptorSets = Core::Settings::m_maxFramesInFlight;
    pointLightUniformAllocConfig.numMemories = 1;
    pointLightUniformAllocConfig.numResources = 1;

    pointLightBufferSharingConfig.maxUniformPerResource = MAX_POINT_LIGHTS; // keep it a multiple of numdesccriptorSets of allocConfig
    pointLightBufferSharingConfig.allocatedUniformCount = 0;

    pointUniformSize = sizeof(PointLightUniform) * MAX_POINT_LIGHTS;
    memoryAlignedPointUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(pointUniformSize);
}

void LightSystem::DeInit()
{

}

void LightSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    m_lightData.m_pointLights.clear();
    uint32_t index = 0;
    for (auto & entity : registeredEntities)
    {
        Core::ECS::ComponentHandle<Core::ECS::Components::Light>* lightHandle;
        Core::ECS::ComponentHandle<Core::ECS::Components::Transform>* transformHandle;
        Core::ECS::ComponentHandle<Core::ECS::Components::Bound> * boundHandle;
        worldObj->Unpack(entity, &lightHandle, &transformHandle, &boundHandle);

        auto maxPos = boundHandle->GetComponent()->GetBoundsCategory()->m_maxPos;
        auto minPos = boundHandle->GetComponent()->GetBoundsCategory()->m_minPos;

        Core::ECS::Components::Light * light = lightHandle->GetComponent();
        if (light->GetLightType() == Core::ECS::Components::LightType::Point)
        {
            PointLightUniform uniformObj{};
            uniformObj.ambient = light->GetLightCategory()->m_ambient;
            uniformObj.diffuse = light->GetLightCategory()->m_diffuse;
            uniformObj.lightColor = light->GetLightCategory()->m_color;
            uniformObj.lightPos = transformHandle->GetComponent()->GetGlobalPosition();
            uniformObj.lightRadius = ((Core::ECS::Components::PointLight*)light->GetLightCategory())->m_lightRadius;
            uniformObj.specular = light->GetLightCategory()->m_specular;

            m_pointUniformListPerFrame[frameInfo.m_frameInFlightIndex][index++] = uniformObj;

            Core::Utility::PointLightData data{ uniformObj.lightPos , uniformObj.lightRadius, minPos, maxPos };
            m_lightData.m_pointLights.push_back(data);
        }
    }

    // as we have all the light data in a single buffer hence both descriptor set id are stored in a single DescriptorSetInfo
    const Core::Utility::DescriptorSetInfo& desc = pointLightDescriptorInfoList[0];

    //upload data to buffers
    {
        ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).bufferIdList[0],
            pointUniformSize, memoryAlignedPointUniformSize, m_pointUniformListPerFrame[frameInfo.m_frameInFlightIndex].data(),
            std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[frameInfo.m_frameInFlightIndex], false);
    }

    m_lightData.m_descriptorSetId = desc.m_descriptorSetIds[frameInfo.m_frameInFlightIndex];
}

void LightSystem::HandleLightAddition(Core::ECS::Events::LightAdditionEvent * lightAdditionEvent)
{
    Core::ECS::Components::Light* light = lightAdditionEvent->light;
    Core::ECS::Components::Transform* transform = lightAdditionEvent->transform;

    lightlist.push_back(light);
    light->componentId = GeneratedLightId();

    if (light->GetLightType() == Core::ECS::Components::LightType::Point)
    {

        auto GetPointLightBinding = [this]() -> Core::Utility::DescriptorSetBindingInfo
        {
            Core::Utility::BufferBindingInfo bufInfo{};
            bufInfo.info.allocationConfig = pointLightUniformAllocConfig;
            bufInfo.info.dataSizePerDescriptor = pointUniformSize;
            bufInfo.info.dataSizePerDescriptorAligned = memoryAlignedPointUniformSize;
            bufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedPointUniformSize, pointLightUniformAllocConfig, pointLightBufferSharingConfig);
            bufInfo.info.sharingConfig = pointLightBufferSharingConfig;
            bufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedPointUniformSize, pointLightUniformAllocConfig, pointLightBufferSharingConfig);

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "lightUniform";
            bindingDescription.m_bindingNumber = 0;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = bufInfo;

            return bindingDescription;
        };

        auto GetLightClusterBinding = [this]() -> Core::Utility::DescriptorSetBindingInfo
        {
            uint32_t numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

            Core::Utility::GlobalResourceAllocationConfig allocConfig{};
            allocConfig.numDescriptorSets = numDescriptorSetsPerUniform;
            allocConfig.numMemories = 2; // 2 memory
            allocConfig.numResources = 2; //2 buff

            Core::Utility::GlobalResourceSharingConfig resourceSharingConfig{};
            resourceSharingConfig.maxUniformPerResource = 1;
            resourceSharingConfig.allocatedUniformCount = 0;

            size_t clusterBoundDataSize = sizeof(ClusterInfo);
            uint32_t memoryAlignedClusterBoundsDataSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(clusterBoundDataSize);

            Core::Utility::BufferBindingInfo clusterBoundBufInfo{};
            clusterBoundBufInfo.info.allocationConfig = allocConfig;
            clusterBoundBufInfo.info.dataSizePerDescriptor = clusterBoundDataSize;
            clusterBoundBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedClusterBoundsDataSize;
            clusterBoundBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.info.sharingConfig = resourceSharingConfig;
            clusterBoundBufInfo.info.totalSize = memoryAlignedClusterBoundsDataSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.bufferIdList.push_back(m_lightData.m_lightClusterBufferIds[0]);
            clusterBoundBufInfo.bufferIdList.push_back(m_lightData.m_lightClusterBufferIds[1]);

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "ClusterInfo";
            bindingDescription.m_bindingNumber = 1;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = clusterBoundBufInfo;

            return bindingDescription;
        };

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_numBindings = 2;
        setDescription.m_setNumber = (uint32_t)Core::Enums::ResourceSets::LIGHT;
        setDescription.m_setBindings.push_back(GetPointLightBinding());
        setDescription.m_setBindings.push_back(GetLightClusterBinding());

        // Check if it can be fit into an existing buffer,
        // in light system 20 + 20 (2 frames) point light uniforms are stuffed into one buffer
        if (Core::Utility::IsNewAllocationRequired(pointLightBufferSharingConfig))
        {
            // True : Allocate new buffer
            lightSetWrapper = ShdrResMgrAlias::GetInstance()->AllocateSetResources(setDescription);
        }
        else
        {
            // False : Assign the buffer id to this shaderResourceDescription
            std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.push_back(std::get<Core::Utility::BufferBindingInfo>(pointLightDescriptorInfoList[pointLightDescriptorInfoList.size() - 1].m_setBindings[0].m_bindingInfo).bufferIdList[0]);
            std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferMemoryId.push_back(std::get<Core::Utility::BufferBindingInfo>(pointLightDescriptorInfoList[pointLightDescriptorInfoList.size() - 1].m_setBindings[0].m_bindingInfo).bufferMemoryId[0]);
        }

        //pointLightBufferSharingConfig.allocatedUniformCount += 1;

        PointLightUniform uniformObj{};
        uniformObj.ambient = light->GetLightCategory()->m_ambient;
        uniformObj.diffuse = light->GetLightCategory()->m_diffuse;
        uniformObj.lightColor = light->GetLightCategory()->m_color;
        uniformObj.lightPos = transform->GetGlobalPosition();
        uniformObj.lightRadius = ((Core::ECS::Components::PointLight*)light->GetLightCategory())->m_lightRadius;
        uniformObj.specular = light->GetLightCategory()->m_specular;

        for(auto& perFrameList : m_pointUniformListPerFrame)
            perFrameList[m_pointLightCount] = uniformObj;

        //upload data to buffers
        for (uint32_t i = 0; i < pointLightUniformAllocConfig.numDescriptorSets; i++)
        {
            uint32_t bufferId = 0;
            if (pointLightUniformAllocConfig.numDescriptorSets != std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.size())
                bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0];
            else
                bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[i];

            auto dataSize = pointUniformSize;
            auto& bufferLayoutInfo = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).info;
            auto offset = bufferLayoutInfo.offsetsForEachDescriptor[i];

            ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(bufferId, dataSize, memoryAlignedPointUniformSize, m_pointUniformListPerFrame.at(i).data(), offset, false);
        }

        if (pointLightDescriptorInfoList.size() == 0)
        {
            ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(lightSetWrapper, setDescription, pointLightUniformAllocConfig.numDescriptorSets);
            pointLightDescriptorInfoList.push_back(setDescription);
        }

        m_pointLightCount++;
        ASSERT_MSG_DEBUG(m_pointLightCount < MAX_POINT_LIGHTS, "Light limit");
        ASSERT_MSG_DEBUG(pointLightDescriptorInfoList.size() < 2, "Light limit");
    }
}

LightSystem::LightSystem(Core::Utility::LightData& lightData) : m_lightData(lightData)
{
    signature.AddComponent<Core::ECS::Components::Light>();
    signature.AddComponent<Core::ECS::Components::Transform>();
    signature.AddComponent<Core::ECS::Components::Bound>();
}

LightSystem::~LightSystem()
{
}


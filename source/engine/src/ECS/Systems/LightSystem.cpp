#include "ECS/Systems/LightSystem.h"
#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <ECS/Components/Light.h>
#include <Utility/ResourceAllocationHelper.h>
#include <ECS/Components/Transform.h>
#include <ECS/World.h>
#include <ECS/Components/Mesh.h>
#include <ECS/Components/Camera.h>
#include <RendererSettings.h>
#include "ECS/Systems/CameraSystem.h"

//#include "MaterialFactory.h"
//#include "DrawGraphManager.h"
#include "resourceManagement/UniformFactory.h"
//#include "GraphicsPipelineManager.h"


uint32_t LightSystem::GeneratedLightId()
{
    return idCounter++;
}

void LightSystem::CreateLightUniformDescription(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light)
{
#if 0
    desc->set = (uint32_t)ResourceSets::LIGHT;
    desc->binding = 0;
    desc->numElements = 8;
    desc->resourceName = "Lights";
    desc->resourceType = DescriptorType::UNIFORM_BUFFER;
    desc->parentType = light->componentType;
    desc->uniformId = light->componentId; 
    desc->bufferBindingInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    desc->bufferBindingInfo.info.dataSizePerDescriptor = sizeof(LightUniform);
    desc->bufferBindingInfo.info.offsetsForEachDescriptor = AllocationUtility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, lightUniformAllocConfig, lightBufferSharingConfig);
    desc->bufferBindingInfo.info.allocationConfig = lightUniformAllocConfig;
    desc->bufferBindingInfo.bufferIdList.resize(lightUniformAllocConfig.numResources);
    desc->bufferBindingInfo.bufferMemoryId.resize(lightUniformAllocConfig.numMemories);
#endif
}

void LightSystem::CreateLightUniformBuffer(Core::Utility::ShaderBindingDescription * desc, Core::ECS::Components::Light * light, Core::ECS::Components::Camera * cam)
{
#if 0
    // Check if it can be fit into an existing buffer
    if (AllocationUtility::IsNewAllocationRequired(lightBufferSharingConfig))
    {
        size_t totalSize = AllocationUtility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, lightUniformAllocConfig, lightBufferSharingConfig);
        // True : Allocate new buffer, this should be Allocate resource for binding instead of complete set
        //lightSetWrapper = UniformFactory::GetInstance()->AllocateSetResource(desc, &totalSize, 1, AllocationMethod::LAZY);
        UniformFactory::GetInstance()->AllocateBindingResources(lightSetWrapper , desc, totalSize, 0, AllocationMethod::LAZY);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        desc->bufferBindingInfo.bufferIdList[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferIdList[0];
        desc->bufferBindingInfo.bufferMemoryId[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferMemoryId[0];
    }

    resDescriptionList.push_back(desc);
    lightBufferSharingConfig.allocatedUniformCount += 1;

    LightUniform obj = {};
    obj.ambient = Vec3ToVec4_0(light->GetAmbient());
    obj.diffuse = Vec3ToVec4_0(light->GetDiffuse());
    obj.specular = Vec3ToVec4_0(light->GetSpecular());
    obj.lightPos = Vec3ToVec4_0(light->GetTransform()->GetGlobalPosition());
    obj.lightDir = Vec3ToVec4_0(light->GetTransform()->GetForward());
    obj.beamHeight = light->GetBeamHeight();
    obj.beamRadius = light->GetBeamRadius();

    obj.lightSpaceMat = cam->GetProjectionMat() * cam->GetViewMatrix();// *light->GetTransform()->GetGlobalModelMatrix();

    //upload data to buffers
    for (uint32_t i = 0; i < lightUniformAllocConfig.numDescriptorSets; i++)
    {
        UniformFactory::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0], sizeof(LightUniform),
            memoryAlignedUniformSize, &obj, desc->bufferBindingInfo.info.offsetsForEachDescriptor[i], false);
    }
#endif
}

Core::ECS::Components::Camera *  LightSystem::CreateLightCamera(Core::ECS::Components::Transform * transform)
{
    // create a camera 
    Core::ECS::Components::Camera * lightCam = new Core::ECS::Components::Camera(transform, 1024.0f/1024.0f);
    //lightCam->SetProjectionType(CameraType::ORTHOGONAL);
    lightCam->SetFOV(90.0f);
    lightCam->SetNearPlane(2.0f);
    lightCam->SetFarPlane(1000.0f);

    //lightToCamList.push_back(lightCam);

    // create the camera draw node, node has been added to the graph in HandleCameraAddition
    //GraphNode<DrawGraphNode> * cameraNode = ((CameraSystem*)cameraSystem)->HandleCameraAddition(lightCam, RenderPassTag::DepthPass);
    //cameraNode->node->tag = RenderPassTag::DepthPass;

    return lightCam;
}

void LightSystem::CreateShadowMap(Core::Utility::ShaderBindingDescription * desc)
{
#if 0
    SamplerCreateInfo info = {};
    info.minFilter = Filter::FILTER_NEAREST;
    info.magFilter = Filter::FILTER_NEAREST;
    info.addressModeU = SamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.addressModeV = info.addressModeU;
    info.addressModeW = info.addressModeU;
    info.anisotropyEnable = false;
    info.maxAnisotropy = 1.0f;
    info.borderColor = BorderColor::BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    info.unnormalizedCoordinates = false;
    info.compareEnable = false;
    info.compareOp = CompareOp::COMPARE_OP_ALWAYS;
    info.mipmapMode = SamplerMipmapMode::SAMPLER_MIPMAP_MODE_NEAREST;
    info.minLod = 0.0f;
    info.maxLod = 0.0f;
    info.mipLodBias = 0.0f;

    uint32_t samplerId = UniformFactory::GetInstance()->CreateSampler(info);

    std::vector<uint32_t> temp = RendererSettings::depthPrepassImageId;

    desc->set = (uint32_t)ResourceSets::LIGHT;
    desc->binding = 1;
    desc->numElements = 0;
    desc->resourceName = "combined_shadowSampler";
    desc->resourceType = DescriptorType::COMBINED_IMAGE_SAMPLER;
    desc->imageBindingInfo.imageId = RendererSettings::depthPrepassImageId;
    desc->samplerBindingInfo.samplerId.push_back(samplerId);
#endif
}

void LightSystem::Init()
{

    Core::ECS::Events::EventBus::GetInstance()->Subscribe<LightSystem, Core::ECS::Events::LightAdditionEvent>(this, &LightSystem::HandleLightAddition);
    //Core::ECS::Events::EventBus::GetInstance()->Subscribe<LightSystem, Core::ECS::Events::MeshToMatAdditionEvent>(this, &LightSystem::HandleMeshAddition);
    //Core::ECS::Events::EventBus::GetInstance()->Subscribe<LightSystem, Core::ECS::Events::MeshRendererAdditionEvent>(this, &LightSystem::HandleRendererAddition);

    lightUniformAllocConfig.numDescriptorSets = Core::Settings::m_maxFramesInFlight;
    lightUniformAllocConfig.numMemories = 1;
    lightUniformAllocConfig.numResources = 1;

    /*shadowMapUniformAllocConfig.numDescriptorSets = Settings::swapBufferCount;
    shadowMapUniformAllocConfig.numMemories = 1;
    shadowMapUniformAllocConfig.numResources = 1;*/

    lightBufferSharingConfig.maxUniformPerResource = 1;
    lightBufferSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(Core::ECS::Components::LightUniform);
    memoryAlignedUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);

}

void LightSystem::DeInit()
{

}

void LightSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    m_lightDataList.clear();
    for (auto & entity : registeredEntities)
    {
        Core::ECS::ComponentHandle<Core::ECS::Components::Light> * lightHandle;
        worldObj->Unpack(entity, &lightHandle);

        Core::ECS::Components::Light * light = lightHandle->GetComponent();
        Core::ECS::Components::LightUniform obj = {};
        obj.ambient = Vec3ToVec4_0(light->GetAmbient());
        obj.diffuse = Vec3ToVec4_0(light->GetDiffuse());
        obj.specular = Vec3ToVec4_0(light->GetSpecular());
        obj.lightPos = Vec3ToVec4_0(light->GetTransform()->GetGlobalPosition());
        obj.lightDir = Vec3ToVec4_0(light->GetTransform()->GetForward());
        obj.beamHeight = light->GetBeamHeight();
        obj.beamRadius = light->GetBeamRadius();

        Core::ECS::Components::Camera * cam = lightToCamList[light];
        //obj.lightSpaceMat = cam->GetProjectionMat() * cam->GetViewMatrix();// *light->GetTransform()->GetLocalModelMatrix();

        Core::Utility::DescriptorSetInfo desc = lightToDescriptionMap[lightHandle->GetComponent()];

        //upload data to buffers
        {
            // using swapchainIndex as there are 3 descriptorSets because there are three depth images

            // CHECK THE ABOVE COMMENT, WHEN IMPLEMENTING SHADOWS
            UniFactAlias::GetInstance()->UploadDataToBuffers(std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).bufferIdList[0],
                sizeof(Core::ECS::Components::LightUniform), memoryAlignedUniformSize, &obj,
                std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[Core::Settings::m_currentFrameInFlight], false);
        
        }

        Core::Utility::LightData data = {};
        // CHECK THE ABOVE COMMENT, WHEN IMPLEMENTING SHADOWS
        data.m_descriptorSetId = desc.m_descriptorSetIds[frameInfo.m_frameInFlightIndex];
        m_lightDataList.push_back(data);
    }
}

void LightSystem::HandleLightAddition(Core::ECS::Events::LightAdditionEvent * lightAdditionEvent)
{
    Core::ECS::Components::Light* light = lightAdditionEvent->light;

    lightlist.push_back(light);
    light->componentId = GeneratedLightId();
    
    Core::Utility::BufferBindingInfo bufInfo{};
    bufInfo.info.allocationConfig = lightUniformAllocConfig;
    bufInfo.info.dataSizePerDescriptor = sizeof(Core::ECS::Components::LightUniform);
    bufInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    bufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, lightUniformAllocConfig, lightBufferSharingConfig);
    bufInfo.info.sharingConfig = lightBufferSharingConfig;
    bufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, lightUniformAllocConfig, lightBufferSharingConfig);

    Core::Utility::DescriptorSetBindingInfo bindingDescription;
    bindingDescription.m_bindingName = "Lights";
    bindingDescription.m_bindingNumber = 0;
    bindingDescription.m_numElements = 7;
    bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
    bindingDescription.m_bindingInfo = bufInfo;

    Core::Utility::DescriptorSetInfo setDescription;
    setDescription.m_numBindings = 1;
    setDescription.m_setNumber = (uint32_t)Core::Enums::ResourceSets::LIGHT;
    setDescription.m_setBindings.push_back(bindingDescription);

    // Check if it can be fit into an existing buffer
    if (Core::Utility::IsNewAllocationRequired(lightBufferSharingConfig))
    {
        // True : Allocate new buffer
        lightSetWrapper = UniFactAlias::GetInstance()->AllocateSetResources(setDescription);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        // below logic works because we are using just one buffer for sharing/storage purpose
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0] = std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferIdList[0];
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferMemoryId[0] = std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferMemoryId[0];
    }

    lightBufferSharingConfig.allocatedUniformCount += 1;

    Core::ECS::Components::LightUniform obj = {};
    obj.ambient = Vec3ToVec4_0(light->GetAmbient());
    obj.diffuse = Vec3ToVec4_0(light->GetDiffuse());
    obj.specular = Vec3ToVec4_0(light->GetSpecular());
    obj.lightPos = Vec3ToVec4_0(light->GetTransform()->GetGlobalPosition());
    obj.lightDir = Vec3ToVec4_0(light->GetTransform()->GetForward());
    obj.beamHeight = light->GetBeamHeight();
    obj.beamRadius = light->GetBeamRadius();

    //obj.lightSpaceMat = glm::mat4{}; //cam->GetProjectionMat() * cam->GetViewMatrix();

    //upload data to buffers
    for (uint32_t i = 0; i < lightUniformAllocConfig.numDescriptorSets; i++)
    {
        uint32_t bufferId = 0;
        if (lightUniformAllocConfig.numDescriptorSets != std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.size())
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0];
        else
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[i];

        UniFactAlias::GetInstance()->UploadDataToBuffers(bufferId,
            sizeof(Core::ECS::Components::LightUniform), memoryAlignedUniformSize, &obj,
            std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[i], false);
    }

    UniFactAlias::GetInstance()->AllocateDescriptorSets(lightSetWrapper, setDescription, lightUniformAllocConfig.numDescriptorSets);

    resDescriptionList.push_back(setDescription);

    {
        lightToDescriptionMap.insert(std::pair<Core::ECS::Components::Light *, Core::Utility::DescriptorSetInfo>(
        { light, setDescription }));
    }
}

#if 0
void LightSystem::HandleMeshAddition(MeshToMatAdditionEvent * meshAdditionEvent)
{
    uint32_t setCount = (uint32_t)meshAdditionEvent->setWrapperList.size();
    for (uint32_t k = 0; k < lightGraphNodeList.size(); k++)
        for (uint32_t i = 0; i < setCount; i++)
        {
            SetWrapper * wrapper = meshAdditionEvent->setWrapperList[i];
            if (wrapper->id == lightSetWrapper->id)
            {
                std::vector<uint32_t> meshList = lightGraphNodeList[k]->node->meshList;
                std::vector<uint32_t>::iterator it;
                it = std::find_if(meshList.begin(), meshList.end(), [=](uint32_t id) {
                    return id == meshAdditionEvent->meshId; });

                if (it == meshList.end())
                {
                    lightGraphNodeList[k]->node->meshList.push_back(meshAdditionEvent->meshId);
                }
            }
        }
}

void LightSystem::HandleRendererAddition(MeshRendererAdditionEvent * rendererAdditionEvent)
{
    // whaterever is the material attached to the mesh, we need to check if it casts shadow
    // if yes then add the mesh id to shadowMapping material
    
    uint32_t meshId = rendererAdditionEvent->renderer->geometry->componentId;
    if (!rendererAdditionEvent->renderer->castShadows)
        return;

    if (shadowMappingMat == nullptr)
    {
        ShaderDescription shaders[2];
        shaders[0].type = ShaderType::VERTEX;
        shaders[0].shaderName = "ShadowMapping.vert";

        shaders[1].type = ShaderType::FRAGMENT;
        shaders[1].shaderName = "ShadowMapping.frag";

        shadowMappingMat = MaterialFactory::GetInstance()->CreateMaterial(shaders, 2, meshId, (uint16_t)RenderPassTag::DepthPass);
        
        MultiSampleState * multiSampleState = new MultiSampleState;
        multiSampleState->alphaToCoverageEnable = false;
        multiSampleState->alphaToOneEnable = false;
        multiSampleState->sampleCount = Samples::SAMPLE_COUNT_1_BIT;
        multiSampleState->sampleShadingEnable = false;// RendererSettings::sampleRateShadingEnabled;
        multiSampleState->minSampleShading = 0.0f;

        GraphicsPipelineManager<VulkanInterface>::GetInstance()->CreateMultiSampleState(multiSampleState, (uint16_t)RenderPassTag::DepthPass);
    }
    else
    {
        MaterialFactory::GetInstance()->AddMeshIds(shadowMappingMat, meshId);
    }
}

// wont work as the graphics renderer gets setup before light system subscription for event
void LightSystem::HandleDepthPrepassCreation(DepthPassAttachmentCreationEvent * evt)
{
    
}
#endif

void LightSystem::AssignCameraSystem(System * camSystem)
{
    cameraSystem = camSystem;
}

LightSystem::LightSystem(std::vector<Core::Utility::LightData>& lightData) : m_lightDataList(lightData)
{
    signature.AddComponent<Core::ECS::Components::Light>();
}

LightSystem::~LightSystem()
{
}


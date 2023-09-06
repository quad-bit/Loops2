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
//#include "UniformFactory.h"
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
#if 0
    EventBus::GetInstance()->Subscribe<LightSystem, LightAdditionEvent>(this, &LightSystem::HandleLightAddition);
    EventBus::GetInstance()->Subscribe<LightSystem, MeshToMatAdditionEvent>(this, &LightSystem::HandleMeshAddition);
    EventBus::GetInstance()->Subscribe<LightSystem, MeshRendererAdditionEvent>(this, &LightSystem::HandleRendererAddition);

    lightUniformAllocConfig.numDescriptorSets = Settings::swapBufferCount;
    lightUniformAllocConfig.numMemories = 1;
    lightUniformAllocConfig.numResources = 1;

    shadowMapUniformAllocConfig.numDescriptorSets = Settings::swapBufferCount;
    shadowMapUniformAllocConfig.numMemories = 1;
    shadowMapUniformAllocConfig.numResources = 1;

    lightBufferSharingConfig.maxUniformPerResource = 1;
    lightBufferSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(LightUniform);
    memoryAlignedUniformSize = UniformFactory::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);
#endif
}

void LightSystem::DeInit()
{
    for (uint32_t i = 0; i < resDescriptionList.size(); i++)
    {
        delete [] resDescriptionList[i];
    }
    resDescriptionList.clear();
}

void LightSystem::Update(float dt)
{
#if 0
    for (auto & entity : registeredEntities)
    {
        Core::ECS::ComponentHandle<Core::ECS::Components::Light> * lightHandle;
        worldObj->Unpack(entity, &lightHandle);

        Core::ECS::Components::Light * light = lightHandle->GetComponent();
        LightUniform obj = {};
        obj.ambient = Vec3ToVec4_0(light->GetAmbient());
        obj.diffuse = Vec3ToVec4_0(light->GetDiffuse());
        obj.specular = Vec3ToVec4_0(light->GetSpecular());
        obj.lightPos = Vec3ToVec4_0(light->GetTransform()->GetGlobalPosition());
        obj.lightDir = Vec3ToVec4_0(light->GetTransform()->GetForward());
        obj.beamHeight = light->GetBeamHeight();
        obj.beamRadius = light->GetBeamRadius();

        Core::ECS::Components::Camera * cam = lightToCamList[light];
        obj.lightSpaceMat = cam->GetProjectionMat() * cam->GetViewMatrix();// *light->GetTransform()->GetLocalModelMatrix();

        ShaderBindingDescription * desc = lightToDescriptionMap[lightHandle->GetComponent()];

        //upload data to buffers
        {
            // using swapchainIndex as there are 3 descriptorSets because there are three depth images

            UniformFactory::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0], sizeof(LightUniform),
                memoryAlignedUniformSize, &obj, desc->bufferBindingInfo.info.offsetsForEachDescriptor[Settings::currentSwapChainIndex], false);
        
        }

        // TODO : write the uniform data of Camera to gpu memory via void*
    }
#endif
}

#if 0
void LightSystem::HandleLightAddition(LightAdditionEvent * lightAdditionEvent)
{
    Core::ECS::Components::Light* light = lightAdditionEvent->light;

    lightlist.push_back(light);
    light->componentId = GeneratedLightId();
    
    const uint32_t numBindingsInSet = 2; // Light + shadowMap

    ShaderBindingDescription * desc = new ShaderBindingDescription[numBindingsInSet];

    CreateLightUniformDescription(&desc[0], light);
    CreateShadowMap(&desc[1]);
    Core::ECS::Components::Camera * cam = CreateLightCamera(light->GetTransform());

    lightToCamList.insert(std::pair<Core::ECS::Components::Light*, Core::ECS::Components::Camera*>({light, cam}));

    lightSetWrapper = UniformFactory::GetInstance()->GetSetWrapper(desc, numBindingsInSet);
    CreateLightUniformBuffer(&desc[0], light, cam);
    
    // not creating shadow map image as its the depth attachment from depth pass, already created
    // hence depth image view needs to plugged into the descriptor write

    UniformFactory::GetInstance()->AllocateDescriptorSet(lightSetWrapper, desc, 
        numBindingsInSet, lightUniformAllocConfig.numDescriptorSets);

    // Create sampler 
    // Get the image id
    // Get the View id
    
    {
        lightToDescriptionMap.insert(std::pair<Core::ECS::Components::Light *, ShaderBindingDescription *>(
        { light, desc }));

        // draw graph node creation
        // top level node as its Set 0
        DrawGraphNode * lightNode = new LightDrawNode();
        lightNode->meshList = MaterialFactory::GetInstance()->GetMeshList(lightSetWrapper, 1);
        lightNode->setLevel = lightSetWrapper->setValue;
        lightNode->setWrapperList.push_back(lightSetWrapper);
        lightNode->tag = RenderPassTag::ColorPass;

        ((LightDrawNode*)lightNode)->descriptorIds = desc->descriptorSetIds;

        GraphNode<DrawGraphNode> * graphNode = new GraphNode<DrawGraphNode>(lightNode);
        lightGraphNodeList.push_back(graphNode);

        DrawGraphManager::GetInstance()->AddNode(graphNode);
    }
}


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

LightSystem::LightSystem()
{
    signature.AddComponent<Core::ECS::Components::Light>();
}

LightSystem::~LightSystem()
{
}


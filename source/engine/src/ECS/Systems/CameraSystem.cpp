#include "ECS/Systems/CameraSystem.h"
#include <ECS/Components/Camera.h>
#include <ECS/ComponentHandle.h>
#include <ECS/World.h>
#include <ECS/ECS_Setting.h>
#include <ECS/Components/Transform.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Events/ComponentAdditionEvent.h>
#include <Utility/ShaderResourceDescription.h>
#include <Utility/ResourceAllocationHelper.h>
#include <ECS/Events/MeshAdditionEvent.h>

#include "resourceManagement/ShaderResourceManager.h"


/*
#include "MaterialFactory.h"
#include "ShaderResourceManager.h"
#include "DrawGraphManager.h"
#include "DrawCommandBuffer.h"
#include "DrawGraphNode.h"
*/

uint32_t Engine::ECS::Systems::CameraSystem::GeneratedCamId()
{
    return idCounter++;
}

void Engine::ECS::Systems::CameraSystem::Init()
{
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<Engine::ECS::Systems::CameraSystem, Core::ECS::Events::CameraAdditionEvent>(this, &Engine::ECS::Systems::CameraSystem::HandleCameraAddition);
    //Core::ECS::Events::EventBus::GetInstance()->Subscribe<Engine::ECS::Systems::CameraSystem, Core::ECS::Events::MeshToMatAdditionEvent>(this, &Engine::ECS::Systems::CameraSystem::HandleMeshAddition);

    numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

    allocConfig.numDescriptorSets = numDescriptorSetsPerUniform;
    allocConfig.numMemories = 1; // 1 memory
    allocConfig.numResources = 1; //1 buff

    resourceSharingConfig.maxUniformPerResource = 2;
    resourceSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(Core::ECS::Components::CameraUniform);
    memoryAlignedUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);
}

void Engine::ECS::Systems::CameraSystem::DeInit()
{
    resDescriptionList.clear();
}

void Engine::ECS::Systems::CameraSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    m_cameraDataList.clear();
    for (auto & entity : registeredEntities)
    {
        Core::ECS::ComponentHandle<Core::ECS::Components::Camera> * camHandle;
        worldObj->Unpack(entity, &camHandle);

        UpdateCameraVectors(camHandle->GetComponent());

        Core::ECS::Components::CameraUniform obj = {};
        obj.projectionMat = camHandle->GetComponent()->GetProjectionMat();
        obj.viewMat = camHandle->GetComponent()->GetViewMatrix();
        obj.cameraPos = *camHandle->GetComponent()->GetPosition();

        Core::Utility::DescriptorSetInfo desc = camToDescriptionMap[camHandle->GetComponent()];
        //upload data to buffers
        {
            ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).bufferIdList[0],
                sizeof(Core::ECS::Components::CameraUniform), memoryAlignedUniformSize, &obj,
                std::get<Core::Utility::BufferBindingInfo>(desc.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[frameInfo.m_frameInFlightIndex], false);
        }
        // TODO : write the uniform data of Camera to gpu memory via void*
        Core::Utility::CameraData data = {};
        data.m_cameraPosition = obj.cameraPos;
        data.m_projectionMat = obj.projectionMat;
        data.m_viewMat = obj.viewMat;
        data.m_far = camHandle->GetComponent()->GetFar();
        data.m_near = camHandle->GetComponent()->GetNear();
        data.m_descriptorSetId = desc.m_descriptorSetIds[frameInfo.m_frameInFlightIndex];
        data.m_renderLayers = entity->GetEntityLayers();
        data.m_tag = entity->entityTag;
        m_cameraDataList.push_back(data);
    }
}


void Engine::ECS::Systems::CameraSystem::HandleCameraAddition(Core::ECS::Events::CameraAdditionEvent * inputEvent)
{
    // recieved the camera addition to the scene 
    cameraList.push_back(inputEvent->cam);
    inputEvent->cam->componentId = GeneratedCamId();

    Core::Utility::BufferBindingInfo bufInfo{};
    bufInfo.info.allocationConfig = allocConfig;
    bufInfo.info.dataSizePerDescriptor = sizeof(Core::ECS::Components::CameraUniform);
    bufInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    bufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
    bufInfo.info.sharingConfig = resourceSharingConfig;
    bufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
    //bufInfo.bufferIdList.resize(allocConfig.numResources);
    //bufInfo.bufferMemoryId.resize(allocConfig.numResources);

    Core::Utility::DescriptorSetBindingInfo bindingDescription;
    bindingDescription.m_bindingName = "View";
    bindingDescription.m_bindingNumber = 0;
    bindingDescription.m_numElements = 3;
    bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
    bindingDescription.m_bindingInfo = bufInfo;

    Core::Utility::DescriptorSetInfo setDescription;
    setDescription.m_numBindings = 1;
    setDescription.m_setNumber = (uint32_t)Core::Enums::ResourceSets::CAMERA;
    setDescription.m_setBindings.push_back(bindingDescription);

    // Check if it can be fit into an existing buffer
    if (Core::Utility::IsNewAllocationRequired(resourceSharingConfig))
    {
        // True : Allocate new buffer
        cameraSetWrapper = ShdrResMgrAlias::GetInstance()->AllocateSetResources(setDescription);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        // below logic works because we are using just one buffer for sharing/storage purpose
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.push_back(std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferIdList[0]);
        std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferMemoryId.push_back(std::get<Core::Utility::BufferBindingInfo>(resDescriptionList[resDescriptionList.size() - 1].m_setBindings[0].m_bindingInfo).bufferMemoryId[0]);
    }

    resourceSharingConfig.allocatedUniformCount += 1;

    Core::ECS::Components::CameraUniform obj = {};
    obj.projectionMat = inputEvent->cam->GetProjectionMat();
    obj.viewMat = inputEvent->cam->GetViewMatrix();
    obj.cameraPos = *inputEvent->cam->GetPosition();

    //upload data to buffers
    for(uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        uint32_t bufferId = 0;
        if (allocConfig.numDescriptorSets != std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList.size())
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[0];
        else
            bufferId = std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).bufferIdList[i];

        ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(bufferId,
            sizeof(Core::ECS::Components::CameraUniform), memoryAlignedUniformSize, &obj, 
            std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[0].m_bindingInfo).info.offsetsForEachDescriptor[i], false);
    }

    ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(cameraSetWrapper, setDescription, allocConfig.numDescriptorSets);

    resDescriptionList.push_back(setDescription);

    camToDescriptionMap.insert(std::pair<Core::ECS::Components::Camera *, Core::Utility::DescriptorSetInfo>(
    { inputEvent->cam , setDescription}));
}

Core::Utility::GlobalResourceAllocationConfig Engine::ECS::Systems::CameraSystem::GetAllocConfig() const
{
    return allocConfig;
}

Core::Utility::GlobalResourceSharingConfig Engine::ECS::Systems::CameraSystem::GetResourceSharingConfig() const
{
    return resourceSharingConfig;
}

/*
GraphNode<DrawGraphNode>* Engine::ECS::Systems::CameraSystem::HandleCameraAddition(Camera * camera, const RenderPassTag & tag)
{
    // recieved the camera addition to the scene 
    cameraList.push_back(camera);
    camera->componentId = GeneratedCamId();

    ShaderBindingDescription * desc = new ShaderBindingDescription;
    desc->set = (uint32_t)ResourceSets::CAMERA;
    desc->binding = 0;
    desc->numElements = 3;
    desc->resourceName = "View";
    desc->resourceType = DescriptorType::UNIFORM_BUFFER;
    desc->bufferBindingInfo.info.dataSizePerDescriptorAligned = memoryAlignedUniformSize;
    desc->bufferBindingInfo.info.dataSizePerDescriptor = sizeof(CameraUniform);
    desc->bufferBindingInfo.info.offsetsForEachDescriptor = AllocationUtility::CalculateOffsetsForDescInUniform(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
    desc->bufferBindingInfo.info.allocationConfig = allocConfig;
    desc->bufferBindingInfo.bufferIdList.resize(allocConfig.numResources);
    desc->bufferBindingInfo.bufferMemoryId.resize(allocConfig.numMemories);

    // Check if it can be fit into an existing buffer
    if (AllocationUtility::IsNewAllocationRequired(resourceSharingConfig))
    {
        size_t totalSize = AllocationUtility::GetDataSizeMeantForSharing(memoryAlignedUniformSize, allocConfig, resourceSharingConfig);
        // True : Allocate new buffer
        cameraSetWrapper = ShdrResMgrAlias::GetInstance()->AllocateSetResource(desc, &totalSize, 1, AllocationMethod::LAZY);
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        desc->bufferBindingInfo.bufferIdList[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferIdList[0];
        desc->bufferBindingInfo.bufferMemoryId[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferMemoryId[0];
    }

    resDescriptionList.push_back(desc);
    resourceSharingConfig.allocatedUniformCount += 1;

    CameraUniform obj = {};
    obj.projectionMat = camera->GetProjectionMat();
    obj.viewMat = camera->GetViewMatrix();
    obj.cameraPos = *camera->GetPosition();

    //upload data to buffers
    for (uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0],
            sizeof(CameraUniform), memoryAlignedUniformSize, &obj,
            desc->bufferBindingInfo.info.offsetsForEachDescriptor[i], false);
    }

    ShdrResMgrAlias::GetInstance()->AllocateDescriptorSet(cameraSetWrapper, desc, 1, allocConfig.numDescriptorSets);

    camToDescriptionMap.insert(std::pair<Camera *, ShaderBindingDescription *>(
    { camera , desc }));

    // draw graph node creation
    // top level node as its Set 0
    DrawGraphNode * cameraNode = new CameraDrawNode();
    cameraNode->meshList = MaterialFactory::GetInstance()->GetMeshList(cameraSetWrapper, 1);
    cameraNode->setLevel = cameraSetWrapper->setValue;
    cameraNode->tag = tag;
    cameraNode->setWrapperList.push_back(cameraSetWrapper);
    ((CameraDrawNode*)cameraNode)->descriptorIds = desc->descriptorSetIds;

    GraphNode<DrawGraphNode> * graphNode = new GraphNode<DrawGraphNode>(cameraNode);
    DrawGraphManager::GetInstance()->AddNode(graphNode);
    cameraGraphNodeList.push_back(graphNode);

    return graphNode;
}

void Engine::ECS::Systems::CameraSystem::HandleMeshAddition(MeshToMatAdditionEvent * meshAdditionEvent)
{
    uint32_t setCount = (uint32_t)meshAdditionEvent->setWrapperList.size();
    for (uint32_t k = 0; k < cameraGraphNodeList.size(); k++)
    for (uint32_t i = 0; i < setCount; i++)
    {
        SetWrapper * wrapper = meshAdditionEvent->setWrapperList[i];
        if (wrapper->id == cameraSetWrapper->id)
        {
            std::vector<uint32_t> meshList = cameraGraphNodeList[k]->node->meshList;
            std::vector<uint32_t>::iterator it;
            it = std::find_if(meshList.begin(), meshList.end(), [=](uint32_t id) {
                return id == meshAdditionEvent->meshId; });

            if (it == meshList.end())
            {
                cameraGraphNodeList[k]->node->meshList.push_back(meshAdditionEvent->meshId);
            }
        }
    }
}
*/

Engine::ECS::Systems::CameraSystem::CameraSystem(std::vector<Core::Utility::CameraData>& cameraData) : m_cameraDataList(cameraData)
{
    signature.AddComponent<Core::ECS::Components::Camera>();
    signature.AddComponent<Core::ECS::Components::Transform>();
}

Engine::ECS::Systems::CameraSystem::~CameraSystem()
{
}

void Engine::ECS::Systems::CameraSystem::SetMainCamera(uint32_t componentId)
{
    mainCameraId = componentId;
}

void Engine::ECS::Systems::CameraSystem::ProcessKeyboard(Core::ECS::Components::Camera * cam, glm::vec3 * camTransformPos, Core::ECS::Components::Camera_Movement * direction, float deltaTime)
{
    float velocity = cam->GetMovementSpeed()* deltaTime;

    if (*direction == Core::ECS::Components::Camera_Movement::FORWARD)
    {
        *camTransformPos += cam->GetFront() * velocity;
    }

    if (*direction == Core::ECS::Components::Camera_Movement::BACKWARD)
    {
        *camTransformPos -= cam->GetFront() * velocity;
    }

    if (*direction == Core::ECS::Components::Camera_Movement::LEFT)
    {
        *camTransformPos -= cam->GetRight() * velocity;
    }

    if (*direction == Core::ECS::Components::Camera_Movement::RIGHT)
    {
        *camTransformPos += cam->GetRight() * velocity;
    }
}

void Engine::ECS::Systems::CameraSystem::ProcessMouseMovement(Core::ECS::Components::Camera * cam, float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= cam->GetMouseSensitivity();
    yOffset *= cam->GetMouseSensitivity();

    cam->GetYaw() += xOffset;
    cam->GetPitch() += yOffset;

    // Make sure that when GetPitch() is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (cam->GetPitch() > 89.0f)
        {
            cam->GetPitch() = 89.0f;
        }

        if (cam->GetPitch() < -89.0f)
        {
            cam->GetPitch() = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    //cam->updateCameraVectors();
    UpdateCameraVectors(cam);
}

void Engine::ECS::Systems::CameraSystem::UpdateCameraVectors(Core::ECS::Components::Camera * cam)
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(cam->GetYaw())) * cos(glm::radians(cam->GetPitch()));
    front.y = sin(glm::radians(cam->GetPitch()));
    front.z = sin(glm::radians(cam->GetYaw())) * cos(glm::radians(cam->GetPitch()));
    cam->GetFront() = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    cam->GetRight() = glm::normalize(glm::cross(cam->GetFront(), cam->GetWorlUp()));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    cam->GetUp() = glm::normalize(glm::cross(cam->GetRight(), cam->GetFront()));
}


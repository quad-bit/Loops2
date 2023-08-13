#include "MeshRendererSystem.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"
#include "World.h"
#include "EventBus.h"
#include "ComponentAdditionEvent.h"
#include "ResourceAllocationHelper.h"
#include "UniformFactory.h"
#include "DrawGraphManager.h"
#include "Graph.h"
#include <plog/Log.h> 
#include "DrawCommandBuffer.h"
#include "DrawGraphNode.h"

uint32_t MeshRendererSystem::GenerateId()
{
    return idCounter++;
}

void MeshRendererSystem::Init()
{
    EventBus::GetInstance()->Subscribe<MeshRendererSystem, MeshRendererAdditionEvent>(this, &MeshRendererSystem::HandleMeshRendererAddition);

    allocConfig.numDescriptorSets = Settings::maxFramesInFlight;
    allocConfig.numMemories = 1;
    allocConfig.numResources = 1;

    resourceSharingConfig.maxUniformPerResource = 2;
    resourceSharingConfig.allocatedUniformCount = 0;

    size_t uniformSize = sizeof(TransformUniform);
    memoryAlignedDataSize = UniformFactory::GetInstance()->GetMemoryAlignedDataSizeForBuffer(uniformSize);
}

void MeshRendererSystem::DeInit()
{
    for each(auto obj in meshNodeList)
    {
        delete obj->node;
        delete obj;
    }

    meshNodeList.clear();
    
    for each(auto obj in transformNodeList)
    {
        delete obj->node;
        delete obj;
    }

    transformNodeList.clear();

    for each(auto obj in drawingNodeList)
    {
        delete obj->node;
        delete obj;
    }

    drawingNodeList.clear();
}

void MeshRendererSystem::Update(float dt)
{
    for (auto & entity : registeredEntities)
    {
        ComponentHandle<MeshRenderer> * renderer;
        ComponentHandle<Transform> * transform;
        worldObj->Unpack(entity, &renderer, &transform);
        
        Transform * transformObj = transform->GetComponent();
        TransformUniform obj = {};
        obj.modelMat = transformObj->GetGlobalModelMatrix();

        ShaderBindingDescription * desc = transformToBindDescMap[transformObj];
        UniformFactory::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0], memoryAlignedDataSize, memoryAlignedDataSize,
            &obj, desc->bufferBindingInfo.info.offsetsForEachDescriptor[Settings::currentFrameInFlight], false);

    }
}

MeshRendererSystem::MeshRendererSystem()
{
    signature.AddComponent<MeshRenderer>();
    signature.AddComponent<Mesh>();
    signature.AddComponent<Material>();
    signature.AddComponent<Transform>();
}

MeshRendererSystem::~MeshRendererSystem()
{
}

void MeshRendererSystem::HandleMeshRendererAddition(MeshRendererAdditionEvent * inputEvent)
{
    // Create descriptor for transform set
    inputEvent->renderer->componentId = GenerateId();

    size_t uniformSize = sizeof(TransformUniform);

    ShaderBindingDescription * desc = new ShaderBindingDescription;
    desc->set = 4;
    desc->binding = 0;
    desc->numElements = 1;
    desc->resourceName = "Transform";
    desc->resourceType = DescriptorType::UNIFORM_BUFFER;
    desc->resParentId = inputEvent->renderer->componentId;
    desc->parentType = inputEvent->renderer->componentType;
    desc->uniformId = inputEvent->renderer->componentId; 
    desc->bufferBindingInfo.info.dataSizePerDescriptorAligned = memoryAlignedDataSize;
    desc->bufferBindingInfo.info.dataSizePerDescriptor = sizeof(TransformUniform);
    desc->bufferBindingInfo.info.offsetsForEachDescriptor = AllocationUtility::CalculateOffsetsForDescInUniform(memoryAlignedDataSize, allocConfig, resourceSharingConfig);
    desc->bufferBindingInfo.info.allocationConfig = allocConfig;
    desc->bufferBindingInfo.bufferIdList.resize(allocConfig.numResources);
    desc->bufferBindingInfo.bufferMemoryId.resize(allocConfig.numResources);

    transformSetWrapper = UniformFactory::GetInstance()->GetSetWrapper(desc, 1);

    // Check if it can be fit into an existing buffer
    if (AllocationUtility::IsNewAllocationRequired(resourceSharingConfig))
    {
        // True : Allocate new buffer
        size_t totalSize = AllocationUtility::GetDataSizeMeantForSharing(memoryAlignedDataSize, allocConfig, resourceSharingConfig);
        transformSetWrapper = UniformFactory::GetInstance()->AllocateSetResource(desc, &totalSize, 1, AllocationMethod::LAZY);
        
        /*BufferCreateInfo info = {};
        info.size = memoryAlignedDataSize;
        info.usage.push_back(BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        
        uint32_t numBuf = allocConfig.numResources;
        uint32_t * ids = new uint32_t[numBuf];
        size_t * memSizes = new size_t[numBuf];

        UniformFactory::GetInstance()->AllocateUniformBuffer(&info, numBuf, ids, memSizes);
        for (uint32_t i = 0; i < allocConfig.numResources; i++)
        {
            desc->bufferBindingInfo.bufferIdList[i] = ids[i];

            MemoryRequirementInfo info = {};
            info.size = memSizes[i];

            std::array<MemoryType, 2> memType{ MemoryType::HOST_VISIBLE_BIT, MemoryType::HOST_COHERENT_BIT };

            desc->bufferBindingInfo.bufferMemoryId[i] = UniformFactory::GetInstance()->AllocateBufferMemory(ids[i], &info, memType.data(), memType.size(), memSizes[i]);
        }

        delete[] ids;
        delete[] memSizes;*/
    }
    else
    {
        // False : Assign the buffer id to this shaderResourceDescription
        desc->bufferBindingInfo.bufferIdList[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferIdList[0];
        desc->bufferBindingInfo.bufferMemoryId[0] = resDescriptionList[resDescriptionList.size() - 1]->bufferBindingInfo.bufferMemoryId[0];
    }

    resDescriptionList.push_back(desc);
    resourceSharingConfig.allocatedUniformCount += 1;
    
    Transform * transform = inputEvent->renderer->transform;

    TransformUniform obj = {};
    obj.modelMat = transform->GetGlobalModelMatrix();

    //upload data to buffers
    for (uint32_t i = 0; i < allocConfig.numDescriptorSets; i++)
    {
        UniformFactory::GetInstance()->UploadDataToBuffers(desc->bufferBindingInfo.bufferIdList[0], memoryAlignedDataSize, memoryAlignedDataSize, &obj, desc->bufferBindingInfo.info.offsetsForEachDescriptor[i], false);
    }
    
    // allocate descriptors
    UniformFactory::GetInstance()->AllocateDescriptorSet(transformSetWrapper, desc, 1, allocConfig.numDescriptorSets);
    
    uint32_t meshId = inputEvent->renderer->geometry->componentId;
    
    DrawGraphNode * meshNode = new MeshNode;
    {
        meshNode->meshList.push_back(meshId);
        uint32_t numVertBuffers = inputEvent->renderer->geometry->vertexBufferCount;
        for (uint32_t i = 0; i < numVertBuffers; i++)
            ((MeshNode*)meshNode)->bufferIds.push_back(inputEvent->renderer->geometry->vertexBuffersIds[i]);
        ((MeshNode*)meshNode)->pOffsets.push_back(0);
        
        if (inputEvent->renderer->geometry->indexCount > 0)
        {
            ((MeshNode*)meshNode)->isIndexed = true;
            ((MeshNode*)meshNode)->indexBufferId = inputEvent->renderer->geometry->indexBufferId;
        }
        else
            ((MeshNode*)meshNode)->isIndexed = false;
        
    }

    DrawGraphNode * trfnode = new TransformNode;
    {
        trfnode->setWrapperList.push_back(transformSetWrapper);
        trfnode->meshList.push_back(meshId);
        trfnode->setLevel = transformSetWrapper->setValue;
        ((TransformNode*)trfnode)->descriptorSetIds = desc->descriptorSetIds;
    }

    uint32_t indexCount = inputEvent->renderer->geometry->indexCount;
    
    DrawGraphNode * drawingnode;
    if(indexCount > 0)
    {
        drawingnode = new IndexedDrawNode;
        drawingnode->meshList.push_back(meshId);
        
        //TODO : automate the drawing the attribs
        ((IndexedDrawNode*)drawingnode)->info.firstIndex = 0;
        ((IndexedDrawNode*)drawingnode)->info.firstInstance = 0;
        ((IndexedDrawNode*)drawingnode)->info.indexCount = inputEvent->renderer->geometry->indexCount;
        ((IndexedDrawNode*)drawingnode)->info.instanceCount = 1;
        ((IndexedDrawNode*)drawingnode)->info.vertexOffset = 0;
    }
    else
    {
        drawingnode = new DrawArrayDrawNode;
        drawingnode->meshList.push_back(meshId);
        
        //TODO : automate the drawing the attribs
        ((DrawArrayDrawNode*)drawingnode)->info.firstVertex = 0;
        ((DrawArrayDrawNode*)drawingnode)->info.vertexCount = inputEvent->renderer->geometry->vertexCount;
        ((DrawArrayDrawNode*)drawingnode)->info.instanceCount = 1;
        ((DrawArrayDrawNode*)drawingnode)->info.firstInstance = 0;
    }

    drawingnode->tag = RenderPassTag::ColorPass;
    trfnode->tag = RenderPassTag::ColorPass;
    meshNode->tag = RenderPassTag::ColorPass;

    if (inputEvent->renderer->castShadows)
    {
        drawingnode->tag = RenderPassTag::ColorPass | RenderPassTag::DepthPass;
        trfnode->tag = RenderPassTag::ColorPass | RenderPassTag::DepthPass;
        meshNode->tag = RenderPassTag::ColorPass | RenderPassTag::DepthPass;
    }

    GraphNode<DrawGraphNode> * transformGraphNode = new GraphNode<DrawGraphNode>(trfnode);
    GraphNode<DrawGraphNode> * meshGraphNode = new GraphNode<DrawGraphNode>(meshNode);
    GraphNode<DrawGraphNode> * drawingGraphNode = new GraphNode<DrawGraphNode>(drawingnode);
    DrawGraphManager::GetInstance()->AddNode(transformGraphNode);
    DrawGraphManager::GetInstance()->AddNode(meshGraphNode);
    DrawGraphManager::GetInstance()->AddNode(drawingGraphNode);
    
    meshNodeList.push_back(meshGraphNode);
    transformNodeList.push_back(transformGraphNode);
    drawingNodeList.push_back(drawingGraphNode);
    
    DrawGraphManager::GetInstance()->CreateGraphEdges(meshGraphNode, transformGraphNode);
    DrawGraphManager::GetInstance()->CreateGraphEdges(transformGraphNode, drawingGraphNode);

    transformToBindDescMap.insert(std::pair<Transform *, ShaderBindingDescription *>(
    {transform, desc}));
}


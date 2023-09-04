#include "VkCommandBufferFactory.h"
#include <CorePrecompiled.h>
#include "utility/VulkanUtility.h"
#include "VkQueueFactory.h"
#include "VkDrawCommandBuffer.h"

GfxVk::CommandPool::VkCommandBufferFactory* GfxVk::CommandPool::VkCommandBufferFactory::instance = nullptr;
uint32_t GfxVk::CommandPool::VkCommandBufferFactory::poolIdCounter = 0, GfxVk::CommandPool::VkCommandBufferFactory::bufferIdCounter = 0;

/*
VkCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(uint32_t id)
{
    std::vector<VkDrawCommandBuffer*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](VkDrawCommandBuffer* e) { return e->id == id; });

    ASSERT_MSG(it != drawCommandBufferList.end(), "Image id not found");
    
    return (*it)->commandBuffer;
}
*/

VkCommandPool * GfxVk::CommandPool::VkCommandBufferFactory::GetCommandPool(const uint32_t& poolId)
{
    std::vector<VkCommandPoolWrapper>::iterator it;
    it = std::find_if(poolList.begin(), poolList.end(), [&](VkCommandPoolWrapper e) { return e.poolId == poolId; });

    ASSERT_MSG_DEBUG(it != poolList.end(), "Image id not found");

    return it->pool;
}

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::GetPoolId()
{
    return poolIdCounter++;
}

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::GetBufferId()
{
    return bufferIdCounter++;
}

void GfxVk::CommandPool::VkCommandBufferFactory::Init()
{
    PLOGD << "VkCommandBufferFactory init";

    renderQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, GfxVk::Utility::CoreObjects::renderQueueId);
    computeQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, GfxVk::Utility::CoreObjects::computeQueueId);
    transferQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, GfxVk::Utility::CoreObjects::transferQueueId);

    graphicsQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, GfxVk::Utility::CoreObjects::renderQueueId);
    computeQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, GfxVk::Utility::CoreObjects::computeQueueId);
    transferQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, GfxVk::Utility::CoreObjects::transferQueueId);
}

void GfxVk::CommandPool::VkCommandBufferFactory::DeInit()
{
    PLOGD << "VkCommandBufferFactory Deinit";
    for each (VkCommandPoolWrapper pool in poolList)
    {
        delete pool.pool;
    }

    poolList.clear();

    //for each (VkDrawCommandBuffer* buf in drawCommandBufferList)
    for(uint32_t i = 0; i < drawCommandBufferList.size();i++)
    {
        delete drawCommandBufferList[i]->commandBuffer;
        delete drawCommandBufferList[i];
    }
    
    drawCommandBufferList.clear();
}

void GfxVk::CommandPool::VkCommandBufferFactory::Update()
{

}

GfxVk::CommandPool::VkCommandBufferFactory * GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::CommandPool::VkCommandBufferFactory();
    }
    return instance;
}

GfxVk::CommandPool::VkCommandBufferFactory::~VkCommandBufferFactory()
{
}

void GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandPool(VkCommandPoolCreateFlagBits flags, VkQueueFlagBits queueType, uint32_t & id)
{
    uint32_t queueFamily = -1;

    switch (queueType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT :
        queueFamily = graphicsQueueFamilyId;
        break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
        queueFamily = computeQueueFamilyId;
        break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
        queueFamily = transferQueueFamilyId;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Command buffer for this queue not supported");
    }

    VkCommandPoolWrapper wrapper;
    wrapper.poolId = GetPoolId();
    wrapper.pool = new VkCommandPool;
    
    VkCommandPoolCreateInfo info = {};
    info.queueFamilyIndex = queueFamily;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags = flags;

    GfxVk::Utility::ErrorCheck(vkCreateCommandPool(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &info, 
        GfxVk::Utility::CoreObjects::pAllocator, wrapper.pool));

    id = wrapper.poolId;
    poolList.push_back(wrapper);
}

/*
void GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level, uint32_t count, uint32_t * ids)
{
    VkCommandPool* pool = GetCommandPool(poolId);
    VkCommandBufferWrapper * wrapper = new VkCommandBufferWrapper;
    wrapper->commandBuffer = new VkCommandBuffer;
    wrapper->id = GetBufferId();
    wrapper->pool = pool;

    ids[0] = wrapper->id;

    VkCommandBufferAllocateInfo info = {};
    info.commandBufferCount = count;
    info.level = level;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = *pool;

    GfxVk::Utility::ErrorCheck(vkAllocateCommandBuffers(*GfxVk::Utility::CoreObjects::logicalDeviceObj, 
        &info, wrapper->commandBuffer));

    bufferList.push_back(*wrapper);
}
*/

GfxVk::CommandPool::VkDrawCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level,
    Core::Enums::PipelineType commandBufferType, uint32_t* ids)
{
    VkCommandPool* pool = GetCommandPool(poolId);
    VkDrawCommandBuffer * drawCommandBuffer = new VkDrawCommandBuffer;
    
    drawCommandBuffer->commandBuffer = new VkCommandBuffer;
    drawCommandBuffer->id = GetBufferId();
    drawCommandBuffer->pool = pool;
    drawCommandBuffer->commandBufferType = commandBufferType;
    drawCommandBuffer->level = level;
    *ids = drawCommandBuffer->id;

    VkCommandBufferAllocateInfo info = {};
    info.commandBufferCount = 1;
    info.level = level;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = *pool;

    GfxVk::Utility::ErrorCheck(vkAllocateCommandBuffers(*GfxVk::Utility::CoreObjects::logicalDeviceObj,
        &info, drawCommandBuffer->commandBuffer));

    drawCommandBufferList.push_back(drawCommandBuffer);

    return drawCommandBuffer;
}

void GfxVk::CommandPool::VkCommandBufferFactory::DestroyCommandPool(const uint32_t &  id)
{
    std::vector<VkCommandPoolWrapper>::iterator it;
    it = std::find_if(poolList.begin(), poolList.end(), [&](VkCommandPoolWrapper e) { return e.poolId == id; });

    ASSERT_MSG_DEBUG(it != poolList.end(), "Pool id not found");

    vkDestroyCommandPool(*GfxVk::Utility::CoreObjects::logicalDeviceObj, *it->pool, GfxVk::Utility::CoreObjects::pAllocator);
}

void GfxVk::CommandPool::VkCommandBufferFactory::ResetCommandPool(const uint32_t &  id)
{
}

void GfxVk::CommandPool::VkCommandBufferFactory::ResetCommandBuffer(const uint32_t & id, const uint32_t & poolId)
{

    std::vector<VkDrawCommandBuffer*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](VkDrawCommandBuffer * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != drawCommandBufferList.end(), "Pool id not found");

    vkResetCommandBuffer(*(*it)->commandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

}

void GfxVk::CommandPool::VkCommandBufferFactory::BeginCommandBufferRecording(const uint32_t & id, VkCommandBufferUsageFlagBits usage, VkCommandBufferInheritanceInfo * inheritanceInfo)
{
    VkCommandBufferBeginInfo info = {};
    info.pInheritanceInfo = inheritanceInfo;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = usage;
    info.pNext = nullptr;

    GfxVk::Utility::ErrorCheck(vkBeginCommandBuffer(*GetCommandBuffer(id), &info));
}

void GfxVk::CommandPool::VkCommandBufferFactory::BeginCommandBufferRecording(VkCommandBufferUsageFlagBits usage, VkCommandBufferInheritanceInfo * inheritanceInfo)
{
    VkCommandBufferBeginInfo info = {};
    info.pInheritanceInfo = inheritanceInfo;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = usage;
    info.pNext = nullptr;

    GfxVk::Utility::ErrorCheck(vkBeginCommandBuffer(*activeCommandBuffer, &info));
}

void GfxVk::CommandPool::VkCommandBufferFactory::EndCommandBufferRecording(const uint32_t & id)
{
    VkCommandBuffer * buf = GetCommandBuffer(id);
    GfxVk::Utility::ErrorCheck( vkEndCommandBuffer(*buf) );
}

void GfxVk::CommandPool::VkCommandBufferFactory::EndCommandBufferRecording()
{
    GfxVk::Utility::ErrorCheck(vkEndCommandBuffer(*activeCommandBuffer));
}

void GfxVk::CommandPool::VkCommandBufferFactory::DestroyCommandBuffer(const uint32_t & id)
{
    std::vector<VkDrawCommandBuffer*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](VkDrawCommandBuffer * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != drawCommandBufferList.end(), "Pool id not found");

    vkFreeCommandBuffers(*GfxVk::Utility::CoreObjects::logicalDeviceObj, *(*it)->pool, 1, (*it)->commandBuffer);
}

void GfxVk::CommandPool::VkCommandBufferFactory::ActivateCommandBuffer(const uint32_t & id)
{
    activeCommandBuffer = GetCommandBuffer(id);
}

VkCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(const uint32_t & id)
{
    std::vector<VkDrawCommandBuffer*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](VkDrawCommandBuffer* e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != drawCommandBufferList.end(), "Image id not found");

    return (*it)->commandBuffer;
}

VkCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(const uint32_t * ids, const uint32_t & count)
{
    return GetCommandBuffer(ids[0]);
}

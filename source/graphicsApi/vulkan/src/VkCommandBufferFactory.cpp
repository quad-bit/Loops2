#include "VkCommandBufferFactory.h"
#include <CorePrecompiled.h>
#include "utility/VulkanUtility.h"
#include "VkQueueFactory.h"
#include "VkDrawCommandBuffer.h"

GfxVk::CommandPool::VkCommandBufferFactory* GfxVk::CommandPool::VkCommandBufferFactory::instance = nullptr;
uint32_t GfxVk::CommandPool::VkCommandBufferFactory::poolIdCounter = 0, GfxVk::CommandPool::VkCommandBufferFactory::bufferIdCounter = 0;

namespace
{
    void CreateCommandPoolUtil(VkCommandPoolCreateFlagBits flags, VkQueueFlagBits queueType, uint32_t queueFamily, VkCommandPool& pool)
    {
        VkCommandPoolCreateInfo info = {};
        info.queueFamilyIndex = queueFamily;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags = flags;

        GfxVk::Utility::ErrorCheck(vkCreateCommandPool(DeviceInfo::GetLogicalDevice(), &info,
            DeviceInfo::GetAllocationCallback(), &pool));
    }
}


/*
VkCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(uint32_t id)
{
    std::vector<VkDrawCommandBuffer*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](VkDrawCommandBuffer* e) { return e->id == id; });

    ASSERT_MSG(it != drawCommandBufferList.end(), "Image id not found");
    
    return (*it)->commandBuffer;
}
*/

const VkCommandPool& GfxVk::CommandPool::VkCommandBufferFactory::GetCommandPool(uint32_t poolId)
{
    ASSERT_MSG_DEBUG(m_additionsPoolList.find(poolId) != m_additionsPoolList.end(), "Image id not found");
    return m_additionsPoolList[poolId].m_pool;
}

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::GetPoolId()
{
    return poolIdCounter++;
}

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::GetBufferId()
{
    return bufferIdCounter++;
}

GfxVk::CommandPool::VkCommandPoolWrapper& GfxVk::CommandPool::VkCommandBufferFactory::GetPoolWrapper(const VkQueueFlags& queueType)
{
    switch (queueType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
        return m_graphicsPool;
        break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
        return m_computePool;
        break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
        return m_transferPool;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Command buffer for this queue not supported");
    }
    return m_graphicsPool;
}

void GfxVk::CommandPool::VkCommandBufferFactory::Init(uint32_t renderQueueId, uint32_t computeQueueId, uint32_t transferQueueId)
{
    PLOGD << "VkCommandBufferFactory init";

    renderQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, renderQueueId);
    computeQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, computeQueueId);
    transferQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, transferQueueId);

    graphicsQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, renderQueueId);
    computeQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, computeQueueId);
    transferQueueFamilyId = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, transferQueueId);

    VkCommandPoolCreateFlagBits flag = (VkCommandPoolCreateFlagBits)(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto CreatePool = [this, &flag](VkCommandPoolWrapper& wrapper, VkQueueFlagBits queueFlag, uint32_t family)
    {
        wrapper.m_poolId = GetPoolId();
        wrapper.m_queueType = queueFlag;
        CreateCommandPoolUtil(flag, wrapper.m_queueType, family, wrapper.m_pool);
    };

    CreatePool(m_graphicsPool, VK_QUEUE_GRAPHICS_BIT, graphicsQueueFamilyId);
    CreatePool(m_computePool, VK_QUEUE_COMPUTE_BIT, computeQueueFamilyId);
    CreatePool(m_transferPool, VK_QUEUE_TRANSFER_BIT, transferQueueFamilyId);
}

void GfxVk::CommandPool::VkCommandBufferFactory::DeInit()
{
    PLOGD << "VkCommandBufferFactory Deinit";

    vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), m_graphicsPool.m_pool, DeviceInfo::GetAllocationCallback());
    vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), m_computePool.m_pool, DeviceInfo::GetAllocationCallback());
    vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), m_transferPool.m_pool, DeviceInfo::GetAllocationCallback());

    for each (auto& item in m_additionsPoolList)
    {
        vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), item.second.m_pool, DeviceInfo::GetAllocationCallback());
    }

    m_additionsPoolList.clear();
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

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandPool(VkCommandPoolCreateFlagBits flags, VkQueueFlagBits queueType)
{
    uint32_t queueFamily = -1;

    switch (queueType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
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

    VkCommandPoolWrapper wrapper = {};
    wrapper.m_poolId = GetPoolId();
    auto id = wrapper.m_poolId;
    wrapper.m_queueType = queueType;

    CreateCommandPoolUtil(flags, queueType, queueFamily, wrapper.m_pool);
    m_additionsPoolList.insert({ id, std::move(wrapper) });
    return id;
}

uint32_t GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandBuffer(const VkCommandBufferLevel& level, const VkQueueFlags& queueType)
{
    auto CreateBuffer = [&](VkCommandPoolWrapper& wrapper) -> uint32_t
    {
        VkCommandBufferWrapper bufWrapper{};
        bufWrapper.m_id = GetBufferId();

        VkCommandBufferAllocateInfo info = {};
        info.commandBufferCount = 1;
        info.level = level;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = wrapper.m_pool;

        GfxVk::Utility::ErrorCheck(vkAllocateCommandBuffers(DeviceInfo::GetLogicalDevice(),
            &info, &bufWrapper.m_cmdBuffer));

        auto id = bufWrapper.m_id;
        wrapper.m_cmdBufferList.insert({ id, std::move(bufWrapper) });

        return id;
    };

    uint32_t bufferId = CreateBuffer(GetPoolWrapper(queueType));
    return bufferId;
}

void GfxVk::CommandPool::VkCommandBufferFactory::DestroyCommandBuffer(uint32_t bufferId, const VkQueueFlags& queueType)
{
    auto& wrapper = GetPoolWrapper(queueType);

    auto it = wrapper.m_cmdBufferList.find(bufferId);
    ASSERT_MSG_DEBUG(it != wrapper.m_cmdBufferList.end(), "Buf id not found");

    vkFreeCommandBuffers(DeviceInfo::GetLogicalDevice(), wrapper.m_pool, 1, &wrapper.m_cmdBufferList[bufferId].m_cmdBuffer);
    wrapper.m_cmdBufferList.erase(it);
}

void GfxVk::CommandPool::VkCommandBufferFactory::DestroyCommandBufferCustomPool(uint32_t bufferId, uint32_t poolId)
{
    auto it = m_additionsPoolList.find(poolId);
    ASSERT_MSG_DEBUG(it != m_additionsPoolList.end(), "Pool id not found");

    auto itt = it->second.m_cmdBufferList.find(bufferId);
    ASSERT_MSG_DEBUG(itt != it->second.m_cmdBufferList.end(), "Buffer id not found");

    vkFreeCommandBuffers(DeviceInfo::GetLogicalDevice(), (it)->second.m_pool, 1, &itt->second.m_cmdBuffer);
    (it)->second.m_cmdBufferList.erase(itt);
}

const VkCommandBuffer& GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(uint32_t bufferId, uint32_t poolId)
{
    ASSERT_MSG_DEBUG(m_additionsPoolList.find(poolId) != m_additionsPoolList.end(), "Pool id not found");
    ASSERT_MSG_DEBUG(m_additionsPoolList[poolId].m_cmdBufferList.find(bufferId) != m_additionsPoolList[poolId].m_cmdBufferList.end(), "Buffer id not found");
    return m_additionsPoolList[poolId].m_cmdBufferList[bufferId].m_cmdBuffer;
}

const VkCommandBuffer& GfxVk::CommandPool::VkCommandBufferFactory::GetCommandBuffer(uint32_t bufferId, const VkQueueFlagBits& queueType)
{
    switch (queueType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
        ASSERT_MSG_DEBUG(m_graphicsPool.m_cmdBufferList.find(bufferId) != m_graphicsPool.m_cmdBufferList.end(), "Buffer id not found");
        return m_graphicsPool.m_cmdBufferList[bufferId].m_cmdBuffer;
        break;
    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
        ASSERT_MSG_DEBUG(m_computePool.m_cmdBufferList.find(bufferId) != m_computePool.m_cmdBufferList.end(), "Buffer id not found");
        return m_computePool.m_cmdBufferList[bufferId].m_cmdBuffer;
        break;
    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
        ASSERT_MSG_DEBUG(m_transferPool.m_cmdBufferList.find(bufferId) != m_transferPool.m_cmdBufferList.end(), "Buffer id not found");
        return m_transferPool.m_cmdBufferList[bufferId].m_cmdBuffer;
        break;
    default:
        ASSERT_MSG_DEBUG(0, "Pool not found");
        break;
    }

    return m_graphicsPool.m_cmdBufferList[bufferId].m_cmdBuffer;
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

    GfxVk::Utility::ErrorCheck(vkAllocateCommandBuffers(DeviceInfo::GetLogicalDevice(), 
        &info, wrapper->commandBuffer));

    bufferList.push_back(*wrapper);
}
*/
/*
GfxVk::CommandPool::VkDrawCommandBuffer * GfxVk::CommandPool::VkCommandBufferFactory::CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level,
    Core::Enums::PipelineType commandBufferType, uint32_t* ids)
{
    const VkCommandPool& pool = GetCommandPool(poolId);
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

    GfxVk::Utility::ErrorCheck(vkAllocateCommandBuffers(DeviceInfo::GetLogicalDevice(),
        &info, drawCommandBuffer->commandBuffer));

    drawCommandBufferList.push_back(drawCommandBuffer);

    return drawCommandBuffer;
}

void GfxVk::CommandPool::VkCommandBufferFactory::DestroyCommandPool(const uint32_t &  id)
{
    std::vector<VkCommandPoolWrapper>::iterator it;
    it = std::find_if(poolList.begin(), poolList.end(), [&](VkCommandPoolWrapper e) { return e.poolId == id; });

    ASSERT_MSG_DEBUG(it != poolList.end(), "Pool id not found");

    vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), *it->pool, DeviceInfo::GetAllocationCallback());
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

    vkFreeCommandBuffers(DeviceInfo::GetLogicalDevice(), *(*it)->pool, 1, (*it)->commandBuffer);
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
*/
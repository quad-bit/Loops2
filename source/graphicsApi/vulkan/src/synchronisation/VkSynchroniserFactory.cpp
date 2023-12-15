#include "synchronisation/VkSynchroniserFactory.h"
#include "utility/VulkanUtility.h"
#include <CorePrecompiled.h>

GfxVk::Sync::VkSynchroniserFactory* GfxVk::Sync::VkSynchroniserFactory::instance = nullptr;

void GfxVk::Sync::VkSynchroniserFactory::Init()
{
    PLOGD << "GfxVk::Sync::VkSynchroniserFactory init";
}

void GfxVk::Sync::VkSynchroniserFactory::DeInit()
{
    PLOGD << "VkSynchroniserFactory Deinit";

    fenceList.clear();
    semaphoreList.clear();
}

void GfxVk::Sync::VkSynchroniserFactory::Update()
{

}

GfxVk::Sync::VkSynchroniserFactory * GfxVk::Sync::VkSynchroniserFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Sync::VkSynchroniserFactory();
    }
    return instance;
}

GfxVk::Sync::VkSynchroniserFactory::~VkSynchroniserFactory()
{
}

uint32_t GfxVk::Sync::VkSynchroniserFactory::CreateFence(bool isFenceSignaled)
{
    FenceWrapper wrapper = {};
    wrapper.id = GetFenceId();

    VkFenceCreateInfo info = {};
    if (isFenceSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    GfxVk::Utility::ErrorCheck(vkCreateFence(DeviceInfo::GetLogicalDevice(), &info, 
        DeviceInfo::GetAllocationCallback(), &wrapper.fence));

    auto id = wrapper.id;
    fenceList.push_back(std::move(wrapper));

    return id;
}

void GfxVk::Sync::VkSynchroniserFactory::DestroyFence(uint32_t id)
{
    vkDestroyFence(DeviceInfo::GetLogicalDevice(), GetFence(id), DeviceInfo::GetAllocationCallback());
}

const VkFence& GfxVk::Sync::VkSynchroniserFactory::GetFence(uint32_t id)
{
    std::vector<FenceWrapper>::iterator it;
    it = std::find_if(fenceList.begin(), fenceList.end(), [&](FenceWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != fenceList.end(), "Fence id not found");
    return it->fence;
}

uint32_t GfxVk::Sync::VkSynchroniserFactory::Create_Semaphore(bool isSemaphoreSignaled)
{
    SemaphoreWrapper wrapper = {};
    wrapper.id = GetSemaphoreId();
    
    VkSemaphoreCreateInfo info = {};
    if (isSemaphoreSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    GfxVk::Utility::ErrorCheck(vkCreateSemaphore(DeviceInfo::GetLogicalDevice(), &info,
        DeviceInfo::GetAllocationCallback(), &wrapper.semaphore));
    
    auto id = wrapper.id;
    semaphoreList.push_back(std::move(wrapper));

    return id;
}

void GfxVk::Sync::VkSynchroniserFactory::DestroySemaphore(uint32_t id)
{
    vkDestroySemaphore(DeviceInfo::GetLogicalDevice(), GetSemaphore(id), DeviceInfo::GetAllocationCallback());
    //delete semaphore;
}

const VkSemaphore& GfxVk::Sync::VkSynchroniserFactory::GetSemaphore(uint32_t id)
{
    std::vector<SemaphoreWrapper>::iterator it;
    it = std::find_if(semaphoreList.begin(), semaphoreList.end(), [&](SemaphoreWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != semaphoreList.end(), "Image id not found");
    return it->semaphore;
}

std::vector<VkSemaphore> GfxVk::Sync::VkSynchroniserFactory::GetSemaphore(uint32_t* ids, uint32_t count)
{
    std::vector<VkSemaphore> semaphoreList;
    for (uint32_t i = 0; i < count; i++)
        semaphoreList.push_back(GetSemaphore(ids[i]));
    return semaphoreList;
}

uint32_t GfxVk::Sync::VkSynchroniserFactory::CreateBarrier(
    const std::vector<VkImageMemoryBarrier2>& imageBarriers,
    const std::vector<VkBufferMemoryBarrier2>& bufferBarriers,
    const std::vector<VkMemoryBarrier2>& memoryBarriers)
{
    DependencyWrapper dependency{};
    dependency.m_id = GetBarrierId();
    dependency.m_bufferBarriers = bufferBarriers;
    dependency.m_imageBarriers = imageBarriers;
    dependency.m_memoryBarriers = memoryBarriers;
    auto id = dependency.m_id;
    m_dependencyList.insert({ id, std::move(dependency) });

    VkDependencyInfo info{};
    info.pNext = nullptr;
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.bufferMemoryBarrierCount = (uint32_t)m_dependencyList[id].m_bufferBarriers.size();
    info.pBufferMemoryBarriers = m_dependencyList[id].m_bufferBarriers.data();
    info.imageMemoryBarrierCount = (uint32_t)m_dependencyList[id].m_imageBarriers.size();;
    info.pImageMemoryBarriers = m_dependencyList[id].m_imageBarriers.data();
    info.memoryBarrierCount = (uint32_t)m_dependencyList[id].m_memoryBarriers.size();;
    info.pMemoryBarriers = m_dependencyList[id].m_memoryBarriers.data();
    info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    m_dependencyList[id].m_dependencyInfo = info;

    return id;
}

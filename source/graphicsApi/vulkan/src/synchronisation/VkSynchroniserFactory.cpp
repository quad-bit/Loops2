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
    PLOGD << "GfxVk::Sync::VkSynchroniserFactory Deinit";

    for each(FenceWrapper wrapper in fenceList)
    {
        if (wrapper.fence != nullptr)
            delete wrapper.fence;
    }

    fenceList.clear();

    for each(SemaphoreWrapper wrapper in semaphoreList)
    {
        if (wrapper.semaphore != nullptr)
            delete wrapper.semaphore;
    }

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
    FenceWrapper wrapper;
    wrapper.fence = new VkFence;
    wrapper.id = GetFenceId();

    VkFenceCreateInfo info = {};
    if (isFenceSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    GfxVk::Utility::ErrorCheck(vkCreateFence(DeviceInfo::GetLogicalDevice(), &info, 
        DeviceInfo::GetAllocationCallback(), wrapper.fence));

    fenceList.push_back(wrapper);

    return wrapper.id;
}

void GfxVk::Sync::VkSynchroniserFactory::DestroyFence(uint32_t id)
{
    vkDestroyFence(DeviceInfo::GetLogicalDevice(), *GetFence(id), DeviceInfo::GetAllocationCallback());
}

VkFence * GfxVk::Sync::VkSynchroniserFactory::GetFence(uint32_t id)
{
    std::vector<FenceWrapper>::iterator it;
    it = std::find_if(fenceList.begin(), fenceList.end(), [&](FenceWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != fenceList.end(), "Image id not found");
    return it->fence;
}

uint32_t GfxVk::Sync::VkSynchroniserFactory::Create_Semaphore(bool isSemaphoreSignaled)
{
    SemaphoreWrapper wrapper;
    wrapper.semaphore = new VkSemaphore;
    wrapper.id = GetSemaphoreId();
    
    VkSemaphoreCreateInfo info = {};
    if (isSemaphoreSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    GfxVk::Utility::ErrorCheck(vkCreateSemaphore(DeviceInfo::GetLogicalDevice(), &info,
        DeviceInfo::GetAllocationCallback(), wrapper.semaphore));

    semaphoreList.push_back(wrapper);

    return wrapper.id;
}

void GfxVk::Sync::VkSynchroniserFactory::DestroySemaphore(uint32_t id)
{
    VkSemaphore * semaphore = GetSemaphore(id);
    vkDestroySemaphore(DeviceInfo::GetLogicalDevice(), *semaphore, DeviceInfo::GetAllocationCallback());
    //delete semaphore;
}

VkSemaphore* GfxVk::Sync::VkSynchroniserFactory::GetSemaphore(uint32_t id)
{
    std::vector<SemaphoreWrapper>::iterator it;
    it = std::find_if(semaphoreList.begin(), semaphoreList.end(), [&](SemaphoreWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != semaphoreList.end(), "Image id not found");
    return it->semaphore;
}

VkSemaphore * GfxVk::Sync::VkSynchroniserFactory::GetSemaphore(const uint32_t * id, const uint32_t & count)
{
    //return the first one's address. Care should be taken while creating multiple sems in a single call
    //TODO : Create function to handle creation of multiple semaphores simultaneously
    return GetSemaphore(id[0]);
}

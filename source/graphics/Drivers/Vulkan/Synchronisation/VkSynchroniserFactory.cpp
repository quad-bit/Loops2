#include "VkSynchroniserFactory.h"
#include "VulkanUtility.h"
#include <CorePrecompiled.h>


VkSynchroniserFactory* VkSynchroniserFactory::instance = nullptr;

void VkSynchroniserFactory::Init()
{
    PLOGD << "VkSynchroniserFactory init";
}

void VkSynchroniserFactory::DeInit()
{
    PLOGD << "VkSynchroniserFactory Deinit";

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

void VkSynchroniserFactory::Update()
{

}

VkSynchroniserFactory * VkSynchroniserFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkSynchroniserFactory();
    }
    return instance;
}

VkSynchroniserFactory::~VkSynchroniserFactory()
{
}

uint32_t VkSynchroniserFactory::CreateFence(bool isFenceSignaled)
{
    FenceWrapper wrapper;
    wrapper.fence = new VkFence;
    wrapper.id = GetFenceId();

    VkFenceCreateInfo info = {};
    if (isFenceSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    ErrorCheck(vkCreateFence(*CoreObjects::logicalDeviceObj, &info, 
        CoreObjects::pAllocator, wrapper.fence));

    fenceList.push_back(wrapper);

    return wrapper.id;
}

void VkSynchroniserFactory::DestroyFence(uint32_t id)
{
    vkDestroyFence(*CoreObjects::logicalDeviceObj, *GetFence(id), CoreObjects::pAllocator);
}

VkFence * VkSynchroniserFactory::GetFence(uint32_t id)
{
    std::vector<FenceWrapper>::iterator it;
    it = std::find_if(fenceList.begin(), fenceList.end(), [&](FenceWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != fenceList.end(), "Image id not found");
    return it->fence;
}

uint32_t VkSynchroniserFactory::Create_Semaphore(bool isSemaphoreSignaled)
{
    SemaphoreWrapper wrapper;
    wrapper.semaphore = new VkSemaphore;
    wrapper.id = GetSemaphoreId();
    
    VkSemaphoreCreateInfo info = {};
    if (isSemaphoreSignaled)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    ErrorCheck(vkCreateSemaphore(*CoreObjects::logicalDeviceObj, &info,
        CoreObjects::pAllocator, wrapper.semaphore));

    semaphoreList.push_back(wrapper);

    return wrapper.id;
}

void VkSynchroniserFactory::DestroySemaphore(uint32_t id)
{
    VkSemaphore * semaphore = GetSemaphore(id);
    vkDestroySemaphore(*CoreObjects::logicalDeviceObj, *semaphore, CoreObjects::pAllocator);
    //delete semaphore;
}

VkSemaphore* VkSynchroniserFactory::GetSemaphore(uint32_t id)
{
    std::vector<SemaphoreWrapper>::iterator it;
    it = std::find_if(semaphoreList.begin(), semaphoreList.end(), [&](SemaphoreWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != semaphoreList.end(), "Image id not found");
    return it->semaphore;
}

VkSemaphore * VkSynchroniserFactory::GetSemaphore(const uint32_t * id, const uint32_t & count)
{
    //return the first one's address. Care should be taken while creating multiple sems in a single call
    //TODO : Create function to handle creation of multiple semaphores simultaneously
    return GetSemaphore(id[0]);
}

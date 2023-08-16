#pragma once

#include <vulkan\vulkan.h>

void ErrorCheck(VkResult result);

struct CoreObjects
{
    static VkInstance * instanceObj;
    static VkPhysicalDevice * physicalDeviceObj;
    static VkDevice* logicalDeviceObj;
    static VkAllocationCallbacks * pAllocator;
    static VkFormat bestDepthFormat;
    static uint32_t renderQueueId, presentationQueuedId;
    static uint32_t computeQueueId, transferQueueId;
};

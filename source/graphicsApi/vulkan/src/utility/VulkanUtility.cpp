#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"
#include <Platform/Assertion.h>
#include <iostream>

//VkInstance * GfxVk::Utility::CoreObjects::instanceObj = nullptr;
//VkPhysicalDevice * GfxVk::Utility::CoreObjects::physicalDeviceObj = nullptr;
//VkDevice * GfxVk::Utility::CoreObjects::logicalDeviceObj = nullptr;
//VkAllocationCallbacks * GfxVk::Utility::CoreObjects::pAllocator = nullptr;
//VkFormat GfxVk::Utility::CoreObjects::bestDepthFormat = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
//uint32_t GfxVk::Utility::CoreObjects::renderQueueId = 0, GfxVk::Utility::CoreObjects::presentationQueuedId = 0;
//uint32_t GfxVk::Utility::CoreObjects::computeQueueId = 0, GfxVk::Utility::CoreObjects::transferQueueId = 0;

VkAllocationCallbacks* GfxVk::Utility::VulkanSettings::pAllocator = nullptr;

VkInstance GfxVk::Utility::VulkanDeviceInfo::m_instanceObj = VK_NULL_HANDLE;
VkPhysicalDevice GfxVk::Utility::VulkanDeviceInfo::m_physicalDeviceObj = VK_NULL_HANDLE;
VkDevice GfxVk::Utility::VulkanDeviceInfo::m_logicalDeviceObj = VK_NULL_HANDLE;
VkAllocationCallbacks* GfxVk::Utility::VulkanDeviceInfo::m_pAllocator = VK_NULL_HANDLE;
VkFormat GfxVk::Utility::VulkanDeviceInfo::m_bestDepthFormat;
VkSurfaceFormatKHR GfxVk::Utility::VulkanDeviceInfo::m_surfaceFormat;
VkSurfaceKHR GfxVk::Utility::VulkanDeviceInfo::m_surface;
VkSampleCountFlags GfxVk::Utility::VulkanDeviceInfo::m_maxUsableSampleCount;
VkSurfaceCapabilitiesKHR GfxVk::Utility::VulkanDeviceInfo::m_surfaceCapabilities;
VkPhysicalDeviceProperties GfxVk::Utility::VulkanDeviceInfo::m_physicalDeviceProps;
VkPhysicalDeviceMemoryProperties GfxVk::Utility::VulkanDeviceInfo::m_physicalDeviceMemProps;
VkPhysicalDeviceFeatures GfxVk::Utility::VulkanDeviceInfo::m_physicalDeviceFeatures;
VkPhysicalDeviceFeatures GfxVk::Utility::VulkanDeviceInfo::m_enabledPhysicalDeviceFeatures;
bool GfxVk::Utility::VulkanDeviceInfo::m_isSampleRateShadingAvailable;
VkQueue GfxVk::Utility::VulkanDeviceInfo::m_renderQueueObj = VK_NULL_HANDLE;
uint32_t GfxVk::Utility::VulkanDeviceInfo::m_surfaceWidth;
uint32_t GfxVk::Utility::VulkanDeviceInfo::m_surfaceHeight;

VkInstance GfxVk::Utility::VulkanDeviceInfo::GetVkInstance()
{
    return m_instanceObj;
}

VkPhysicalDevice GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDevice()
{
    return m_physicalDeviceObj;
}

VkDevice GfxVk::Utility::VulkanDeviceInfo::GetLogicalDevice()
{
    return m_logicalDeviceObj;
}

VkAllocationCallbacks* GfxVk::Utility::VulkanDeviceInfo::GetAllocationCallback()
{
    return m_pAllocator;
}

VkFormat GfxVk::Utility::VulkanDeviceInfo::GetDepthFormat()
{
    return m_bestDepthFormat;
}

VkSurfaceFormatKHR GfxVk::Utility::VulkanDeviceInfo::GetSurfaceFormat()
{
    return m_surfaceFormat;
}

VkSampleCountFlags GfxVk::Utility::VulkanDeviceInfo::GetMaxSampleCount()
{
    return m_maxUsableSampleCount;
}

VkPhysicalDeviceProperties GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDeviceProps()
{
    return m_physicalDeviceProps;
}

VkSurfaceCapabilitiesKHR GfxVk::Utility::VulkanDeviceInfo::GetSurfaceCapabilities()
{
    return m_surfaceCapabilities;
}

VkPhysicalDeviceMemoryProperties GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDeviceMemProps()
{
    return m_physicalDeviceMemProps;
}

VkPhysicalDeviceFeatures GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDeviceFeatures()
{
    return m_physicalDeviceFeatures;
}

VkPhysicalDeviceFeatures GfxVk::Utility::VulkanDeviceInfo::GetEnabledPhysicalDeviceFeatures()
{
    return m_enabledPhysicalDeviceFeatures;
}

bool GfxVk::Utility::VulkanDeviceInfo::IsSampleRateShadingAvailable()
{
    return m_isSampleRateShadingAvailable;
}

VkQueue GfxVk::Utility::VulkanDeviceInfo::GetRenderQueue()
{
    return m_renderQueueObj;
}


void GfxVk::Utility::ErrorCheck(VkResult result)
{
#if BUILD_ENABLE_VULKAN_RUNTIME_DEBUG
    if (result < 0) {
        switch (result) {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            std::cout << "VK_ERROR_OUT_OF_HOST_MEMORY" << std::endl;
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            std::cout << "VK_ERROR_OUT_OF_DEVICE_MEMORY" << std::endl;
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            std::cout << "VK_ERROR_INITIALIZATION_FAILED" << std::endl;
            break;
        case VK_ERROR_DEVICE_LOST:
            std::cout << "VK_ERROR_DEVICE_LOST" << std::endl;
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            std::cout << "VK_ERROR_MEMORY_MAP_FAILED" << std::endl;
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            std::cout << "VK_ERROR_LAYER_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            std::cout << "VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            std::cout << "VK_ERROR_FEATURE_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER" << std::endl;
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            std::cout << "VK_ERROR_TOO_MANY_OBJECTS" << std::endl;
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            std::cout << "VK_ERROR_FORMAT_NOT_SUPPORTED" << std::endl;
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            std::cout << "VK_ERROR_SURFACE_LOST_KHR" << std::endl;
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            std::cout << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR" << std::endl;
            break;
        case VK_SUBOPTIMAL_KHR:
            std::cout << "VK_SUBOPTIMAL_KHR" << std::endl;
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            std::cout << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            std::cout << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR" << std::endl;
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            std::cout << "VK_ERROR_VALIDATION_FAILED_EXT" << std::endl;
            break;
        default:
            break;
        }

        ASSERT_MSG_DEBUG(0, "Vulkan runtime error.");
    }
#endif
}

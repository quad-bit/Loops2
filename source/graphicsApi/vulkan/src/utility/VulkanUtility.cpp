#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"
#include <Platform/Assertion.h>
#include <iostream>

//using namespace std;

VkInstance * GfxVk::Utility::CoreObjects::instanceObj = nullptr;
VkPhysicalDevice * GfxVk::Utility::CoreObjects::physicalDeviceObj = nullptr;
VkDevice * GfxVk::Utility::CoreObjects::logicalDeviceObj = nullptr;
VkAllocationCallbacks * GfxVk::Utility::CoreObjects::pAllocator = nullptr;
VkFormat GfxVk::Utility::CoreObjects::bestDepthFormat = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
uint32_t GfxVk::Utility::CoreObjects::renderQueueId = 0, GfxVk::Utility::CoreObjects::presentationQueuedId = 0;
uint32_t GfxVk::Utility::CoreObjects::computeQueueId = 0, GfxVk::Utility::CoreObjects::transferQueueId = 0;

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

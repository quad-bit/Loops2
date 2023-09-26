#pragma once

#include <vulkan/vulkan.h>

namespace GfxVk
{
    namespace Utility
    {
        void ErrorCheck(VkResult result);

        struct VulkanSettings
        {
            static VkAllocationCallbacks* pAllocator;
        };

        class VulkanManager;
        class VulkanDeviceInfo
        {
        private:
            static VkInstance m_instanceObj;
            static VkPhysicalDevice m_physicalDeviceObj;
            static VkDevice m_logicalDeviceObj;
            static VkAllocationCallbacks* m_pAllocator;
            static VkFormat m_bestDepthFormat;
            static VkSurfaceFormatKHR m_surfaceFormat;
            static VkSurfaceKHR m_surface;
            static VkSampleCountFlags m_maxUsableSampleCount;
            static VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
            static VkPhysicalDeviceProperties m_physicalDeviceProps;
            static VkPhysicalDeviceMemoryProperties m_physicalDeviceMemProps;
            static VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
            static VkPhysicalDeviceFeatures m_enabledPhysicalDeviceFeatures;
            static bool m_isSampleRateShadingAvailable;
            static VkQueue m_renderQueueObj;
            static uint32_t m_surfaceWidth;
            static uint32_t m_surfaceHeight;

        public:
            friend class VulkanManager;

            static VkInstance GetVkInstance();
            static VkPhysicalDevice GetPhysicalDevice();
            static VkDevice GetLogicalDevice();
            static VkAllocationCallbacks* GetAllocationCallback();
            static VkFormat GetDepthFormat();
            static VkSurfaceKHR GetSurface();
            static VkSurfaceFormatKHR GetSurfaceFormat();
            static VkSampleCountFlags GetMaxSampleCount();
            static VkSurfaceCapabilitiesKHR GetSurfaceCapabilities();
            static VkPhysicalDeviceProperties GetPhysicalDeviceProps();
            static VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemProps();
            static VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures();
            static VkPhysicalDeviceFeatures GetEnabledPhysicalDeviceFeatures();
            static bool IsSampleRateShadingAvailable();
            static VkQueue GetRenderQueue();
        };
    }
}

typedef GfxVk::Utility::VulkanDeviceInfo DeviceInfo;
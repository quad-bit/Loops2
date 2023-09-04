#pragma once
#include <vulkan/vulkan.h>
#include <GLFW\glfw3.h>
#include "Utility/RenderingWrappers/RenderingWrapper.h"

namespace Core 
{
    namespace Wrappers
    {
        struct QueueWrapper;
    }
}

namespace GfxVk
{
    namespace Utility
    {
        class ValidationManager;

        class VulkanManager
        {
        private:
            VulkanManager();
            VulkanManager(VulkanManager const&) {}
            VulkanManager const& operator= (VulkanManager const&) {}

            static VulkanManager* instance;

            ValidationManager* validationManagerObj;

            VkInstance vkInstanceObj = VK_NULL_HANDLE;
            VkDevice vkLogicalDeviceObj = VK_NULL_HANDLE;
            VkPhysicalDevice vkPhysicalDeviceObj = VK_NULL_HANDLE;
            VkAllocationCallbacks* pAllocator = VK_NULL_HANDLE;
            VkQueue graphicsQueueObj = VK_NULL_HANDLE;
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            uint32_t surfaceWidth;
            uint32_t surfaceHeight;

            VkSurfaceFormatKHR surfaceFormat = {};
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

            VkPhysicalDeviceProperties                      physicalDeviceProps{};
            VkPhysicalDeviceMemoryProperties                physicalDeviceMemProps{};
            VkPhysicalDeviceFeatures                        physicalDeviceFeatures{}, enabledPhysicalDeviceFeatures{};
            //uint32_t                                        vulkanGraphicsQueueFamilyIndex;

            void CreateInstance();
            void CreateLogicalDevice();

            void GetPhysicalDevice();

        public:
            static VulkanManager* GetInstance();
            ~VulkanManager();

            void Init();
            void Init(Core::Wrappers::QueueWrapper* queueRequirement, const uint32_t& count);
            void InitializeFactories();
            void DeInit();
            void Update();
            void CreateSurface(GLFWwindow* glfwWindow);
            VkSurfaceKHR* GetSurface() { return &surface; }
            VkSurfaceFormatKHR* GetSurfaceFormat() { return &surfaceFormat; }

            //const uint32_t&                                 GetQueueFamilyIndex() const { return vulkanGraphicsQueueFamilyIndex; }
            const VkAllocationCallbacks* GetAllocator() const { return pAllocator; }
            const VkPhysicalDeviceMemoryProperties& GetPhyicalDeviceMemProps() const { return physicalDeviceMemProps; }
            const VkQueue& GetGraphicsQueue() const { return graphicsQueueObj; }
            const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return physicalDeviceFeatures; }

            VkPhysicalDeviceFeatures& GetEnabledPhysicalDeviceFeatures() { return enabledPhysicalDeviceFeatures; }
            const VkPhysicalDeviceProperties& GetPhysicalDeviceProps() { return physicalDeviceProps; }

            bool IsSampleRateShadingAvailable();
            VkSampleCountFlagBits GetMaxUsableVKSampleCount();
            Core::Enums::Samples GetMaxUsableSampleCount();
        };
    }
}
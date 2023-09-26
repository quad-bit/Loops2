#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include <Settings.h>

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
            VulkanManager() = delete;
            VulkanManager(VulkanManager const&) = delete;
            VulkanManager const& operator= (VulkanManager const&) = delete;

            ValidationManager* m_validationManagerObj;
            const Core::WindowSettings& m_windowSettings;

            VkSurfaceKHR m_surface = VK_NULL_HANDLE;
            VkInstance vkInstanceObj = VK_NULL_HANDLE;

            /*VkInstance vkInstanceObj = VK_NULL_HANDLE;
            VkDevice vkLogicalDeviceObj = VK_NULL_HANDLE;
            VkPhysicalDevice vkPhysicalDeviceObj = VK_NULL_HANDLE;
            VkAllocationCallbacks* pAllocator = VK_NULL_HANDLE;
            VkQueue graphicsQueueObj = VK_NULL_HANDLE;
            uint32_t surfaceWidth;
            uint32_t surfaceHeight;

            VkSurfaceFormatKHR surfaceFormat = {};
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

            VkPhysicalDeviceProperties                      physicalDeviceProps{};
            VkPhysicalDeviceMemoryProperties                physicalDeviceMemProps{};
            VkPhysicalDeviceFeatures                        physicalDeviceFeatures{}, enabledPhysicalDeviceFeatures{};
            */
            //uint32_t                                        vulkanGraphicsQueueFamilyIndex;

            void CreateInstance();
            void CreateLogicalDevice();
            void GetPhysicalDevice();
            void GetMaxUsableVKSampleCount();

        public:
            ~VulkanManager();
            VulkanManager(const Core::WindowSettings& windowSettings);

            //void Init();
            //void Init(Core::Wrappers::QueueWrapper* queueRequirement, const uint32_t& count);
            void Init(std::vector<Core::Wrappers::QueueWrapper>& queueRequirements,
                      uint32_t& renderQueueId, uint32_t& presentationQueueId, uint32_t& computeQueueId, uint32_t& transferQueueId);

            void InitializeFactories();
            void DeInit();
            void Update();
            void CreateSurface(GLFWwindow* glfwWindow);
            //VkSurfaceKHR* GetSurface() { return &surface; }
            //VkSurfaceFormatKHR* GetSurfaceFormat() { return &surfaceFormat; }

            ////const uint32_t&                                 GetQueueFamilyIndex() const { return vulkanGraphicsQueueFamilyIndex; }
            //const VkAllocationCallbacks* GetAllocator() const { return pAllocator; }
            //const VkPhysicalDeviceMemoryProperties& GetPhyicalDeviceMemProps() const { return physicalDeviceMemProps; }
            //const VkQueue& GetGraphicsQueue() const { return graphicsQueueObj; }
            //const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return physicalDeviceFeatures; }

            //VkPhysicalDeviceFeatures& GetEnabledPhysicalDeviceFeatures() { return enabledPhysicalDeviceFeatures; }
            //const VkPhysicalDeviceProperties& GetPhysicalDeviceProps() { return physicalDeviceProps; }

            //bool IsSampleRateShadingAvailable();
            //Core::Enums::Samples GetMaxUsableSampleCount();

        };
    }
}

#pragma once
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include <vector>
#include "Utility/RenderingWrappers/RenderingWrapper.h"

namespace GfxVk
{
    namespace Utility
    {
        class VulkanManager;

        struct VkMemoryWrapper
        {
            VkDeviceMemory memory;
            uint32_t id;
            VkDeviceSize memorySize;
            bool isShared;
            bool isActive;
        };

        class VulkanMemoryManager
        {
        private:
            VulkanMemoryManager();
            VulkanMemoryManager(VulkanMemoryManager const&) {};
            VulkanMemoryManager const& operator=(VulkanMemoryManager const&) {};
            static VulkanMemoryManager* memManagerInstance;

            VkPhysicalDeviceMemoryProperties physicalDeviceMemoryPropertiesObj{};
            VmaAllocator allocator;
            VkDevice vulkanLogicalDevice;
            uint32_t FindMemoryTypeIndex(
                const VkPhysicalDeviceMemoryProperties* gpu_memory_properties,
                const VkMemoryRequirements& memory_requirements,
                const VkMemoryPropertyFlags& required_properties);

            uint32_t idCounter = 0;
            uint32_t GetId();
            std::vector<VkMemoryWrapper> memoryWrapperList;

        public:
            ~VulkanMemoryManager();
            static VulkanMemoryManager* GetSingleton();
            void Init(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryPropertiesObj);
            void DeInit();

            VkMemoryRequirements GetImageMemoryRequirement(const VkImage& image);

            uint32_t AllocateMemory(const VkMemoryRequirements& memReq, const VkMemoryPropertyFlags& userReq, VkDeviceMemory& memoryObj, const std::optional<VkDeviceSize>& allocationSize);
            uint32_t AllocateMemory(const VkMemoryRequirements& memReq, const VkMemoryPropertyFlags& userReq, const std::optional<VkDeviceSize>& allocationSize);

            void FreeMemory(uint32_t id);
            const VkDeviceMemory& GetDeviceMemory(const uint32_t& memId);
        };
    }
}
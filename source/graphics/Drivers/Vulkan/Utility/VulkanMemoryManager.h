#pragma once
#include <vulkan\vulkan.h>
#include "vk_mem_alloc.h"
#include <vector>
#include "RenderingWrapper.h"

class VulkanManager;

struct VkMemoryWrapper
{
    VkDeviceMemory * memory;
    uint32_t id;
    VkDeviceSize memorySize;
    bool isShared;
    bool isActive;
};
	
class VulkanMemoryManager
{
private:
	VulkanMemoryManager();
	VulkanMemoryManager(VulkanMemoryManager const &) {};
	VulkanMemoryManager const & operator=(VulkanMemoryManager const &) {};
	static VulkanMemoryManager* memManagerInstance;

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryPropertiesObj{};
	VmaAllocator allocator;
	VkDevice vulkanLogicalDevice;
	uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * gpu_memory_properties, const VkMemoryRequirements * memory_requirements, const VkMemoryPropertyFlags required_properties);

    uint32_t idCounter = 0;
    uint32_t GetId();
    std::vector<VkMemoryWrapper> memoryWrapperList;

public:
	~VulkanMemoryManager();
	static VulkanMemoryManager* GetSingleton();
	void Init(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryPropertiesObj);
	void DeInit();

	//deprecated.	
	void AllocateImageMemory(VkImage * imageObj, VkMemoryPropertyFlags userReq, VkDeviceMemory * memoryObj);
    //deprecated.
    VkMemoryRequirements AllocateBufferMemory(VkBuffer * bufferObj, VkMemoryPropertyFlags userReq, VkDeviceMemory * memoryObj);
    

    VkMemoryRequirements GetImageMemoryRequirement(VkImage * image);
    uint32_t AllocateMemory(VkMemoryRequirements * memReq, VkMemoryPropertyFlags userReq, VkDeviceMemory * memoryObj);
    uint32_t AllocateMemory(VkMemoryRequirements * memReq, VkMemoryPropertyFlags userReq, VkDeviceMemory * memoryObj, VkDeviceSize allocationSize);
    
    uint32_t AllocateMemory(VkMemoryRequirements * memReq, VkMemoryPropertyFlags userReq);
    uint32_t AllocateMemory(VkMemoryRequirements * memReq, VkMemoryPropertyFlags userReq, VkDeviceSize allocationSize);

    uint32_t AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq, const size_t & allocationSize);
    uint32_t AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq);

    void FreeMemory(uint32_t id);

    VkDeviceMemory * GetDeviceMemory(const uint32_t & memId);
};


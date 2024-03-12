#define VMA_IMPLEMENTATION

#include "utility/VulkanMemoryManager.h"
#include "utility/VulkanUtility.h"
#include "utility/VkRenderingUnwrapper.h"
#include <CorePrecompiled.h>

GfxVk::Utility::VulkanMemoryManager * GfxVk::Utility::VulkanMemoryManager::memManagerInstance = nullptr;

GfxVk::Utility::VulkanMemoryManager * GfxVk::Utility::VulkanMemoryManager::GetSingleton()
{
    if (memManagerInstance == nullptr)
        memManagerInstance = new GfxVk::Utility::VulkanMemoryManager;
    return memManagerInstance;
}

void GfxVk::Utility::VulkanMemoryManager::Init(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryPropertiesObj)
{
    PLOGD << "VulkanMemoryManager init";

    this->physicalDeviceMemoryPropertiesObj = physicalDeviceMemoryPropertiesObj;
    vulkanLogicalDevice = DeviceInfo::GetLogicalDevice();

    /*VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = DeviceInfo::GetPhysicalDevice();
    allocatorInfo.device = DeviceInfo::GetLogicalDevice();
    allocatorInfo.instance = DeviceInfo::GetVkInstance();

    vmaCreateAllocator(&allocatorInfo, &allocator);*/

    /*VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = 65536;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkBuffer buffer;
    VmaAllocation allocation;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    */


    //// Create a pool that can have at most 2 blocks, 128 MiB each.
    //VmaPoolCreateInfo poolCreateInfo = {};
    //poolCreateInfo.memoryTypeIndex = ...
    //poolCreateInfo.blockSize = 128ull * 1024 * 1024;
    //poolCreateInfo.maxBlockCount = 2;

    //VmaPool pool;
    //vmaCreatePool(allocator, &poolCreateInfo, &pool);
}

void GfxVk::Utility::VulkanMemoryManager::DeInit()
{
    PLOGD << "VulkanMemoryManager Deinit";
    //vmaDestroyAllocator(allocator);
    for (uint32_t i = 0; i < memoryWrapperList.size(); i++)
    {
        vkFreeMemory(DeviceInfo::GetLogicalDevice(), memoryWrapperList[i].memory, DeviceInfo::GetAllocationCallback());
    }
    memoryWrapperList.clear();
}

uint32_t GfxVk::Utility::VulkanMemoryManager::FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * gpu_memory_properties, const VkMemoryRequirements& memory_requirements, const VkMemoryPropertyFlags& required_properties)
{
    for (uint32_t i = 0; i < gpu_memory_properties->memoryTypeCount; ++i) {
        if (memory_requirements.memoryTypeBits & (1 << i)) {
            if ((gpu_memory_properties->memoryTypes[i].propertyFlags & required_properties) == required_properties) {
                return i;
            }
        }
    }
    assert(0 && "Couldn't find proper memory type.");
    return UINT32_MAX;
}

uint32_t GfxVk::Utility::VulkanMemoryManager::GetId()
{
    return idCounter++;
}

VkMemoryRequirements GfxVk::Utility::VulkanMemoryManager::GetImageMemoryRequirement(const VkImage& image)
{
    VkMemoryRequirements memoryReqObj{};
    vkGetImageMemoryRequirements(vulkanLogicalDevice, image, &memoryReqObj);

    return memoryReqObj;
}

uint32_t GfxVk::Utility::VulkanMemoryManager::AllocateMemory(const VkMemoryRequirements& memReq, const VkMemoryPropertyFlags& userReq, VkDeviceMemory& memoryObj, const std::optional<VkDeviceSize>& allocationSize)
{
    uint32_t memIndex = GfxVk::Utility::VulkanMemoryManager::FindMemoryTypeIndex(&physicalDeviceMemoryPropertiesObj, memReq, userReq);
    // found memory index..

    auto allocSize = allocationSize.has_value() ? allocationSize.value() : memReq.size;

    VkMemoryAllocateInfo allocateInfoObj{};
    allocateInfoObj.allocationSize = allocSize;
    allocateInfoObj.memoryTypeIndex = memIndex;
    allocateInfoObj.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    ErrorCheck(vkAllocateMemory(vulkanLogicalDevice, &allocateInfoObj, DeviceInfo::GetAllocationCallback(), &memoryObj));

    VkMemoryWrapper wrapper = {};
    wrapper.memory = memoryObj;
    wrapper.memorySize = allocSize;
    wrapper.id = GetId();

    memoryWrapperList.push_back(wrapper);

    return wrapper.id;
}

uint32_t GfxVk::Utility::VulkanMemoryManager::AllocateMemory(const VkMemoryRequirements& memReq, const VkMemoryPropertyFlags& userReq, const std::optional<VkDeviceSize>& allocationSize)
{
    VkDeviceMemory * memoryObj = new VkDeviceMemory;
    uint32_t memIndex = GfxVk::Utility::VulkanMemoryManager::FindMemoryTypeIndex(&physicalDeviceMemoryPropertiesObj, memReq, userReq);
    // found memory index..

    auto allocSize = allocationSize.has_value() ? allocationSize.value() : memReq.size;

    VkMemoryAllocateInfo allocateInfoObj{};
    allocateInfoObj.allocationSize = allocSize;
    allocateInfoObj.memoryTypeIndex = memIndex;
    allocateInfoObj.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    ErrorCheck(vkAllocateMemory(vulkanLogicalDevice, &allocateInfoObj, DeviceInfo::GetAllocationCallback(), memoryObj));

    VkMemoryWrapper wrapper = {};
    wrapper.memory = *memoryObj;
    wrapper.memorySize = allocSize;
    wrapper.id = GetId();

    memoryWrapperList.push_back(wrapper);

    return wrapper.id;
}

void GfxVk::Utility::VulkanMemoryManager::FreeMemory(uint32_t id)
{
    std::vector<VkMemoryWrapper>::iterator it;
    it = std::find_if(memoryWrapperList.begin(), memoryWrapperList.end(), [&](VkMemoryWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != memoryWrapperList.end(), "Memory id not found");

    vkFreeMemory(DeviceInfo::GetLogicalDevice(), it->memory, DeviceInfo::GetAllocationCallback());

    memoryWrapperList.erase(it);
}

const VkDeviceMemory& GfxVk::Utility::VulkanMemoryManager::GetDeviceMemory(const uint32_t & memId)
{
    std::vector<VkMemoryWrapper>::iterator it;
    it = std::find_if(memoryWrapperList.begin(), memoryWrapperList.end(), [&](VkMemoryWrapper e) { return e.id == memId; });

    return it->memory;
}

GfxVk::Utility::VulkanMemoryManager::VulkanMemoryManager()
{
}

GfxVk::Utility::VulkanMemoryManager::~VulkanMemoryManager()
{
}

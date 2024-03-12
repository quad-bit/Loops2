#include "utility/VkBufferFactory.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"
#include "utility/VulkanMemoryManager.h"
#include <CorePrecompiled.h>
#include "VulkanManager.h"
#include "utility/VkRenderingUnwrapper.h"
#include <utility/VkDebugMarkerUtil.h>

GfxVk::Utility::VkBufferFactory* GfxVk::Utility::VkBufferFactory::s_instance = nullptr;
uint32_t GfxVk::Utility::VkBufferFactory::s_bufferIdCounter= 0;

uint32_t GfxVk::Utility::VkBufferFactory::GetId()
{
    return s_bufferIdCounter++;
}

std::map<uint32_t, GfxVk::Utility::VkBufferWrapper>::iterator GfxVk::Utility::VkBufferFactory::FindBuffer(uint32_t id)
{
    auto it = m_bufferList.find(id);
    ASSERT_MSG_DEBUG(it != m_bufferList.end(), "Buffer not found");

    return it;
}

void GfxVk::Utility::VkBufferFactory::Init()
{
    PLOGD << "VKBUfferFactory init";
    //physicalDeviceProps = GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDeviceProps();
}

void GfxVk::Utility::VkBufferFactory::DeInit()
{
    PLOGD << "VKBUfferFactory Deinit";
}

GfxVk::Utility::VkBufferFactory* GfxVk::Utility::VkBufferFactory::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new GfxVk::Utility::VkBufferFactory();
    }
    return s_instance;
}

GfxVk::Utility::VkBufferFactory::~VkBufferFactory()
{
}

uint32_t GfxVk::Utility::VkBufferFactory::CreateBuffer(const VkBufferCreateInfo& info, const std::string& name)
{
    GfxVk::Utility::VkBufferWrapper bufWrapper = {};
    bufWrapper.m_id = GetId();
    bufWrapper.m_bufferUsage = info.usage;

    GfxVk::Utility::ErrorCheck(vkCreateBuffer(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &bufWrapper.m_buffer));
    vkGetBufferMemoryRequirements(DeviceInfo::GetLogicalDevice(), bufWrapper.m_buffer, &bufWrapper.m_bufMemReq);
    m_bufferList.insert({ bufWrapper.m_id, bufWrapper });

    GfxVk::DebugMarker::SetObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)(bufWrapper.m_buffer), name.c_str());
    return bufWrapper.m_id;
}

uint32_t GfxVk::Utility::VkBufferFactory::AllocateBindBufferMemory(
    const VkBuffer& buffer,
    const VkMemoryPropertyFlags& userReq,
    const VkMemoryRequirements& bufferMemReq,
    std::optional<size_t> allocationSize)
{
    uint32_t id = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(bufferMemReq, userReq, std::nullopt);
    auto& bufferMemory = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(id);
    vkBindBufferMemory(DeviceInfo::GetLogicalDevice(), buffer, bufferMemory, 0);
    return id;
}

uint32_t GfxVk::Utility::VkBufferFactory::AllocateBindBufferMemory(
    uint32_t bufferId,
    const VkMemoryPropertyFlags& userReq,
    bool isMemoryShared,
    std::optional<size_t> allocationSize)
{
    auto it = FindBuffer(bufferId);
    auto& buf = it->second.m_buffer;
    it->second.m_memFlags = userReq;

    size_t allocSize = 0;
    if (allocationSize.has_value())
    {
        allocSize = allocationSize.value();
    }
    else
    {
        allocSize = it->second.m_bufMemReq.size;
    }

    auto memId = AllocateBindBufferMemory(buf, userReq, it->second.m_bufMemReq, allocSize);
    it->second.m_isBufferSharingMemory = isMemoryShared;
    it->second.m_bufferMemoryId = memId;

    return memId;
}

void GfxVk::Utility::VkBufferFactory::BindBufferMemory(
    const VkBuffer& buffer,
    const VkDeviceMemory& memory,
    const VkDeviceSize& offset)
{
    vkBindBufferMemory(DeviceInfo::GetLogicalDevice(), buffer, memory, offset);
}

void GfxVk::Utility::VkBufferFactory::BindBufferMemory(
    uint32_t bufferId,
    uint32_t memId,
    const size_t& offset,
    bool isMemoryShared)
{
    auto it = FindBuffer(bufferId);
    auto& buffer = it->second.m_buffer;
    auto& memory = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);
    BindBufferMemory(buffer, memory, offset);
    it->second.m_bufferMemoryId = memId;
    it->second.m_isBufferSharingMemory = isMemoryShared;
    it->second.m_memoryOffset = offset;
}

void GfxVk::Utility::VkBufferFactory::DestroyBuffer(uint32_t id, bool freeMemory)
{
    auto it = FindBuffer(id);
    vkDestroyBuffer(DeviceInfo::GetLogicalDevice(), it->second.m_buffer, DeviceInfo::GetAllocationCallback());

    if (freeMemory)
    {
        ASSERT_MSG_DEBUG(it->second.m_isBufferSharingMemory != true, "Cant free shared memory");
        GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory(it->second.m_bufferMemoryId.value());
    }

    m_bufferList.erase(it);
}

const VkBuffer& GfxVk::Utility::VkBufferFactory::GetBuffer(const uint32_t& id)
{
    auto it = FindBuffer(id);
    return it->second.m_buffer;
}

Core::Wrappers::MemoryRequirementInfo GfxVk::Utility::VkBufferFactory::GetBufferMemoryRequirement(uint32_t id)
{
    auto it = FindBuffer(id);
    auto& memReq = it->second.m_bufMemReq;

    Core::Wrappers::MemoryRequirementInfo req{};
    req.alignment = memReq.alignment;
    req.memoryTypeBits = memReq.memoryTypeBits;
    req.size = memReq.size;

    return req;
}

size_t GfxVk::Utility::VkBufferFactory::GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize)
{
    size_t alignedDataSize = dataSize;
    auto& physicalDeviceProps = GfxVk::Utility::VulkanDeviceInfo::GetPhysicalDeviceProps();
    VkDeviceSize minUniformAlignment = physicalDeviceProps.limits.minUniformBufferOffsetAlignment;
    if (minUniformAlignment)
        alignedDataSize = (dataSize + minUniformAlignment - 1) & ~(minUniformAlignment - 1);
    return alignedDataSize;
}

void GfxVk::Utility::VkBufferFactory::CopyBufferDataToMemory(const uint32_t& bufId, const VkDeviceSize& dataSize, const VkDeviceSize& memAlignedSize, void* data, const VkDeviceSize& memoryMapOffset, bool keepMemoryMounted)
{
    auto it = FindBuffer(bufId);
    auto& bufferMemory = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(it->second.m_bufferMemoryId.value());

    void* gpuMem;
    GfxVk::Utility::ErrorCheck(vkMapMemory(DeviceInfo::GetLogicalDevice(), bufferMemory, memoryMapOffset,
        memAlignedSize, 0, (void**)&gpuMem));
    memcpy(gpuMem, data, dataSize);
    if(!keepMemoryMounted)
        vkUnmapMemory(DeviceInfo::GetLogicalDevice(), bufferMemory);
}

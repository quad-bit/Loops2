#include "VkBufferFactory.h"
#include "VulkanUtility.h"
#include "VulkanSettings.h"
#include "VulkanMemoryManager.h"
#include <CorePrecompiled.h>
#include "VulkanManager.h"
#include "VkRenderingUnwrapper.h"

VkBufferFactory* VkBufferFactory::instance = nullptr;

uint32_t VkBufferFactory::GetId()
{
    return idCounter++;
}

void VkBufferFactory::Init()
{
    PLOGD << "VKBUfferFactory init";
    physicalDeviceProps = VulkanManager::GetInstance()->GetPhysicalDeviceProps();

}

void VkBufferFactory::DeInit()
{
    PLOGD << "VKBUfferFactory Deinit";

}

void VkBufferFactory::Update()
{

}

VkBufferFactory * VkBufferFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkBufferFactory();
    }
    return instance;
}

VkBufferFactory::~VkBufferFactory()
{
}

/*
// deprecated.
uint32_t * VkBufferFactory::CreateBuffer(VkBufferUsageFlags * bufferType,
    VkMemoryPropertyFlags * memType, void * data, size_t dataSize, void * pGpuMem)
{
    {
    VkBufferWrapper bufWrapper = {};
    bufWrapper.buffer = new VkBuffer;
    bufWrapper.id = GetId();
    bufWrapper.bufferMemory = new VkDeviceMemory;
    bufWrapper.bufferType = *bufferType;
    bufWrapper.pGpuMem = nullptr;

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.usage = *bufferType;
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    ErrorCheck(vkCreateBuffer(*CoreObjects::logicalDeviceObj, &bufferCreateInfo, CoreObjects::pAllocator, bufWrapper.buffer));

    VkMemoryRequirements memoryReqObj = VulkanMemoryManager::GetSingleton()->AllocateBufferMemory(bufWrapper.buffer, *memType, bufWrapper.bufferMemory);
    bufWrapper.bufMemReq = memoryReqObj;

    ErrorCheck(vkMapMemory(*CoreObjects::logicalDeviceObj, *bufWrapper.bufferMemory, 0,
        memoryReqObj.size, 0, (void**)&bufWrapper.pGpuMem));
    memcpy(bufWrapper.pGpuMem, data, dataSize);

    vkUnmapMemory(*CoreObjects::logicalDeviceObj, *bufWrapper.bufferMemory);

    vkBindBufferMemory(*CoreObjects::logicalDeviceObj, *bufWrapper.buffer, *bufWrapper.bufferMemory, 0);

    bufferWrapperList.push_back(bufWrapper);
    pGpuMem = bufferWrapperList[bufferWrapperList.size() - 1].pGpuMem;
    }
    return &bufferWrapperList[bufferWrapperList.size() - 1].id;
}
*/

uint32_t * VkBufferFactory::CreateBuffers(const uint32_t & bufferCount, VkBufferUsageFlags * bufferType, VkMemoryPropertyFlags * memType, size_t * dataSizes)
{
    uint32_t * ids = new uint32_t[bufferCount];

    for (uint32_t i = 0; i < bufferCount; i++)
    {
        VkBufferWrapper bufWrapper = {};
        bufWrapper.id = GetId();
        bufWrapper.buffer = new VkBuffer;
        bufWrapper.bufferType = bufferType[i];
        bufWrapper.memFlags = memType[i];

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.size = dataSizes[i];
        bufferCreateInfo.usage = bufferType[i];
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ErrorCheck(vkCreateBuffer(*CoreObjects::logicalDeviceObj, &bufferCreateInfo, CoreObjects::pAllocator, bufWrapper.buffer));

        vkGetBufferMemoryRequirements(*CoreObjects::logicalDeviceObj, *bufWrapper.buffer, &bufWrapper.bufMemReq);
        
        ids[i] = bufWrapper.id;
        bufferWrapperList.push_back(bufWrapper);
        idToBufferMap.insert(std::pair<uint32_t, VkBuffer *>({bufWrapper.id, bufWrapper.buffer}));
    }
    return ids;
}

uint32_t * VkBufferFactory::CreateBuffers(const uint32_t & bufferCount, VkBufferCreateInfo * info)
{
    uint32_t * ids = new uint32_t[bufferCount];

    for (uint32_t i = 0; i < bufferCount; i++)
    {
        VkBufferWrapper bufWrapper = {};
        bufWrapper.id = GetId();
        bufWrapper.buffer = new VkBuffer;
        bufWrapper.bufferType = info[i].usage;
        
        ErrorCheck(vkCreateBuffer(*CoreObjects::logicalDeviceObj, &info[i], CoreObjects::pAllocator, bufWrapper.buffer));

        ids[i] = bufWrapper.id;
        bufferWrapperList.push_back(bufWrapper);
        idToBufferMap.insert(std::pair<uint32_t, VkBuffer *>({ bufWrapper.id, bufWrapper.buffer }));

    }

    return ids;
}

void VkBufferFactory::CreateBuffers(const uint32_t & bufferCount, BufferCreateInfo * info, uint32_t * out_buffIds, size_t * out_bufferMemRequirementSize)
{
    VkBufferCreateInfo * vkInfo = VulkanUnwrap::UnwrapBufferCreateInfo(info, bufferCount);

    uint32_t * ids = CreateBuffers(bufferCount, vkInfo);

    for (uint32_t i = 0; i < bufferCount; i++)
    {
        out_buffIds[i] = ids[i];
        VkBuffer * buf = idToBufferMap[ids[i]];
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(*CoreObjects::logicalDeviceObj, *buf, &req);

        out_bufferMemRequirementSize[i] = req.size;
    }

    delete[] ids;
    delete[] vkInfo;
}

uint32_t * VkBufferFactory::AllocateBufferMemory(uint32_t * bufferId, const uint32_t & bufCount)
{
    uint32_t * ids = new uint32_t[bufCount];

    for (uint32_t i = 0; i < bufCount; i++)
    {
        std::vector<VkBufferWrapper>::iterator it;
        it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == bufferId[i]; });
        ASSERT_MSG_DEBUG(it != bufferWrapperList.end(), "buffer id not found");

        it->bufferMemory = new VkDeviceMemory;
        ids[i] = VulkanMemoryManager::GetSingleton()->AllocateMemory(&it->bufMemReq, it->memFlags, it->bufferMemory);
        
        it->memoryOffset = 0;
        it->isBufferSharingMemory = false;
        // binding happens as the size is not explicitly mentioned hence it will be memreq.size and the offset will be 0
        vkBindBufferMemory(*CoreObjects::logicalDeviceObj, *it->buffer, *it->bufferMemory, 0);
    }

    return ids;
}

// TODO : Needs to be tested
uint32_t VkBufferFactory::AllocateSharedBufferMemory(uint32_t * bufferId, const uint32_t & bufCount)
{
    //ASSERT_MSG_DEBUG(0, "Needs testing");

    uint32_t id;

    VkDeviceSize allocationSize = 0;
    VkMemoryRequirements req = {};
    VkMemoryPropertyFlags userReq = {};
    
    std::vector<VkBufferWrapper>::iterator it;
    for (uint32_t i = 0; i < bufCount; i++)
    {
        it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == bufferId[i]; });
        ASSERT_MSG_DEBUG(it != bufferWrapperList.end(), "buffer id not found");

        it->isBufferSharingMemory = true;
        it->memoryOffset = allocationSize;
        allocationSize = allocationSize + it->bufMemReq.size;
        if (i == 0)
        {
            req = it->bufMemReq;
            userReq = it->memFlags;
        }
        else
        {
            ASSERT_MSG_DEBUG(req.alignment == it->bufMemReq.alignment && req.memoryTypeBits == it->bufMemReq.memoryTypeBits && req.size == it->bufMemReq.size,
                "Buffer mem req mismatch");
        }
    }
    
    VkDeviceMemory * memory = new VkDeviceMemory;
    id = VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, userReq, memory, allocationSize);
    
    for (uint32_t i = 0; i < bufCount; i++)
    {
        it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == bufferId[i]; });
        it->bufferMemory = memory;

        vkBindBufferMemory(*CoreObjects::logicalDeviceObj, *it->buffer, *it->bufferMemory, it->memoryOffset);
    }
    return id;
}

void VkBufferFactory::CopyBufferDataToMemory(const uint32_t & bufId, VkDeviceSize dataSize, void * data, VkDeviceSize memoryMapOffset, bool keepMemoryMounted)
{
    std::vector<VkBufferWrapper>::iterator it;
    it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == bufId; });
    ASSERT_MSG_DEBUG(it != bufferWrapperList.end(), "buffer id not found");

    void * gpuMem;
    ErrorCheck(vkMapMemory(*CoreObjects::logicalDeviceObj, *it->bufferMemory, memoryMapOffset,
        dataSize, 0, (void**)&gpuMem));
    memcpy(gpuMem, data, dataSize);
    vkUnmapMemory(*CoreObjects::logicalDeviceObj, *it->bufferMemory);
}

void VkBufferFactory::CopyBufferDataToMemory(const uint32_t & bufId, const VkDeviceSize & dataSize, const VkDeviceSize & memAlignedSize, void * data, const VkDeviceSize & memoryMapOffset, bool keepMemoryMounted)
{
    std::vector<VkBufferWrapper>::iterator it;
    it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == bufId; });
    ASSERT_MSG_DEBUG(it != bufferWrapperList.end(), "buffer id not found");

    void * gpuMem;
    ErrorCheck(vkMapMemory(*CoreObjects::logicalDeviceObj, *it->bufferMemory, memoryMapOffset,
        memAlignedSize, 0, (void**)&gpuMem));
    memcpy(gpuMem, data, dataSize);
    vkUnmapMemory(*CoreObjects::logicalDeviceObj, *it->bufferMemory);
}

void VkBufferFactory::DestroyBuffer(uint32_t id)
{
    std::vector<VkBufferWrapper>::iterator it;
    it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == id; });

    if (it == bufferWrapperList.end())
        return;
    /*if (it->isBufferSharingMemory == false)
    {
        vkFreeMemory(*CoreObjects::logicalDeviceObj, *it->bufferMemory, CoreObjects::pAllocator);
        delete it->bufferMemory;
    }*/
    vkDestroyBuffer(*CoreObjects::logicalDeviceObj, *it->buffer, CoreObjects::pAllocator);

    delete it->buffer;

    bufferWrapperList.erase(it);
}

VkBuffer * VkBufferFactory::GetBuffer(const uint32_t & id)
{
    std::vector<VkBufferWrapper>::iterator it;
    it = std::find_if(bufferWrapperList.begin(), bufferWrapperList.end(), [&](VkBufferWrapper e) { return e.id == id; });

    ASSERT_MSG_DEBUG(it != bufferWrapperList.end(), "Buffer not found");
    
    return it->buffer;
}

size_t VkBufferFactory::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    size_t alignedDataSize;
    VkDeviceSize minUniformAlignment = physicalDeviceProps.limits.minUniformBufferOffsetAlignment;
    if (minUniformAlignment)
        alignedDataSize = (dataSize + minUniformAlignment - 1) &
        ~(minUniformAlignment - 1);
    return alignedDataSize;
}

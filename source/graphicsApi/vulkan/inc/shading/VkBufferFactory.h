#pragma once
#include <vector>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vulkan/vulkan.h>

namespace GfxVk
{
    namespace Shading
    {
        struct VkBufferWrapper
        {
            uint32_t id;
            VkBufferUsageFlags bufferType;
            VkBuffer* buffer;
            VkDeviceMemory* bufferMemory;
            bool isBufferSharingMemory;
            VkDeviceSize memoryOffset;
            VkMemoryRequirements bufMemReq;
            VkMemoryPropertyFlags memFlags;
            void* pGpuMem;
        };

        class VkBufferFactory
        {
        private:
            VkBufferFactory() {}
            VkBufferFactory(VkBufferFactory const&) {}
            VkBufferFactory const& operator= (VkBufferFactory const&) {}

            static VkBufferFactory* instance;

            uint32_t idCounter = 0;
            uint32_t GetId();

            std::vector<VkBufferWrapper> bufferWrapperList;
            std::map<uint32_t, VkBuffer*> idToBufferMap;

            VkPhysicalDeviceProperties physicalDeviceProps;

        public:
            void Init();
            void DeInit();
            void Update();
            static VkBufferFactory* GetInstance();
            ~VkBufferFactory();

            /* uint32_t * CreateBuffer(VkBufferUsageFlags * bufferType, VkMemoryPropertyFlags * memType,
                 void * data, size_t dataSize, void * pGpuMem);
             */

             //deprecated
            uint32_t* CreateBuffers(const uint32_t& bufferCount, VkBufferUsageFlags* bufferType, VkMemoryPropertyFlags* memType, size_t* dataSizes);
            uint32_t* CreateBuffers(const uint32_t& bufferCount, VkBufferCreateInfo* info);
            void CreateBuffers(const uint32_t& bufferCount, Core::Wrappers::BufferCreateInfo* info, uint32_t* out_buffIds, size_t* out_bufferMemRequirements);

            std::pair<uint32_t, std::optional<uint32_t>> CreateBuffer(VkBufferCreateInfo& info, bool allocateMemory);

            uint32_t* AllocateBufferMemory(uint32_t* bufferId, const uint32_t& bufCount);
            uint32_t AllocateSharedBufferMemory(uint32_t* bufferId, const uint32_t& bufCount);
            // deprecated.
            void CopyBufferDataToMemory(const uint32_t& bufId, VkDeviceSize dataSize, void* data, VkDeviceSize memoryOffset, bool keepMemoryMounted = false);
            void CopyBufferDataToMemory(const uint32_t& bufId, const VkDeviceSize& dataSize, const VkDeviceSize& memAlignedSize, void* data, const VkDeviceSize& memoryOffset, bool keepMemoryMounted = false);
            void DestroyBuffer(uint32_t id);

            VkBuffer* GetBuffer(const uint32_t& id);
            size_t GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize);
        };
    }
}
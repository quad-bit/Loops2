#ifndef GFXVULKAN_VKBUFFERFACTORY_H_
#define GFXVULKAN_VKBUFFERFACTORY_H_

#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <map>

namespace GfxVk
{
    namespace Utility
    {
        struct VkBufferWrapper
        {
            uint32_t m_id;
            VkBufferUsageFlags m_bufferUsage;
            VkBuffer m_buffer;
            std::optional<uint32_t> m_bufferMemoryId;
            bool m_isBufferSharingMemory = false;
            VkDeviceSize m_memoryOffset;
            VkMemoryRequirements m_bufMemReq;
            VkMemoryPropertyFlags m_memFlags;
            void* pGpuMem;
        };

        class VkBufferFactory
        {
        private:
            VkBufferFactory() {}
            VkBufferFactory(VkBufferFactory const&) {}
            VkBufferFactory const& operator= (VkBufferFactory const&) {}

            static VkBufferFactory* s_instance;
            static uint32_t s_bufferIdCounter;
            uint32_t GetId();

            std::map<uint32_t, VkBufferWrapper> m_bufferList;
            std::map<uint32_t, VkBufferWrapper>::iterator FindBuffer(uint32_t id);

        public:
            void Init();
            void DeInit();
            static VkBufferFactory* GetInstance();
            ~VkBufferFactory();

            uint32_t CreateBuffer(const VkBufferCreateInfo& info, const std::string& name);

            // memid and memory
            uint32_t AllocateBindBufferMemory(
                const VkBuffer& buffer,
                const VkMemoryPropertyFlags& userReq,
                const VkMemoryRequirements& bufferMemReq,
                std::optional<size_t> allocationSize);

            // allocates and binds, if allocationSize defined that will be used as the memory 
            // size or else the bufferMemReq.Size will be used
            uint32_t AllocateBindBufferMemory(
                uint32_t bufferId,
                const VkMemoryPropertyFlags& userReq,
                bool isMemoryShared,
                std::optional<size_t> allocationSize);

            void BindBufferMemory(
                const VkBuffer& buffer,
                const VkDeviceMemory& memory,
                const VkDeviceSize& offset);

            void BindBufferMemory(
                uint32_t bufferId,
                uint32_t memId,
                const size_t& offset,
                bool isMemoryShared);

            void DestroyBuffer(uint32_t id, bool freeMemory);
            const VkBuffer& GetBuffer(const uint32_t& id);

            size_t GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize);
            Core::Wrappers::MemoryRequirementInfo GetBufferMemoryRequirement(uint32_t id);

            void CopyBufferDataToMemory(const uint32_t& bufId, const VkDeviceSize& dataSize, const VkDeviceSize& memAlignedSize, void* data, const VkDeviceSize& memoryOffset, bool keepMemoryMounted = false);
        };
    }
}
#endif //GFXVULKAN_VKBUFFERFACTORY_H_
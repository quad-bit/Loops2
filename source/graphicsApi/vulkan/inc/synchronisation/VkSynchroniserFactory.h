#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <string>

namespace GfxVk
{
    namespace Sync
    {
        struct FenceWrapper
        {
            uint32_t id;
            VkFence fence = VK_NULL_HANDLE;
        };

        struct SemaphoreWrapper
        {
            uint32_t id;
            VkSemaphore semaphore = VK_NULL_HANDLE;
        };

        struct ImageBarrierWrapper
        {
            std::string m_imageName;
            VkImageMemoryBarrier2 m_barrier;
        };

        struct BufferBarrierWrapper
        {
            std::string m_bufferName;
            VkBufferMemoryBarrier2 m_barrier;
        };

        struct MemoryBarrierWrapper
        {
            std::string m_memoryBarrierName;
            VkMemoryBarrier2 m_barrier;
        };

        struct DependencyWrapper
        {
            uint32_t m_id;
            std::vector<VkImageMemoryBarrier2> m_imageBarriers;
            std::vector<VkBufferMemoryBarrier2> m_bufferBarriers;
            std::vector<VkMemoryBarrier2> m_memoryBarriers;
            VkDependencyInfo m_dependencyInfo;
        };

        class VkSynchroniserFactory
        {
        private:
            VkSynchroniserFactory() {}
            VkSynchroniserFactory(VkSynchroniserFactory const&) {}
            VkSynchroniserFactory const& operator= (VkSynchroniserFactory const&) {}

            static VkSynchroniserFactory* instance;

            std::vector<FenceWrapper> fenceList;
            std::vector<SemaphoreWrapper> semaphoreList;
            std::map<uint32_t, DependencyWrapper> m_dependencyList;

            uint32_t fenceIdCounter = 0, semaphoreIdCounter = 0;
            uint32_t barrierIdCounter = 0;

            uint32_t GetFenceId() { return fenceIdCounter++; }
            uint32_t GetSemaphoreId() { return semaphoreIdCounter++; }
            uint32_t GetBarrierId() { return barrierIdCounter++; }

        public:
            void Init();
            void DeInit();
            void Update();
            static VkSynchroniserFactory* GetInstance();
            ~VkSynchroniserFactory();

            uint32_t CreateFence(bool isFenceSignaled);
            void DestroyFence(uint32_t id);
            const VkFence& GetFence(uint32_t id);

            uint32_t Create_Semaphore(bool isSemaphoreSignaled); // "CreateSemaphore" is already existing, not available
            void DestroySemaphore(uint32_t id);
            const VkSemaphore& GetSemaphore(uint32_t id);
            std::vector<VkSemaphore> GetSemaphore(uint32_t* ids, uint32_t count);

            uint32_t CreateBarrier(
                const std::vector<VkImageMemoryBarrier2>& m_imageBarriers,
                const std::vector<VkBufferMemoryBarrier2>& m_bufferBarriers,
                const std::vector<VkMemoryBarrier2>& m_memoryBarriers
            );

            const VkDependencyInfo& GetDependencyInfo(uint32_t dependencyId) const;
        };
    }
}
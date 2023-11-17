#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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

        class VkSynchroniserFactory
        {

        private:
            VkSynchroniserFactory() {}
            VkSynchroniserFactory(VkSynchroniserFactory const&) {}
            VkSynchroniserFactory const& operator= (VkSynchroniserFactory const&) {}

            static VkSynchroniserFactory* instance;

            std::vector<FenceWrapper> fenceList;
            std::vector<SemaphoreWrapper> semaphoreList;

            uint32_t fenceIdCounter = 0, semaphoreIdCounter = 0;

            uint32_t GetFenceId() { return fenceIdCounter++; }
            uint32_t GetSemaphoreId() { return semaphoreIdCounter++; }

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
        };
    }
}
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace GfxVk
{
    namespace CommandPool
    {
        enum class CMD_BUF_STATES
        {
            INITIAL,
            RECORDING,
            EXECUTABLE,
            PENDING,
            INVALID
        };

        class VkCommandPoolWrapper
        {
        public:
            VkCommandPool* pool;
            uint32_t poolId;
        };

        class VkDrawCommandBuffer;

        class VkCommandBufferFactory
        {
        private:
            VkCommandBufferFactory() {}
            VkCommandBufferFactory(VkCommandBufferFactory const&) {}
            VkCommandBufferFactory const& operator= (VkCommandBufferFactory const&) {}

            static VkCommandBufferFactory* instance;
            static uint32_t poolIdCounter, bufferIdCounter;

            std::vector<VkCommandPoolWrapper> poolList;
            //std::vector<VkCommandBufferWrapper> bufferList;

            std::vector<VkDrawCommandBuffer* > drawCommandBufferList;

            uint32_t GetPoolId();
            uint32_t GetBufferId();

            VkQueue* renderQueue, * computeQueue, * transferQueue;
            uint32_t graphicsQueueFamilyId, computeQueueFamilyId, transferQueueFamilyId;

            VkCommandBuffer* activeCommandBuffer;

        public:
            void Init(uint32_t renderQueueId, uint32_t computeQueueId, uint32_t transferQueueId);
            void DeInit();
            void Update();
            static VkCommandBufferFactory* GetInstance();
            ~VkCommandBufferFactory();

            void CreateCommandPool(VkCommandPoolCreateFlagBits flags, VkQueueFlagBits queueType, uint32_t& id);
            //void CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level, uint32_t count, uint32_t * ids);
            VkDrawCommandBuffer* CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level, Core::Enums::PipelineType commandBufferType, uint32_t* ids);
            void DestroyCommandPool(const uint32_t& id);
            void ResetCommandPool(const uint32_t& id);
            void ResetCommandBuffer(const uint32_t& id, const uint32_t& poolId);
            void BeginCommandBufferRecording(const uint32_t& id, VkCommandBufferUsageFlagBits usage, VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr);
            void BeginCommandBufferRecording(VkCommandBufferUsageFlagBits usage, VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr);
            void EndCommandBufferRecording(const uint32_t& id);
            void EndCommandBufferRecording();
            void DestroyCommandBuffer(const uint32_t& id);
            void ActivateCommandBuffer(const uint32_t& id);

            VkCommandBuffer* GetCommandBuffer(const uint32_t& id);
            VkCommandBuffer* GetCommandBuffer(const uint32_t* ids, const uint32_t& count);
            VkCommandPool* GetCommandPool(const uint32_t& poolId);
        };
    }
}
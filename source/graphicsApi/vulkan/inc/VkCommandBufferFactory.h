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

        struct VkCommandBufferWrapper
        {
            VkCommandBuffer m_cmdBuffer = VK_NULL_HANDLE;
            uint32_t m_id;
        };

        struct VkCommandPoolWrapper
        {
            VkCommandPool m_pool = VK_NULL_HANDLE;
            VkQueueFlagBits m_queueType;
            uint32_t m_poolId;
            // id to commandBuffer
            std::map<uint32_t, VkCommandBufferWrapper> m_cmdBufferList;
        };

        class VkCommandBufferFactory
        {
        private:
            VkCommandBufferFactory() {}
            VkCommandBufferFactory(VkCommandBufferFactory const&) {}
            VkCommandBufferFactory const& operator= (VkCommandBufferFactory const&) {}

            static VkCommandBufferFactory* instance;
            static uint32_t poolIdCounter, bufferIdCounter;

            VkCommandPoolWrapper m_graphicsPool, m_computePool, m_transferPool;

            std::map<uint32_t, VkCommandPoolWrapper> m_additionsPoolList;
            //std::vector<VkCommandBufferWrapper> bufferList;

            //std::vector<VkDrawCommandBuffer* > drawCommandBufferList;

            uint32_t GetPoolId();
            uint32_t GetBufferId();

            VkQueue* renderQueue, *computeQueue, *transferQueue;
            uint32_t graphicsQueueFamilyId, computeQueueFamilyId, transferQueueFamilyId;

            //VkCommandBuffer* activeCommandBuffer;
            VkCommandPoolWrapper& GetPoolWrapper(const VkQueueFlagBits& queueType);

        public:
            void Init(uint32_t renderQueueId, uint32_t computeQueueId, uint32_t transferQueueId);
            void DeInit();
            void Update();
            static VkCommandBufferFactory* GetInstance();
            ~VkCommandBufferFactory();

            uint32_t CreateCommandPool(VkCommandPoolCreateFlagBits flags, VkQueueFlagBits queueType);
            uint32_t CreateCommandBuffer(const VkCommandBufferLevel& level, const VkQueueFlagBits& queueType);
            void DestroyCommandBuffer(uint32_t bufferId, const VkQueueFlagBits& queueType);
            void DestroyCommandBufferCustomPool(uint32_t bufferId, uint32_t poolId);
            void BeginCommandBufferRecording(const uint32_t& id, const VkQueueFlagBits& queueType, VkCommandBufferUsageFlagBits usage, VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr);
            void EndCommandBufferRecording(const uint32_t& id, const VkQueueFlagBits& queueType);

            //void CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level, uint32_t count, uint32_t * ids);
            /*VkDrawCommandBuffer* CreateCommandBuffer(uint32_t poolId, VkCommandBufferLevel level, Core::Enums::PipelineType commandBufferType, uint32_t* ids);
            void DestroyCommandPool(const uint32_t& id);
            void ResetCommandPool(const uint32_t& id);
            void ResetCommandBuffer(const uint32_t& id, const uint32_t& poolId);
            
            void DestroyCommandBuffer(const uint32_t& id);
            void ActivateCommandBuffer(const uint32_t& id);*/

            //VkCommandBuffer* GetCommandBuffer(const uint32_t* ids, const uint32_t& count);
            const VkCommandPool& GetCommandPool(uint32_t poolId);
            const VkCommandBuffer& GetCommandBufferFromPool(uint32_t bufferId, uint32_t poolId);
            const VkCommandBuffer& GetCommandBuffer(uint32_t bufferId,const VkQueueFlagBits& queueType);
        };
    }
}
#ifndef RENDERER_RENDERGRAPHMANAGER_H_
#define RENDERER_RENDERGRAPHMANAGER_H_

#include <RenderData.h>
#include <renderGraph/Pipeline.h>
#include <memory>
#include <vector>
#include "Graph.h"
#include "renderGraph/Task.h"
#include <queue>

namespace Renderer
{
    namespace RenderGraph
    {
        struct CommandBufferWrapper
        {
            uint32_t m_bufId;
            Core::Enums::QueueType m_queueType;
            bool m_isAvailable = true;
        };

        struct PerFrameCommandBufferInfo
        {
            // per frame
            const uint32_t c_maxGraphicsBufferCount = 5;
            const uint32_t c_maxComputeBufferCount = 5;
            const uint32_t c_maxTransferBufferCount = 5;

            std::vector<uint32_t> m_graphicsCmdBufList;
            std::vector<uint32_t> m_computeCmdBufList;
            std::vector<uint32_t> m_transferCmdBufList;

            std::queue<uint32_t> m_graphicsCmdBufAvailabilityList;
            std::queue<uint32_t> m_computeCmdBufAvailabilityList;
            std::queue<uint32_t> m_transferCmdBufAvailabilityList;
        };

        struct PerFrameSemaphoreInfo
        {
            // per frame
            const uint32_t c_maxSemaphoreCount = 7;
            std::vector<uint32_t> m_semaphoreList;
            std::queue<uint32_t> m_semaphoreAvailabilityList;
        };

        class RenderGraphManager
        {
        private:
            RenderGraphManager() = delete;
            const Core::Utility::RenderData& m_renderData;

            std::vector<std::unique_ptr<Renderer::RenderGraph::Pipeline>> m_pipelines;
            Renderer::RenderGraph::Pipeline* m_activePipeline = nullptr;

            uint32_t m_renderQueueId;
            uint32_t m_computeQueueId;
            uint32_t m_transferQueueId;

            std::map<uint32_t, CommandBufferWrapper> m_idToCmdWrapperList;

            void AssignCommandBuffers();

            std::vector<PerFrameCommandBufferInfo> m_perFrameInfo;
            std::vector<PerFrameSemaphoreInfo> m_perFrameSemaphoreInfo;
            std::vector<uint32_t> m_acquireSemaphore, m_presentationSemaphore;
        public:
            explicit RenderGraphManager(const Core::Utility::RenderData& renderData);
            void AddPipeline(std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline);
            void Init(uint32_t renderQueueId, uint32_t computeQueueId, uint32_t transferQueueId);
            void DeInit();
            void Update(const uint32_t frameIndex);
            ~RenderGraphManager();
        };
    }
}

#endif // RENDERER_RENDERGRAPHMANAGER_H_
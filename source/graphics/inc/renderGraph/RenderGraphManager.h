#ifndef RENDERER_RENDERGRAPHMANAGER_H_
#define RENDERER_RENDERGRAPHMANAGER_H_

#include <RenderData.h>
#include <renderGraph/Pipeline.h>
#include <memory>
#include <vector>
#include "Graph.h"
#include "renderGraph/Task.h"
#include <queue>
#include <Settings.h>

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
            uint32_t m_presentationQueueId;
            const Core::WindowSettings& m_windowSettings;

            std::map<uint32_t, CommandBufferWrapper> m_idToCmdWrapperList;

            void AssignCommandBuffers();
            void AssignResourceInfo();

            std::vector<PerFrameCommandBufferInfo> m_perFrameInfo;
            std::vector<PerFrameSemaphoreInfo> m_perFrameSemaphoreInfo;
            std::vector<uint32_t> m_acquireSemaphore, m_presentationSemaphore;
            std::vector<uint32_t> m_acquireSwapchainFence;
            uint32_t m_swapBufferIndex = 0, m_frameInFlightIndex = 0;

            Task* m_pipelineEndTask;
            std::unique_ptr<Task> m_presentationTask;

        public:
            RenderGraphManager(const Core::Utility::RenderData& renderData, const Core::WindowSettings& windowSettings);
            void AddPipeline(std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline);
            void Init(uint32_t renderQueueId,
                uint32_t computeQueueId,
                uint32_t transferQueueId,
                uint32_t presentationQueueId,
                std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline);
            void DeInit();
            void Update(const Core::Wrappers::FrameInfo& frameInfo);
            void SetupFrame(Core::Wrappers::FrameInfo& frameInfo);
            void EndFrame();
            void OnExit();

            ~RenderGraphManager();
        };
    }
}

#endif // RENDERER_RENDERGRAPHMANAGER_H_
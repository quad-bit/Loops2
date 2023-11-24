#ifndef RENDERER_PRESENTATIONTASK_H_
#define RENDERER_PRESENTATIONTASK_H_

#include <renderGraph/Task.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            class PresentationTask : public Task
            {
            private:
                std::vector<uint32_t> m_swapchainImages;
                std::vector<uint32_t> m_swapchainImageViews;

                const uint32_t m_swapbufferCount;

            public:
                PresentationTask(const std::string& name, uint32_t swapbufferCount, uint32_t width, uint32_t height) :
                    Task(name, TaskType::PRESENTATION_TASK), m_swapbufferCount(swapbufferCount)
                {
                }

                ~PresentationTask()
                {
                }

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    // submit for presentation
                    Core::Wrappers::PresentInfo presentInfo = {};
                    presentInfo.m_waitSemaphoreIds.push_back(m_taskSubmitInfo[frameInfo.m_farmeInFlightIndex].m_waitSemaphoreId.value());
                    presentInfo.m_imageIndices.push_back(frameInfo.m_swapBufferIndex);
                    presentInfo.m_queueId = m_taskSubmitInfo[frameInfo.m_farmeInFlightIndex].m_queueId;

                    VulkanInterfaceAlias::PresentSwapchainImage(presentInfo);
                }
            };
        }
    }
}

#endif //RENDERER_PRESENTATIONTASK_H_

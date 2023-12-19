#ifndef RENDERER_RENDERTASK_H_
#define RENDERER_RENDERTASK_H_

#include <renderGraph/Task.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            class RenderTask : public Task
            {
            private:
                std::vector<uint32_t> m_renderingInfoId;
                std::vector<Core::Wrappers::RenderingInfo> m_renderingInfo;
                Core::Wrappers::Rect2D m_renderArea;

            public:
                RenderTask(const std::string& name, const Core::Wrappers::Rect2D& renderArea) :
                    Task(name, TaskType::RENDER_TASK), m_renderArea(renderArea)
                {}

                void AssignRenderingInfo(std::vector<Core::Wrappers::RenderingInfo>& renderingInfo)
                {
                    m_renderingInfo = renderingInfo;
                    for(auto& info : renderingInfo)
                        m_renderingInfoId.push_back(VulkanInterfaceAlias::CreateRenderingInfo(info));
                }

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::RENDER;
                    auto queuePurpose = Core::Enums::PipelineType::GRAPHICS;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);
                    Renderer::CommandReader::BeginRendering(info, m_renderingInfoId[frameInfo.m_swapBufferIndex]);

                    Renderer::CommandReader::EndRendering(info);
                    EndTask(frameInfo, queueType);
                }

                Core::Wrappers::Rect2D GetRenderArea()
                {
                    return m_renderArea;
                }
            };
        }
    }
}

#endif //RENDERER_RENDERTASK_H_

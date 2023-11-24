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
            public:
                RenderTask(const std::string& name) :
                    Task(name, TaskType::RENDER_TASK)
                {}

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::RENDER;
                    auto queuePurpose = Core::Enums::PipelineType::GRAPHICS;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    EndTask(frameInfo);
                }
            };
        }
    }
}

#endif //RENDERER_RENDERTASK_H_

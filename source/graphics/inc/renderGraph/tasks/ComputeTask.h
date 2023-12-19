#ifndef RENDERER_COMPUTETASK_H_
#define RENDERER_COMPUTETASK_H_

#include <renderGraph/Task.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            class ComputeTask : public Task
            {
            public:
                ComputeTask(const std::string& name) :
                    Task(name, TaskType::COMPUTE_TASK)
                {}

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::COMPUTE;
                    auto queuePurpose = Core::Enums::PipelineType::COMPUTE;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    EndTask(frameInfo, queueType);

                }
            };
        }
    }
}

#endif //RENDERER_COMPUTETASK_H_

#ifndef RENDERER_BLITTASK_H_
#define RENDERER_BLITTASK_H_

#include <renderGraph/Task.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            class TransferTask : public Task
            {
            public:
                TransferTask(const std::string& name) :
                    Task(name, TaskType::TRANSFER_TASK)
                {}

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::TRANSFER;
                    auto queuePurpose = Core::Enums::PipelineType::TRANSFER;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    EndTask(frameInfo);
                }
            };
        }
    }
}

#endif //RENDERER_BLITTASK_H_

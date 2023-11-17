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

                void Execute()
                {
                    PLOGD << m_name;
                }
            };
        }
    }
}

#endif //RENDERER_BLITTASK_H_

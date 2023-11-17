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

                void Execute()
                {
                    PLOGD << m_name;
                }
            };
        }
    }
}

#endif //RENDERER_COMPUTETASK_H_

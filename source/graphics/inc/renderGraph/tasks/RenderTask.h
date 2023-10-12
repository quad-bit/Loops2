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

                void Execute()
                {
                    PLOGD << m_name;
                }
            };
        }
    }
}

#endif //RENDERER_RENDERTASK_H_

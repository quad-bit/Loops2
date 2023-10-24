#ifndef RENDERER_NODE_H_
#define RENDERER_NODE_H_

#include <optional>
#include <functional>
#include <string>
#include <CorePrecompiled.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <resourceManagement/Resource.h>
#include <renderGraph/utility/Utils.h>

namespace Renderer
{
    namespace RenderGraph
    {
        enum class TaskType
        {
            RENDER_TASK,
            COMPUTE_TASK,
            BLIT_TASK,
            DOWNLOAD_TASK
        };

        struct TaskInputInfo
        {
            Renderer::RenderGraph::Utils::ResourceMemoryUsage m_usage;
            ResourceAlias* m_resource;
            uint32_t m_resourceParentNodeId;
        };
        /// <summary>
        /// Any cmd can be encapsulated into a task node. The standard types will be RenderTask, ComputeTask, DownloadTask
        /// and BlitTask
        /// </summary>
        class Task
        {
        private:

        protected:
            std::vector<TaskInputInfo> m_inputs;
            std::vector <ResourceAlias*> m_outputs;
            std::string m_name;
            TaskType m_taskType;

        public:
            //virtual const uint32_t& GetId() = 0;
            Task() = delete;
            Task(const std::string& name, const TaskType& taskType) :
                m_name(name), m_taskType(taskType)
            {}

            std::string GetTaskName()
            {
                return m_name;
            }
            void AddInput(ResourceAlias* input,const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage, uint32_t nodeId)
            {
                m_inputs.push_back(TaskInputInfo{usage, input, nodeId});
            }
            void AddOutput(ResourceAlias* output)
            {
                m_outputs.push_back(output);
            }
            const std::vector<TaskInputInfo>& GetInputs()
            {
                return m_inputs;
            }
            const std::vector<ResourceAlias*>& GetOutputs()
            {
                return m_outputs;
            }
        };

        class TaskNode : public Renderer::RenderGraph::Utils::RenderGraphNodeBase
        {
        protected:
            std::unique_ptr<Task> m_task;
        public:
            TaskNode(std::unique_ptr<Task> task, Utils::GraphTraversalCallback& funcs) :
                RenderGraphNodeBase(Renderer::RenderGraph::Utils::RenderGraphNodeType::TASK_NODE, task->GetTaskName(), funcs), m_task(std::move(task))
            {}

            Task* GetTask()
            {
                return m_task.get();
            }

            virtual void Execute() override
            {
                m_graphTraversalCallback.PipelineCompileCallback(this);
            }
        };

        class ResourceNode : public Renderer::RenderGraph::Utils::RenderGraphNodeBase
        {
        protected:
            Renderer::ResourceManagement::ResourceType m_resourceType;
            std::map<uint32_t, Renderer::RenderGraph::Utils::ResourceMemoryUsage> sourceTaskIdToUsageMap, destTaskIdToUsageMap;
            ResourceAlias* m_resource;
        public:
            ResourceNode(ResourceAlias* resource, const std::string& nodeName, const Renderer::ResourceManagement::ResourceType& resourceType, Utils::GraphTraversalCallback& funcs) :
                RenderGraphNodeBase(Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE, nodeName, funcs), m_resourceType(resourceType), m_resource(resource)
            {}

            void RegisterAsTaskInput(const uint32_t taskId, const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
            {
                if(sourceTaskIdToUsageMap.find(taskId) != sourceTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                sourceTaskIdToUsageMap.insert({ taskId, usage });
            }

            void RegisterAsTaskOutput(const uint32_t taskId, const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
            {
                if (destTaskIdToUsageMap.find(taskId) != destTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                destTaskIdToUsageMap.insert({ taskId, usage });
            }

            ResourceAlias* GetResource()
            {
                return m_resource;
            }

            virtual void Execute() override
            {
                m_graphTraversalCallback.PipelineCompileCallback(this);
            }
        };

    }
}

#endif //RENDERER_NODE_H_
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
            TRANSFER_TASK,
            DOWNLOAD_TASK
        };

        struct TaskCommandBufferInfo
        {
            uint32_t m_bufId;
            std::optional<bool> m_shouldStart, m_shouldStop;
        };

        struct TaskSubmitInfo
        {
            std::optional<uint32_t> m_waitSemaphoreId, m_signalSemaphoreId;
            std::optional<uint32_t> m_fenceId;
        };

        /// <summary>
        /// Any cmd can be encapsulated into a task node. The standard types will be RenderTask, ComputeTask, DownloadTask
        /// and BlitTask
        /// </summary>
        class Task
        {
        private:

        protected:
            std::vector<Renderer::RenderGraph::Utils::ConnectionInfo> m_inputs;
            std::vector <Renderer::RenderGraph::Utils::ConnectionInfo> m_outputs;
            std::string m_name;
            TaskType m_taskType;

            std::vector<TaskCommandBufferInfo> m_cmdBufferInfo;
            std::vector<TaskSubmitInfo> m_taskSubmitInfo;

        public:
            Task() = delete;
            Task(const std::string& name, const TaskType& taskType) :
                m_name(name), m_taskType(taskType)
            {}

            virtual ~Task()
            {}

            std::string GetTaskName()
            {
                return m_name;
            }

            const TaskType& GetTaskType()
            {
                return m_taskType;
            }

            void AddInput(const Renderer::RenderGraph::Utils::ConnectionInfo& info)
            {
                m_inputs.push_back(info);
            }

            void AddOutput(const Renderer::RenderGraph::Utils::ConnectionInfo& info)
            {
                m_outputs.push_back(info);
            }

            void AssignSubmitInfo(const std::vector<TaskSubmitInfo>& submitInfo)
            {
                for (auto& item : submitInfo)
                    m_taskSubmitInfo.push_back(item);
            }

            void AssignCommandBufferInfo(const std::vector<TaskCommandBufferInfo>& info)
            {
                for (auto& item : info)
                {
                    m_cmdBufferInfo.push_back(item);
                }
            }

            void CloseTaskCommandBuffer()
            {
                for (auto& item : m_cmdBufferInfo)
                {
                    item.m_shouldStop = true;
                }
            }

            void OpenTaskCommandBuffer()
            {
                for (auto& item : m_cmdBufferInfo)
                {
                    item.m_shouldStart = true;
                }
            }

            void PrintTaskInfo()
            {
                std::cout << "task id : " << m_name << "\n";
                for (auto& item : m_cmdBufferInfo)
                {
                    std::cout << "command buffer id : " << item.m_bufId << "\n";
                    if (item.m_shouldStart.has_value())
                    {
                        if (item.m_shouldStart.value())
                            std::cout << "command should start : " << "true" << "\n";
                        else
                            std::cout << "command should start : " << "false" << "\n";
                    }

                    if (item.m_shouldStop.has_value())
                    {
                        if (item.m_shouldStop.value())
                            std::cout << "command should stop : " << "true" << "\n";
                        else
                            std::cout << "command should stop : " << "false" << "\n";
                    }
                }

                for (auto& item : m_taskSubmitInfo)
                {
                    if (item.m_signalSemaphoreId.has_value())
                    {
                        std::cout << "signal " << item.m_signalSemaphoreId.value() << "\n";
                    }

                    if (item.m_waitSemaphoreId.has_value())
                    {
                        std::cout << "wait " << item.m_waitSemaphoreId.value() << "\n";
                    }
                }
            }

            const std::vector<Renderer::RenderGraph::Utils::ConnectionInfo>& GetInputs()
            {
                return m_inputs;
            }
            const std::vector<Renderer::RenderGraph::Utils::ConnectionInfo>& GetOutputs()
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
#ifndef RENDERER_NODE_H_
#define RENDERER_NODE_H_

#include <optional>
#include <functional>
#include <string>
#include <CorePrecompiled.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <resourceManagement/Resource.h>
#include <renderGraph/utility/Utils.h>
#include <VulkanInterface.h>
#include <CommandReader.h>

namespace Renderer
{
    namespace RenderGraph
    {
        enum class TaskType
        {
            RENDER_TASK,
            COMPUTE_TASK,
            TRANSFER_TASK,
            PRESENTATION_TASK,
            DOWNLOAD_TASK
        };

        struct TaskCommandBufferInfo
        {
            uint32_t m_bufId;
            std::optional<bool> m_shouldStart, m_shouldStop;
            std::optional<uint32_t> m_previousQueueId;
        };

        struct TaskQueueInfo
        {
            std::optional<uint32_t> m_previousTaskQueueId, m_nextTaskQueueId;
            uint32_t m_taskQueueId;
        };

        struct TaskSubmitInfo
        {
            std::optional<uint32_t> m_waitSemaphoreId, m_signalSemaphoreId;
            std::optional<uint32_t> m_fenceId;
            Core::Enums::PipelineType m_pipelineType;
            Core::Enums::QueueType m_queuePurpose;
        };

        struct TaskImageResourceInfo
        {
            // Per frame images
            std::vector<ResourceAlias*> m_imageResource;
            std::optional<Core::Enums::ImageLayout> m_previousLayout;
            Core::Enums::ImageLayout m_expectedLayout;
        };

        struct PerFrameTaskBarrierInfo
        {
            uint32_t m_barrierHandle;
            //Core::Wrappers::BarrierDependencyInfo m_dependencyInfo;
            std::vector<Core::Wrappers::ImageBarrier2> m_imageBarriers;
            std::vector<Core::Wrappers::BufferBarrier2> m_bufferBarriers;
            std::vector<Core::Wrappers::MemoryBarrier2> m_memoryBarriers;
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
            std::vector<Renderer::RenderGraph::Utils::ConnectionInfo> m_outputs;

            std::string m_name;
            TaskType m_taskType;

            std::vector<TaskCommandBufferInfo> m_cmdBufferInfo;
            std::vector<TaskSubmitInfo> m_taskSubmitInfo;
            TaskQueueInfo m_taskQueueInfo;

            uint32_t m_activeCommandBuffer;

            // one for each swap buffer ( image count = swapbufferCount ),
            // one PerFrameTaskBarrierInfo for all the resources in one frame
            std::vector<std::optional<PerFrameTaskBarrierInfo>> m_taskBarrierInfo;
            std::optional< std::vector<PerFrameTaskBarrierInfo>> m_presentationBarrierInfo;

        protected:
            void StartTask(const Core::Wrappers::FrameInfo& frameInfo, const Core::Enums::QueueType queueType)
            {
                m_activeCommandBuffer = m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_bufId;
                if (m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_shouldStart)
                {
                    Core::Enums::CommandBufferUsage usage{ Core::Enums::CommandBufferUsage::USAGE_ONE_TIME_SUBMIT_BIT };
                    VulkanInterfaceAlias::BeginCommandBufferRecording(m_activeCommandBuffer, queueType, usage, std::nullopt);
                }

                {
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);
                    Renderer::CommandReader::CommandBeginLabel(info, GetTaskName());
                }

                if(m_taskBarrierInfo.size() > 0 &&  m_taskBarrierInfo[frameInfo.m_swapBufferIndex].has_value())
                {
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);
                    Renderer::CommandReader::SetPipelineBarrier(info, m_taskBarrierInfo[frameInfo.m_swapBufferIndex].value().m_barrierHandle);
                }
            }

            void EndTask(const Core::Wrappers::FrameInfo& frameInfo, const Core::Enums::QueueType queueType)
            {
                if (m_presentationBarrierInfo.has_value())
                {
                    Core::Wrappers::CommandBufferInfo info(m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_bufId, queueType);
                    Renderer::CommandReader::SetPipelineBarrier(info, m_presentationBarrierInfo.value()[frameInfo.m_swapBufferIndex].m_barrierHandle);
                }

                {
                    Core::Wrappers::CommandBufferInfo info(m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_bufId, queueType);
                    Renderer::CommandReader::CommandEndLabel(info);
                }

                if (m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_shouldStop)
                {
                    VulkanInterfaceAlias::EndCommandBufferRecording(m_cmdBufferInfo[frameInfo.m_swapBufferIndex].m_bufId, m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_queuePurpose);

                    Core::Wrappers::SubmitInfo info = {};
                    info.commandBufferCount = 1;
                    info.commandBufferIds = &m_activeCommandBuffer;
                    info.pipelineStage = Core::Enums::PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT;
                    info.purpose = &m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_queuePurpose;
                    info.queueType = &m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_pipelineType;
                    info.queueId = m_taskQueueInfo.m_taskQueueId;
                    info.signalSemaphoreCount = 1;
                    info.signalSemaphoreIds = &m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_signalSemaphoreId.value();
                    info.waitSemaphoreCount = 1;
                    info.waitSemaphoreIds = &m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_waitSemaphoreId.value();

                    if (m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_fenceId.has_value())
                        VulkanInterfaceAlias::SubmitJob(&info, 1, m_taskSubmitInfo[frameInfo.m_frameInFlightIndex].m_fenceId.value());
                    else
                        VulkanInterfaceAlias::SubmitJob(&info, 1, std::nullopt);
                }
            }

        public:
            Task() = delete;
            Task(const std::string& name, const TaskType& taskType) :
                m_name(name), m_taskType(taskType)
            {}

            virtual ~Task()
            {}

            virtual void Execute(const Core::Wrappers::FrameInfo& frameInfo) = 0;


            std::string GetTaskName()
            {
                return m_name;
            }

            const TaskType& GetTaskType()
            {
                return m_taskType;
            }

            const std::vector<Renderer::RenderGraph::TaskCommandBufferInfo>& GetCommandBufferInfo()
            {
                return m_cmdBufferInfo;
            }

            void AddInput(const Renderer::RenderGraph::Utils::ConnectionInfo& info)
            {
                m_inputs.push_back(info);
            }

            void AddOutput(const Renderer::RenderGraph::Utils::ConnectionInfo& info)
            {
                m_outputs.push_back(info);
            }

            void AssignSubmitInfo(const std::vector<TaskSubmitInfo>& submitInfo,
                std::optional<uint32_t> nextTaskQueueId)
            {
                for (auto& item : submitInfo)
                    m_taskSubmitInfo.push_back(item);

                m_taskQueueInfo.m_nextTaskQueueId = nextTaskQueueId;
            }

            void AssignCommandBufferInfo(
                const std::vector<TaskCommandBufferInfo>& info,
                uint32_t taskQueueId, std::optional<uint32_t> previousTaskQueueId)
            {
                for (auto& item : info)
                {
                    m_cmdBufferInfo.push_back(item);
                }
                m_taskQueueInfo.m_taskQueueId = taskQueueId;
                m_taskQueueInfo.m_previousTaskQueueId = previousTaskQueueId;
            }

            void AssignBarrierInfo(const std::vector<PerFrameTaskBarrierInfo>& barrierInfo, bool isPresentationBarrier = false)
            {
                if (barrierInfo.size() > 0)
                {
                    if (barrierInfo[0].m_bufferBarriers.size() > 0 ||
                        barrierInfo[0].m_imageBarriers.size() > 0 ||
                        barrierInfo[0].m_memoryBarriers.size() > 0)
                    {
                        if(isPresentationBarrier)
                            m_presentationBarrierInfo = std::vector<Renderer::RenderGraph::PerFrameTaskBarrierInfo>{};
                        for (auto& barrier : barrierInfo)
                        {
                            if (isPresentationBarrier)
                            {
                                m_presentationBarrierInfo.value().push_back(barrier);
                                m_presentationBarrierInfo.value()[m_presentationBarrierInfo.value().size() - 1].m_barrierHandle = VulkanInterfaceAlias::CreateBarrier(
                                    barrier.m_imageBarriers,
                                    barrier.m_bufferBarriers,
                                    barrier.m_memoryBarriers);
                            }
                            else
                            {
                                m_taskBarrierInfo.push_back(barrier);
                                m_taskBarrierInfo[m_taskBarrierInfo.size() - 1].value().m_barrierHandle = VulkanInterfaceAlias::CreateBarrier(
                                    barrier.m_imageBarriers,
                                    barrier.m_bufferBarriers,
                                    barrier.m_memoryBarriers);
                            }
                        }
                    }
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

            const std::vector<TaskSubmitInfo>& GetTaskSubmitInfo()
            {
                return m_taskSubmitInfo;
            }

            void PrintTaskInfo()
            {
                std::cout << "\n\ntask id : " << m_name << "\n";
                
                bool printCommandBufferInfo = false;
                bool printQueueInfo = false;
                bool printSubmitInfo = true;
                bool printBarrierInfo = false;

                if (printCommandBufferInfo)
                {
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
                }

                if (printQueueInfo)
                {
                    if (m_taskQueueInfo.m_previousTaskQueueId.has_value())
                        std::cout << "previous task queue : " << m_taskQueueInfo.m_previousTaskQueueId.value() << "\n";
                    std::cout << "task queue : " << m_taskQueueInfo.m_taskQueueId << "\n";
                    if (m_taskQueueInfo.m_nextTaskQueueId.has_value())
                        std::cout << "next task queue : " << m_taskQueueInfo.m_nextTaskQueueId.value() << "\n";
                }

                if (printSubmitInfo)
                {
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

                        if (item.m_fenceId.has_value())
                        {
                            std::cout << "fence " << item.m_fenceId.value() << "\n";
                        }
                    }
                }

                if (printBarrierInfo)
                {
                    for (auto& info : m_taskBarrierInfo)
                    {
                        if (!info.has_value())
                            break;
                        for (auto& imageBarrierInfo : info.value().m_imageBarriers)
                        {
                            std::cout << "\n  Image : " << imageBarrierInfo.m_imageName;
                            std::cout << "\n    SrcStage : " << Core::Utility::ConvertPipelineStageFlagToString(imageBarrierInfo.m_srcStageMask[0]);
                            std::cout << "    SrcAccess : " << Core::Utility::ConvertPipelineAccessFlagToString(imageBarrierInfo.m_srcAccessMask[0]);
                            std::cout << "\n    DstStage : " << Core::Utility::ConvertPipelineStageFlagToString(imageBarrierInfo.m_dstStageMask[0]);
                            std::cout << "    DstAccess : " << Core::Utility::ConvertPipelineAccessFlagToString(imageBarrierInfo.m_dstAccessMask[0]);
                            std::cout << "\n    previous layout : " << Core::Utility::ConvertImageLayoutToString(imageBarrierInfo.m_oldLayout);
                            std::cout << "    expected layout : " << Core::Utility::ConvertImageLayoutToString(imageBarrierInfo.m_newLayout);
                        }
                    }
                }
                std::cout << "\n=============\n";
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
            std::map<uint32_t, Renderer::RenderGraph::Utils::ResourceMemoryUsage> m_sourceTaskIdToUsageMap, m_destTaskIdToUsageMap;
            std::vector<ResourceAlias*> m_resourceList;

        public:
            ResourceNode(const std::vector<ResourceAlias*>& resource, const std::string& nodeName, const Renderer::ResourceManagement::ResourceType& resourceType, Utils::GraphTraversalCallback& funcs) :
                RenderGraphNodeBase(Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE, nodeName, funcs), m_resourceType(resourceType), m_resourceList(resource)
            {}

            void RegisterAsTaskInput(const uint32_t taskId, const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
            {
                if(m_sourceTaskIdToUsageMap.find(taskId) != m_sourceTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                m_sourceTaskIdToUsageMap.insert({ taskId, usage });
            }

            void RegisterAsTaskOutput(const uint32_t taskId, const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
            {
                if (m_destTaskIdToUsageMap.find(taskId) != m_destTaskIdToUsageMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "duplicate edge.");
                }
                m_destTaskIdToUsageMap.insert({ taskId, usage });
            }

            const std::vector<ResourceAlias*>& GetResource()
            {
                return m_resourceList;
            }

            virtual void Execute() override
            {
                m_graphTraversalCallback.PipelineCompileCallback(this);
            }
        };

    }
}

#endif //RENDERER_NODE_H_
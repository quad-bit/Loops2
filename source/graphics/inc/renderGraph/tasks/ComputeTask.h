#ifndef RENDERER_COMPUTETASK_H_
#define RENDERER_COMPUTETASK_H_

#include <renderGraph/Task.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            struct DispatchInfo
            {
                Core::Wrappers::DescriptorSetBindingInfo m_descriptorInfo;
            };

            class ComputeTask : public Task
            {
            private:
                uint32_t m_workGroupX = 1, m_workGroupY = 1, m_workGroupZ = 1;

                // to avoid default 0 value
                std::optional<uint32_t> m_shaderStateId;
                std::optional<uint32_t> m_computePipelineId;
                std::optional<uint32_t> m_pipelineLayoutId;

                std::vector<DispatchInfo> m_dispatchCache;

            public:
                ComputeTask(const std::string& name, const std::string& effectName, const std::string& techniqueName,
                    uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) :
                    Task(name, TaskType::COMPUTE_TASK),
                    m_workGroupX(workGroupX), m_workGroupY(workGroupY), m_workGroupZ(workGroupZ)
                {
                    auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
                    auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, techniqueName);
                    m_taskId = VulkanInterfaceAlias::GetTaskId(effectId, techId, m_name);

                    // Shader module info
                    m_shaderStateId = VulkanInterfaceAlias::GetShaderStateId(m_taskId);

                    // pipelineLayout
                    m_pipelineLayoutId = VulkanInterfaceAlias::GetPipelineLayoutId(m_taskId);
                }

                void CreateComputePipeline()
                {
                    Core::Wrappers::ComputePipelineCreateInfo pipelineInfo{};
                    pipelineInfo.pipelineLayoutId = m_pipelineLayoutId.value();
                    pipelineInfo.shaderStateId = m_shaderStateId.value();

                    m_computePipelineId = VulkanInterfaceAlias::CreatePipeline(pipelineInfo);
                }

                void UpdateDispatchInfo(const DispatchInfo& dispatchInfo)
                {
                    m_dispatchCache.push_back(dispatchInfo);
                    ASSERT_MSG_DEBUG(m_dispatchCache.size() < m_maxFrameCacheCount + 1, "Cache count exceeded");

                    if (m_cachingEnabled)
                    {
                        UpdateCacheInfo();
                    }
                }

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::COMPUTE;
                    auto queuePurpose = Core::Enums::PipelineType::COMPUTE;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);

                    if (m_dispatchCache.size() > 0)
                    {
                        uint32_t index = 0;
                        if (m_cachingEnabled)
                        {
                            index = frameInfo.m_frameInFlightIndex;
                        }

                        auto& dispatchInfo = m_dispatchCache[index];

                        // bind pipeline
                        CommandReader::BindPipeline(info, Core::Enums::PipelineType::COMPUTE, m_computePipelineId.value());

                        // bind descriptor set
                        CommandReader::BindDescriptorSet(info, dispatchInfo.m_descriptorInfo, m_pipelineLayoutId.value());

                        // dispatch
                        CommandReader::Dispatch(info, m_workGroupX, m_workGroupY, m_workGroupZ);
                    }

                    EndTask(frameInfo, queueType);

                    if (!m_cachingEnabled)
                    {
                        m_dispatchCache.clear();
                    }
                }
            };
        }
    }
}

#endif //RENDERER_COMPUTETASK_H_

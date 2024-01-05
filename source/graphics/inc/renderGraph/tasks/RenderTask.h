#ifndef RENDERER_RENDERTASK_H_
#define RENDERER_RENDERTASK_H_

#include <renderGraph/Task.h>
#include <utility>

#define PIPELINE_STATE_DEFAULT_INDEX 0

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Tasks
        {
            class RenderTask : public Task
            {
            private:
                std::vector<uint32_t> m_renderingInfoId;
                std::vector<Core::Wrappers::RenderingInfo> m_renderingInfo;
                Core::Wrappers::Rect2D m_renderArea;

                std::optional<uint32_t> m_graphicsPipelineId;

            public:
                std::optional<uint32_t> m_vertexInputStateId;
                std::optional<uint32_t> m_shaderStateId;
                std::optional<uint32_t> m_inputAssemblyStateId;
                std::optional<uint32_t> m_tesselationStateId;
                std::optional<uint32_t> m_rasterizationStateId;
                std::optional<uint32_t> m_multiSampleStateId;
                std::optional<uint32_t> m_depthStencilStateId;
                std::optional<uint32_t> m_blendingStateId;
                std::optional<uint32_t> m_dynamicStateId;
                std::optional<uint32_t> m_viewportStateId;

                std::optional<uint32_t> m_pipelineLayoutId;

                RenderTask(const std::string& name, const Core::Wrappers::Rect2D& renderArea,
                    const std::string& effectName, const std::string& techniqueName) :
                    Task(name, TaskType::RENDER_TASK), m_renderArea(renderArea)
                {
                    // Vertex input Create info
                    m_vertexInputStateId = VulkanInterfaceAlias::GetVertexInputStateId(effectName,
                        techniqueName, m_name);

                    // Shader module info
                    m_shaderStateId = VulkanInterfaceAlias::GetShaderStateId(effectName,
                        techniqueName, m_name);

                    // pipelineLayout
                    m_pipelineLayoutId = VulkanInterfaceAlias::GetPipelineLayoutId(effectName,
                        techniqueName, m_name);

                    m_inputAssemblyStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_tesselationStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_rasterizationStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_multiSampleStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_depthStencilStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_blendingStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_dynamicStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_viewportStateId = PIPELINE_STATE_DEFAULT_INDEX;
                }

                void AssignRenderingInfo(std::vector<Core::Wrappers::RenderingInfo>& renderingInfo)
                {
                    m_renderingInfo = renderingInfo;
                    for(auto& info : renderingInfo)
                        m_renderingInfoId.push_back(VulkanInterfaceAlias::CreateRenderingInfo(info));
                }

                void CreateGraphicsPipeline()
                {
                    std::vector<Core::Enums::Format> colorFormats;
                    auto& info = m_renderingInfo[0];
                    {
                        for (auto& colorInfo : info.m_colorAttachmentInfo)
                        {
                            colorFormats.push_back(colorInfo.m_imageFormat);
                        }
                    }

                    Core::Wrappers::PipelineCreateInfo pipelineInfo{};
                    pipelineInfo.colorFormats = colorFormats;
                    if(info.m_depthAttachmentInfo.has_value())
                        pipelineInfo.depthFormat = info.m_depthAttachmentInfo.value().m_imageFormat;
                    
                    pipelineInfo.pipelineLayoutId = m_pipelineLayoutId.value();
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::ColorBlendState, m_blendingStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::DepthStencilState, m_depthStencilStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::DynamicState, m_dynamicStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::InputAssemblyState, m_inputAssemblyStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::MultisampleState, m_multiSampleStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::RasterizationState, m_rasterizationStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::TessellationState, m_tesselationStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::ViewportState, m_viewportStateId.value() });
                    pipelineInfo.statesToIdMap.insert({ Core::Enums::PipelineStates::ShaderStage, m_shaderStateId.value() });
                    pipelineInfo.statesToIdMap.insert({Core::Enums::PipelineStates::VertexInputState, m_vertexInputStateId.value()});

                    m_graphicsPipelineId = VulkanInterfaceAlias::CreatePipeline(pipelineInfo);
                }

                void DestroyGraphicsPipeline()
                {
                    //VulkanInterfaceAlias::pipe
                }

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::RENDER;
                    auto queuePurpose = Core::Enums::PipelineType::GRAPHICS;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);
                    Renderer::CommandReader::BeginRendering(info, m_renderingInfoId[frameInfo.m_swapBufferIndex]);

                    Renderer::CommandReader::EndRendering(info);
                    EndTask(frameInfo, queueType);
                }

                Core::Wrappers::Rect2D GetRenderArea()
                {
                    return m_renderArea;
                }
            };
        }
    }
}

#endif //RENDERER_RENDERTASK_H_

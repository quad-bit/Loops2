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
            struct MeshDrawInfo
            {
                Core::Wrappers::DescriptorSetBindingInfo m_descriptorInfo;
                // incase of non contigous setup in vertex buffers, multiple calls to bind
                // vertex buffer are required
                std::vector<Core::Wrappers::VertexBufferBindingInfo> m_vertexBufferInfo;
                std::optional<Core::Wrappers::IndexBufferBindingInfo> m_indexBufferInfo;
                uint32_t m_vertexCount, m_indexCount;
            };

            struct DrawInfo
            {
                std::vector<MeshDrawInfo> m_meshInfoList{};
            };

            class RenderTask : public Task
            {
            private:
                std::vector<uint32_t> m_renderingInfoId;
                std::vector<Core::Wrappers::RenderingInfo> m_renderingInfo;
                Core::Wrappers::Rect2D m_renderArea;

                // to avoid default 0 value
                std::optional<uint32_t> m_graphicsPipelineId;

                // in case there are no object for a specific technique/material, we can remove the drawing setup
                //std::optional<DrawInfo> m_drawInfo;
                std::vector<DrawInfo> m_drawCache;

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
                    //m_drawInfo = {};

                    auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
                    auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, techniqueName);
                    m_taskId = VulkanInterfaceAlias::GetTaskId(effectId, techId, m_name);

                    // Vertex input Create info
                    m_vertexInputStateId = VulkanInterfaceAlias::GetVertexInputStateId(m_taskId);

                    // Shader module info
                    m_shaderStateId = VulkanInterfaceAlias::GetShaderStateId(m_taskId);

                    // pipelineLayout
                    m_pipelineLayoutId = VulkanInterfaceAlias::GetPipelineLayoutId(m_taskId);

                    m_inputAssemblyStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_tesselationStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_rasterizationStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_multiSampleStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_depthStencilStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_blendingStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_dynamicStateId = PIPELINE_STATE_DEFAULT_INDEX;
                    m_viewportStateId = PIPELINE_STATE_DEFAULT_INDEX;
                }

                void SetDepthStencilStateId(uint32_t id)
                {
                    m_depthStencilStateId = id;
                }

                void SetRasterisationStateId(uint32_t id)
                {
                    m_rasterizationStateId = id;
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

                void Execute(const Core::Wrappers::FrameInfo& frameInfo) override
                {
                    auto queueType = Core::Enums::QueueType::RENDER;
                    auto queuePurpose = Core::Enums::PipelineType::GRAPHICS;

                    StartTask(frameInfo, queueType);
                    //PLOGD << m_name;
                    Core::Wrappers::CommandBufferInfo info(m_activeCommandBuffer, queueType);
                    Renderer::CommandReader::BeginRendering(info, m_renderingInfoId[frameInfo.m_swapBufferIndex]);

                    if (m_drawCache.size() > 0)
                    {
                        uint32_t index = 0;
                        if (m_cachingEnabled)
                        {
                            index = frameInfo.m_frameInFlightIndex;
                        }

                        auto& drawInfo = m_drawCache[index];

                        CommandReader::SetViewport(info, m_renderArea.lengthX, m_renderArea.lengthY,
                            m_renderArea.offsetX, m_renderArea.offsetY, 0.0f, 1.0f);

                        CommandReader::SetScissor(info, m_renderArea.lengthX, m_renderArea.lengthY,
                            m_renderArea.offsetX, m_renderArea.offsetY);

                        // bind pipeline
                        CommandReader::BindPipeline(info, Core::Enums::PipelineType::GRAPHICS, m_graphicsPipelineId.value());
                        for (auto& meshInfo : drawInfo.m_meshInfoList)
                        {
                            // bind vertex buffer
                            for (auto& vertInfo : meshInfo.m_vertexBufferInfo)
                            {
                                CommandReader::BindVertexBuffers(info, vertInfo);
                            }

                            // bind index buffer
                            if (meshInfo.m_indexBufferInfo.has_value())
                            {
                                CommandReader::BindIndexBuffers(info, meshInfo.m_indexBufferInfo.value());
                            }

                            // bind descriptor set
                            CommandReader::BindDescriptorSet(info, meshInfo.m_descriptorInfo, m_pipelineLayoutId.value());

                            // draw
                            if (meshInfo.m_indexBufferInfo.has_value())
                            {
                                Core::Wrappers::IndexedDrawInfo indexDrawInfo{};
                                indexDrawInfo.firstIndex = 0;
                                indexDrawInfo.firstInstance = 0;
                                indexDrawInfo.indexCount = meshInfo.m_indexCount;
                                indexDrawInfo.instanceCount = 1;
                                indexDrawInfo.vertexOffset = 0;

                                CommandReader::DrawIndex(info, indexDrawInfo);
                            }
                            else
                            {
                                Core::Wrappers::DrawArrayInfo drawInfo{};
                                drawInfo.firstInstance = 0;
                                drawInfo.firstVertex = 0;
                                drawInfo.instanceCount = 1;
                                drawInfo.vertexCount = meshInfo.m_vertexCount;

                                CommandReader::Draw(info, drawInfo);
                            }
                        }
                    }

                    Renderer::CommandReader::EndRendering(info);
                    EndTask(frameInfo, queueType);

                    if (!m_cachingEnabled)
                    {
                        m_drawCache.clear();
                    }
                }

                Core::Wrappers::Rect2D GetRenderArea()
                {
                    return m_renderArea;
                }

                void UpdateDrawInfo(const DrawInfo& drawInfo)
                {
                    m_drawCache.push_back(drawInfo);
                    ASSERT_MSG_DEBUG(m_drawCache.size() < m_maxFrameCacheCount + 1, "Cache count exceeded");
                    //m_drawInfo = drawInfo;

                    if (m_cachingEnabled)
                    {
                        UpdateCacheInfo();
                    }
                }
            };
        }
    }
}

#endif //RENDERER_RENDERTASK_H_

#include "CommandReader.h"
#include <vulkan/vulkan.h>
#include <VkCommandWriter.h>


void Renderer::CommandReader::SetViewport(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY, float minDepth, float maxDepth)
{
    GfxVk::CommandWriter::SetViewport(cmdBufInfo, width, height, positionX, positionY, minDepth, maxDepth);
}

void Renderer::CommandReader::SetScissor(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY)
{
    GfxVk::CommandWriter::SetScissor(cmdBufInfo, width, height, positionX, positionY);
}

void Renderer::CommandReader::SetDepthBias(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float depthBiasConstant, float depthBiasClamp, float depthBiasSlope)
{
    GfxVk::CommandWriter::SetDepthBias(cmdBufInfo, depthBiasConstant, depthBiasClamp, depthBiasSlope);
}

void Renderer::CommandReader::BeginRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::RenderPassBeginInfo& renderPassBeginInfo, const Core::Enums::SubpassContentStatus& subpassContentStatus)
{
    GfxVk::CommandWriter::BeginRenderPass(cmdBufInfo, renderPassBeginInfo, subpassContentStatus);
}

void Renderer::CommandReader::EndRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
{
    GfxVk::CommandWriter::EndRenderPass(cmdBufInfo);
}

void Renderer::CommandReader::BindPipeline(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Enums::PipelineType& type, uint32_t pipelineId)
{
    GfxVk::CommandWriter::BindPipeline(cmdBufInfo, type, pipelineId);
}

void Renderer::CommandReader::BindDescriptorSet(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DescriptorSetBindingInfo& info)
{
    GfxVk::CommandWriter::BindDescriptorSet(cmdBufInfo, info);
}

void Renderer::CommandReader::BindVertexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::VertexBufferBindingInfo& info)
{
    GfxVk::CommandWriter::BindVertexBuffers(cmdBufInfo, info);
}

void Renderer::CommandReader::BindIndexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexBufferBindingInfo& info)
{
    GfxVk::CommandWriter::BindIndexBuffers(cmdBufInfo, info);
}

void Renderer::CommandReader::DrawIndex(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexedDrawInfo& info)
{
    GfxVk::CommandWriter::DrawIndex(cmdBufInfo, info);
}

void Renderer::CommandReader::Draw(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DrawArrayInfo& info)
{
    GfxVk::CommandWriter::Draw(cmdBufInfo, info);
}

void Renderer::CommandReader::SetPipelineBarrier(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, uint32_t barrierId)
{
    GfxVk::CommandWriter::SetPipelineBarrier(cmdBufInfo, barrierId);
}

void Renderer::CommandReader::BeginRendering(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, uint32_t renderingInfoId)
{
    GfxVk::CommandWriter::BeginRendering(cmdBufInfo, renderingInfoId);
}

void Renderer::CommandReader::EndRendering(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
{
    GfxVk::CommandWriter::EndRendering(cmdBufInfo);
}

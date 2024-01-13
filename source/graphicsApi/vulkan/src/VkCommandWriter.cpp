#include "VkCommandWriter.h"
#include <vulkan/vulkan.h>
#include "VkCommandBufferFactory.h"
#include <Platform/Assertion.h>
#include "synchronisation/VkSynchroniserFactory.h"
#include "VkRenderPassFactory.h"
#include <utility/VkDebugMarkerUtil.h>
#include "shading/VkShaderResourceManager.h"
#include "pipeline/VulkanGraphicsPipelineFactory.h"
#include "shading/VkBufferFactory.h"

std::vector<float> labelColor{ 0.0f, 0.0f, 0.0f, 0.0f };

namespace
{
    const VkCommandBuffer& GetCommandBuffer(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
    {
        auto containsPoolId = std::holds_alternative<uint32_t>(cmdBufInfo.GetPool());
        if (containsPoolId)
        {
            return GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->GetCommandBufferFromPool(cmdBufInfo.GetBufferId(), std::get<uint32_t>(cmdBufInfo.GetPool()));
        }
        else
        {
            Core::Enums::QueueType queueType = std::get<Core::Enums::QueueType>(cmdBufInfo.GetPool());

            VkQueueFlagBits flag;
            switch (queueType)
            {
            case Core::Enums::QueueType::RENDER:
                flag = VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT;
                break;

            case Core::Enums::QueueType::COMPUTE:
                flag = VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
                break;

            case Core::Enums::QueueType::TRANSFER:
                flag = VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT;
                break;

            default:
                ASSERT_MSG_DEBUG(0, "Command buffer for this queue not supported");
            }
            return GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->GetCommandBuffer(cmdBufInfo.GetBufferId(), flag);
        }
        ASSERT_MSG_DEBUG(0, "ERROR");
        return VkCommandBuffer{};
    }
}

void GfxVk::CommandWriter::SetViewport(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY, float minDepth, float maxDepth)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);

    VkViewport viewport;
    viewport.height = height;
    viewport.width = width;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    viewport.x = positionX;
    viewport.y = positionY;
    vkCmdSetViewport(cmBuf, 0, 1, &viewport);
}

void GfxVk::CommandWriter::SetScissor(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);

    VkRect2D scissor;
    scissor.extent.width = (uint32_t)width;
    scissor.extent.height = (uint32_t)height;
    scissor.offset.x = (int32_t)positionX;
    scissor.offset.y = (int32_t)positionY;
    vkCmdSetScissor(cmBuf, 0, 1, &scissor);
}

void GfxVk::CommandWriter::SetDepthBias(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float depthBiasConstant, float depthBiasClamp, float depthBiasSlope)
{
}

void GfxVk::CommandWriter::BeginRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::RenderPassBeginInfo& renderPassBeginInfo, const Core::Enums::SubpassContentStatus& subpassContentStatus)
{
}

void GfxVk::CommandWriter::EndRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
{
}

void GfxVk::CommandWriter::BindPipeline(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Enums::PipelineType& type, uint32_t pipelineId)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);
    VkPipelineBindPoint bindPoint;
    switch (type)
    {
    case Core::Enums::PipelineType::COMPUTE :
        bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE;
        break;
    case Core::Enums::PipelineType::GRAPHICS:
        bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
        break;
    default:
        ASSERT_MSG_DEBUG(0, "invalid option");
    }

    const VkPipeline& pipeline = GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->GetPipeline(pipelineId);

    vkCmdBindPipeline(cmBuf, bindPoint, pipeline);
}

void GfxVk::CommandWriter::BindDescriptorSet(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DescriptorSetBindingInfo& info, uint32_t pipelineLayoutId)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);

    VkPipelineBindPoint bindPoint;
    switch (info.pipelineBindPoint)
    {
    case Core::Enums::PipelineType::GRAPHICS:
        bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        break;

    case Core::Enums::PipelineType::COMPUTE:
        bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Invalid option");
    }

    VkPipelineLayout* layout = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineLayout(pipelineLayoutId);
    std::vector<VkDescriptorSet> sets = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetDescriptors(info.descriptorSetIds.data(), (uint32_t)info.descriptorSetIds.size());
    vkCmdBindDescriptorSets(cmBuf, bindPoint, *layout, info.firstSet,
        info.numSetsToBind, &sets[info.firstSet], info.dynamicOffsetCount, info.pDynamicOffsets);
}

void GfxVk::CommandWriter::BindVertexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::VertexBufferBindingInfo& info)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);
    std::vector<VkBuffer> buffers;
    for (auto& id : info.bufferIds)
    {
        buffers.push_back(*GfxVk::Shading::VkBufferFactory::GetInstance()->GetBuffer(id));
    }
    vkCmdBindVertexBuffers(cmBuf, info.firstBinding, info.bindingCount, buffers.data(), info.pOffsets.data());
}

void GfxVk::CommandWriter::BindIndexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexBufferBindingInfo& info)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);

    VkIndexType indexType;
    switch (info.indexType)
    {
    case Core::Enums::IndexType::INDEX_TYPE_UINT16:
        indexType = VK_INDEX_TYPE_UINT16;
        break;
    case Core::Enums::IndexType::INDEX_TYPE_UINT32:
        indexType = VK_INDEX_TYPE_UINT32;
        break;
    default: ASSERT_MSG_DEBUG(0, "index type invalid");
    }

    VkBuffer indBuf = *GfxVk::Shading::VkBufferFactory::GetInstance()->GetBuffer(info.bufferId);
    vkCmdBindIndexBuffer(cmBuf, indBuf, info.offset, indexType);
}

void GfxVk::CommandWriter::DrawIndex(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexedDrawInfo& info)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);
    vkCmdDrawIndexed(cmBuf, info.indexCount, info.instanceCount, info.firstIndex, info.vertexOffset, info.firstInstance);
}

void GfxVk::CommandWriter::Draw(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DrawArrayInfo& info)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);
    vkCmdDraw(cmBuf, info.vertexCount, info.instanceCount, info.firstVertex, info.firstInstance);
}

void GfxVk::CommandWriter::SetPipelineBarrier(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, uint32_t barrierId)
{
    VkCommandBuffer cmBuf = GetCommandBuffer(cmdBufInfo);
    VkDependencyInfo dependencyInfo = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetDependencyInfo(barrierId);

    vkCmdPipelineBarrier2(cmBuf, &dependencyInfo);
}

void GfxVk::CommandWriter::BeginRendering(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, uint32_t renderingInfoId)
{
    VkCommandBuffer cmdBuf = GetCommandBuffer(cmdBufInfo);
    const VkRenderingInfo& info = GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->GetRenderingInfo(renderingInfoId);

    vkCmdBeginRendering(cmdBuf, &info);
}

void GfxVk::CommandWriter::EndRendering(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
{
    VkCommandBuffer cmdBuf = GetCommandBuffer(cmdBufInfo);
    vkCmdEndRendering(cmdBuf);
}

void GfxVk::CommandWriter::CommandBeginLabel(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const std::string& label)
{
    VkCommandBuffer cmdBuf = GetCommandBuffer(cmdBufInfo);
    GfxVk::DebugMarker::SetCommandBufferBeginLabel(cmdBuf, label.c_str(), labelColor);
}

void GfxVk::CommandWriter::CommandEndLabel(const Core::Wrappers::CommandBufferInfo& cmdBufInfo)
{
    VkCommandBuffer cmdBuf = GetCommandBuffer(cmdBufInfo);
    GfxVk::DebugMarker::SetCommandBufferEndLabel(cmdBuf);
}

void GfxVk::CommandWriter::InsertLabel(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const std::string& label)
{
    VkCommandBuffer cmdBuf = GetCommandBuffer(cmdBufInfo);
    GfxVk::DebugMarker::InsertLabelInCommandBuffer(cmdBuf, label.c_str(), labelColor);
}

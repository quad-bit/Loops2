#include "VkCommandWriter.h"
#include <vulkan/vulkan.h>
#include "VkCommandBufferFactory.h"
#include <Platform/Assertion.h>
#include "synchronisation/VkSynchroniserFactory.h"
#include "VkRenderPassFactory.h"
#include <utility/VkDebugMarkerUtil.h>

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
}

void GfxVk::CommandWriter::SetScissor(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY)
{
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
}

void GfxVk::CommandWriter::BindDescriptorSet(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DescriptorSetBindingInfo& info)
{
}

void GfxVk::CommandWriter::BindVertexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::VertexBufferBindingInfo& info)
{
}

void GfxVk::CommandWriter::BindIndexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexBufferBindingInfo& info)
{
}

void GfxVk::CommandWriter::DrawIndex(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexedDrawInfo& info)
{
}

void GfxVk::CommandWriter::Draw(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DrawArrayInfo& info)
{
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

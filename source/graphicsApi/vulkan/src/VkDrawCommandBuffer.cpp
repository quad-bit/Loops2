#include "VkDrawCommandBuffer.h"
#include "utility/VkRenderingUnwrapper.h"
#include "pipeline/VulkanGraphicsPipelineFactory.h"
#include "shading/VkShaderResourceManager.h"
#include "shading/VkBufferFactory.h"

void GfxVk::CommandPool::VkDrawCommandBuffer::SetViewport(const float & width, const float & height, const float & positionX, const float & positionY, const float & minDepth, const float & maxDepth)
{
    VkViewport viewport;
    viewport.height = height;
    viewport.width = width;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    viewport.x = positionX;
    viewport.y = positionY;
    vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::SetScissor(const float & width, const float & height, const float & positionX, const float & positionY)
{
    VkRect2D scissor;
    scissor.extent.width = (uint32_t)width;
    scissor.extent.height = (uint32_t)height;
    scissor.offset.x = (int32_t)positionX;
    scissor.offset.y = (int32_t)positionY;
    vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::SetDepthBias(const float & depthBiasConstant, const float & depthBiasClamp, const float & depthBiasSlope)
{
    vkCmdSetDepthBias(*commandBuffer, depthBiasConstant, depthBiasClamp, depthBiasSlope);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::BeginRenderPass(Core::Wrappers::RenderPassBeginInfo * renderPassBeginInfo, Core::Enums::SubpassContentStatus * subpassContentStatus)
{
    VkRenderPassBeginInfo vkRenderPassBeginInfo = GfxVk::Unwrap::UnwrapRenderPassBeginInfo(*renderPassBeginInfo);
    
    VkSubpassContents subpassContent;
    switch (*subpassContentStatus)
    {
    case Core::Enums::SubpassContentStatus::SUBPASS_CONTENTS_INLINE:
        subpassContent = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE;
        break;

    case Core::Enums::SubpassContentStatus::SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS:
        subpassContent = VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
        break;
    }

    vkCmdBeginRenderPass(*commandBuffer, &vkRenderPassBeginInfo, subpassContent);

    delete vkRenderPassBeginInfo.pClearValues;
}

void GfxVk::CommandPool::VkDrawCommandBuffer::EndRenderPass()
{
    //vkEndCommandBuffer(*commandBuffer);
    vkCmdEndRenderPass(*commandBuffer);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::BindPipeline(Core::Enums::PipelineType * type, const uint32_t & pipelineId)
{
    VkPipelineBindPoint bindPoint;
    switch(*type)
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

    vkCmdBindPipeline(*commandBuffer, bindPoint, *GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->GetPipeline(pipelineId));
}

void GfxVk::CommandPool::VkDrawCommandBuffer::BindDescriptorSet(Core::Wrappers::DescriptorSetBindingInfo * info)
{
    VkPipelineBindPoint bindPoint;
    switch (*info->pipelineBindPoint)
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
/*
    static int count = 0;
    uint32_t size, offset;

    switch (count)
    {
    case 0 :
        size = 5;
        offset = 0;
        break;

    case 1:
        size = 1;
        offset = 4;
        break;

    case 2:
        size = 3;
        offset = 2;
        break;

    case 3:
        size = 1;
        offset = 4;
        break;
    }

    count++;
    if (count >= 4)
    {
        count = 0;
    }
    uint32_t temp = info->firstSet;
*/

    /*
    INFO : vkCmdBindDescriptorSets
    array bound [a, b, c, d]
    firstSet = 0, size = 4, address = &array[0]

    array bound [a, b, c, e]
    firstSet = 3, size = 1, address = &array[3]

    array bound [a, b, c, f, g]
    firstSet = 4, size = 2, address = &array[4]
    */

    VkPipelineLayout* layout = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineLayout(info->pipelineLayoutId);
    uint32_t numSets = (uint32_t)info->descriptorSetIds.size();

    uint32_t firstSet, numSetsToBind;
    std::vector<VkDescriptorSet> setList;
    std::tie(setList, firstSet) = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetDescriptors(
        info->descriptorSetIds.data(), numSets, info->pipelineLayoutId, info->firstSet);

    numSetsToBind = (uint32_t)setList.size() - firstSet;

    vkCmdBindDescriptorSets(*commandBuffer, bindPoint, *layout, firstSet, 
      numSetsToBind, &setList[firstSet], info->dynamicOffsetCount, info->pDynamicOffsets);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::BindVertexBuffers(Core::Wrappers::VertexBufferBindingInfo * info)
{
    std::vector<VkBuffer> buffers;
    for (uint32_t i = 0; i < (uint32_t)info->bufferIds.size(); i++)
    {
        buffers.push_back(*GfxVk::Shading::VkBufferFactory::GetInstance()->GetBuffer(info->bufferIds[i]));
    }

    for (uint32_t i = 0; i < (uint32_t)info->pOffsets.size(); i++)
    {
        buffers.push_back(*GfxVk::Shading::VkBufferFactory::GetInstance()->GetBuffer(info->bufferIds[i]));
    }

    vkCmdBindVertexBuffers(*commandBuffer, info->firstBinding, info->bindingCount, buffers.data(), info->pOffsets.data());
}

void GfxVk::CommandPool::VkDrawCommandBuffer::BindIndexBuffers(Core::Wrappers::IndexBufferBindingInfo * info)
{
    VkIndexType indexType;
    switch (info->indexType)
    {
    case Core::Enums::IndexType::INDEX_TYPE_UINT16:
        indexType = VK_INDEX_TYPE_UINT16;
        break;
    case Core::Enums::IndexType::INDEX_TYPE_UINT32:
        indexType = VK_INDEX_TYPE_UINT32;
        break;
    default: ASSERT_MSG_DEBUG(0, "index type invalid");
    }

    VkBuffer indBuf = *GfxVk::Shading::VkBufferFactory::GetInstance()->GetBuffer(info->bufferId);
    vkCmdBindIndexBuffer(*commandBuffer, indBuf, info->offset, indexType);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::DrawIndex(Core::Wrappers::IndexedDrawInfo * info)
{
    vkCmdDrawIndexed(*commandBuffer, info->indexCount, info->instanceCount, info->firstIndex, info->vertexOffset, info->firstInstance);
}

void GfxVk::CommandPool::VkDrawCommandBuffer::Draw(Core::Wrappers::DrawArrayInfo * info)
{
    vkCmdDraw(*commandBuffer, info->vertexCount, info->instanceCount, info->firstVertex, info->firstInstance);
}

#pragma once

#include <vulkan\vulkan.h>
#include <RenderingWrapper.h>

class VkDrawCommandBuffer
{
public:
    uint32_t id;
    VkCommandBuffer * commandBuffer;
    VkCommandPool * pool;
    VkCommandBufferLevel level;
    PipelineType commandBufferType;

    void SetViewport(const float & width, const float & height, const float & positionX, const float & positionY, const float & minDepth, const float & maxDepth);
    void SetScissor(const float & width, const float & height, const float & positionX, const float & positionY);
    void SetDepthBias(const float & depthBiasConstant, const float & depthBiasClamp, const float & depthBiasSlope);

    void BeginRenderPass(RenderPassBeginInfo * renderPassBeginInfo, SubpassContentStatus * subpassContentStatus);
    void EndRenderPass();

    void BindPipeline(PipelineType * type, const uint32_t & pipelineId);
    void BindDescriptorSet(DescriptorSetBindingInfo * info);
    void BindVertexBuffers(VertexBufferBindingInfo * info);
    void BindIndexBuffers(IndexBufferBindingInfo * info);
    void DrawIndex(IndexedDrawInfo * info);
    void Draw(DrawArrayInfo * info);

};
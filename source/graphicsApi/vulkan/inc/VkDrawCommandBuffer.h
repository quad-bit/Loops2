#pragma once

#include <vulkan/vulkan.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace GfxVk
{
    namespace CommandPool
    {
        class VkDrawCommandBuffer
        {
        public:
            uint32_t id;
            VkCommandBuffer* commandBuffer;
            VkCommandPool* pool;
            VkCommandBufferLevel level;
            Core::Enums::PipelineType commandBufferType;

            void SetViewport(const float& width, const float& height, const float& positionX, const float& positionY, const float& minDepth, const float& maxDepth);
            void SetScissor(const float& width, const float& height, const float& positionX, const float& positionY);
            void SetDepthBias(const float& depthBiasConstant, const float& depthBiasClamp, const float& depthBiasSlope);

            void BeginRenderPass(Core::Wrappers::RenderPassBeginInfo* renderPassBeginInfo, Core::Enums::SubpassContentStatus* subpassContentStatus);
            void EndRenderPass();

            void BindPipeline(Core::Enums::PipelineType* type, const uint32_t& pipelineId);
            void BindDescriptorSet(Core::Wrappers::DescriptorSetBindingInfo* info);
            void BindVertexBuffers(Core::Wrappers::VertexBufferBindingInfo* info);
            void BindIndexBuffers(Core::Wrappers::IndexBufferBindingInfo* info);
            void DrawIndex(Core::Wrappers::IndexedDrawInfo* info);
            void Draw(Core::Wrappers::DrawArrayInfo* info);
        };
    }
}
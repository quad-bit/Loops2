#ifndef GFXVULKAN_COMMANDWRITER_H_
#define GFXVULKAN_COMMANDWRITER_H_

#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <variant>

namespace GfxVk
{
    namespace CommandWriter
    {
        void SetViewport(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY, float minDepth, float maxDepth);
        void SetScissor(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float width, float height, float positionX, float positionY);
        void SetDepthBias(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, float depthBiasConstant, float depthBiasClamp, float depthBiasSlope);

        void BeginRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::RenderPassBeginInfo& renderPassBeginInfo, const Core::Enums::SubpassContentStatus& subpassContentStatus);
        void EndRenderPass(const Core::Wrappers::CommandBufferInfo& cmdBufInfo);

        void BindPipeline(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Enums::PipelineType& type, uint32_t pipelineId);
        void BindDescriptorSet(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DescriptorSetBindingInfo& info);
        void BindVertexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::VertexBufferBindingInfo& info);
        void BindIndexBuffers(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexBufferBindingInfo& info);
        void DrawIndex(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::IndexedDrawInfo& info);
        void Draw(const Core::Wrappers::CommandBufferInfo& cmdBufInfo, const Core::Wrappers::DrawArrayInfo& info);
    }
}

#endif//VKCOMMANDWRITER
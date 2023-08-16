#pragma once
#include <stdint.h>

#if (RENDERING_API == VULKAN)
class VkDrawCommandBuffer;
#elif (RENDERING_API == DX)

#endif

enum class CommandBufferLevel;
enum class PipelineType;
enum class SubpassContentStatus;

template <typename T>
class DrawCommandBuffer
{
private:
    T * apiInterface;
    uint32_t poolId;
    uint32_t queueId;
    uint32_t cmdBufferId;
    CommandBufferLevel* commandBufferLevel;
    PipelineType* bufferType;

#if (RENDERING_API == VULKAN)
    VkDrawCommandBuffer * commandInterface;
#elif (RENDERING_API == DX)
    
#endif

public:
    void Init(T * apiInterface, CommandBufferLevel* level, const uint32_t & poolId);
    void DeInit();
    DrawCommandBuffer();
    ~DrawCommandBuffer();

#if (RENDERING_API == VULKAN)
    void SetCommandInterface(VkDrawCommandBuffer * commandInterface);
#elif (RENDERING_API == DX)

#endif
    const uint32_t & GetId() { return cmdBufferId; }
    const uint32_t & GetPoolId() { return poolId; }

    void SetViewport(const float & width, const float & height, const float & positionX, const float & positionY, const float & minDepth, const float & maxDepth);
    void SetScissor(const float & width, const float & height, const float & positionX, const float & positionY);
    void SetDepthBias(const float & depthBiasConstant, const float & depthBiasClamp, const float & depthBiasSlope);
    void BeginRenderPass(RenderPassBeginInfo * renderPassBeginInfo, SubpassContentStatus * subpassContentStatus);
    void EndRenderPass();

    void BindPipeline(PipelineType * type, const uint32_t & pipelineId);

    void BindDescriptorSets(DescriptorSetBindingInfo * info);
    void BindVertexBuffers(VertexBufferBindingInfo * info);
    void BindIndexBuffer(IndexBufferBindingInfo * info);
    void DrawIndex(IndexedDrawInfo * info);
    void Draw(DrawArrayInfo * info);
};

#include "RenderingWrapper.h"

#if (RENDERING_API == VULKAN)
#include <VkDrawCommandBuffer.h>
#elif (RENDERING_API == DX)

#endif
#include <CorePrecompiled.h>

template<typename T>
inline void DrawCommandBuffer<T>::Init(T * apiInterface, CommandBufferLevel * level, const uint32_t & poolId)
{
    PLOGD << "Draw command buffer Init";
    this->apiInterface = apiInterface;

    PipelineType * bufType = new PipelineType;
    this->bufferType = bufType;

#if (RENDERING_API == VULKAN)
    commandInterface = apiInterface->CreateCommandBuffer(poolId, &cmdBufferId, level, PipelineType::GRAPHICS);
#elif (RENDERING_API == DX)
    apiInterface = new DxInterface();
#endif
    
    this->poolId = poolId;
    this->commandBufferLevel = level;
}

template<typename T>
inline void DrawCommandBuffer<T>::DeInit()
{
    PLOGD << "Draw command buffer DeInit";

    apiInterface->DestroyCommandBuffer(cmdBufferId);
    delete commandBufferLevel;
    delete bufferType;
}

template<typename T>
inline DrawCommandBuffer<T>::DrawCommandBuffer()
{
}

template<typename T>
inline DrawCommandBuffer<T>::~DrawCommandBuffer()
{
}

#if (RENDERING_API == VULKAN)
template<typename T>
inline void DrawCommandBuffer<T>::SetCommandInterface(VkDrawCommandBuffer * commandInterface)
{

}

#elif (RENDERING_API == DX)

#endif

template<typename T>
inline void DrawCommandBuffer<T>::SetViewport(const float & width, const float & height, const float & positionX, const float & positionY, const float & minDepth, const float & maxDepth)
{
    commandInterface->SetViewport(width, height, positionX, positionY, minDepth, maxDepth);
}

template<typename T>
inline void DrawCommandBuffer<T>::SetScissor(const float & width, const float & height, const float & positionX, const float & positionY)
{
    commandInterface->SetScissor(width, height, positionX, positionY);
}

template<typename T>
inline void DrawCommandBuffer<T>::SetDepthBias(const float & depthBiasConstant, const float & depthBiasClamp, const float & depthBiasSlope)
{
    commandInterface->SetDepthBias(depthBiasConstant, depthBiasClamp, depthBiasSlope);
}

template<typename T>
inline void DrawCommandBuffer<T>::BeginRenderPass(RenderPassBeginInfo * renderPassBeginInfo, SubpassContentStatus * subpassContentStatus)
{
    commandInterface->BeginRenderPass(renderPassBeginInfo, subpassContentStatus);
}

template<typename T>
inline void DrawCommandBuffer<T>::EndRenderPass()
{
    commandInterface->EndRenderPass();
}

template<typename T>
inline void DrawCommandBuffer<T>::BindPipeline(PipelineType * type, const uint32_t & pipelineId)
{
    commandInterface->BindPipeline(type, pipelineId);
}

template<typename T>
inline void DrawCommandBuffer<T>::BindDescriptorSets(DescriptorSetBindingInfo * info)
{
    commandInterface->BindDescriptorSet(info);
}

template<typename T>
inline void DrawCommandBuffer<T>::BindVertexBuffers(VertexBufferBindingInfo * info)
{
    commandInterface->BindVertexBuffers(info);
}

template<typename T>
inline void DrawCommandBuffer<T>::BindIndexBuffer(IndexBufferBindingInfo * info)
{
    commandInterface->BindIndexBuffers(info);
}

template<typename T>
inline void DrawCommandBuffer<T>::DrawIndex(IndexedDrawInfo * info)
{
    commandInterface->DrawIndex(info);
}

template<typename T>
inline void DrawCommandBuffer<T>::Draw(DrawArrayInfo * info)
{
    commandInterface->Draw(info);
}

#pragma once
#include <stdint.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <optional>
#include <Utility/ShaderResourceDescription.h>

//using namespace std;
namespace Core
{
    namespace Utility
    {
        struct ShaderBindingDescription;
        struct DescriptorSetInfo;
        struct ShaderStateWrapper;
        struct InputAssemblyWrapper;
        struct VertexInputWrapper;
        struct TessellationStateWrapper;
        struct ViewPortStateWrapper;
        struct RasterizationStateWrapper;
        struct DepthStencilStateWrapper;
        struct ColorBlendStateWrapper;
        struct MultiSampleStateWrapper;
        struct DynamicStateWrapper;
    }

    namespace Wrappers
    {
        struct ImageInfo;
        struct RenderPassAttachmentInfo;
        struct SubpassInfo;
        struct SubpassDependency;
        struct RenderPassBeginInfo;

        struct CommandBufferInheritanceInfo;
        struct SubmitInfo;
        struct PresentInfo;
        struct QueueWrapper;
        struct BufferInfo;

        struct SetWrapper;
        struct VertexInputAttributeInfo;
        struct VertexInputBindingInfo;
        struct PipelineCreateInfo;
    }

    namespace Enums
    {
        enum class Format;
        enum class CommandBufferLevel;
        enum class PipelineType;
        enum class CommandPoolProperty;
        enum class PipelineStage;
        enum class CommandBufferUsage;
        enum class MemoryType;
        enum class BufferType;
        enum class PrimtiveType;
        enum class ShaderType;
    }
}

namespace GfxVk
{
    namespace CommandPool
    {
        class VkDrawCommandBuffer;
    }
}

namespace Renderer
{
    namespace Utility
    {
        namespace VulkanInterface
        {
            Core::Enums::Format GetWindowSurfaceFormat();
            Core::Enums::ColorSpace GetWindowColorSpace();

            Core::Enums::Format FindBestDepthFormat();

            void CreateImageView(const Core::Wrappers::ImageViewCreateInfo& viewInfo, uint32_t imageId);
            uint32_t CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name);
            uint32_t CreateImage(void* buffer, size_t bufferSize, const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name);
            void DestroyImage(uint32_t imageId, bool freeImageMemory);
            uint32_t CreateCubemap(const std::string& path, const std::string& imageName);

            void CreateRenderPass(
                const Core::Wrappers::RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t& attachmentCount,
                const Core::Wrappers::SubpassInfo* subpassList, const uint32_t& subpassCount,
                const Core::Wrappers::SubpassDependency* dependencyList, const uint32_t& dependencyCount,
                uint32_t& renderPassId);

            void DestroyRenderPass(const uint32_t& id);

            void CreateFrameBuffer(uint32_t numFrameBuffers, uint32_t* imageViewId, const uint32_t& viewsPerFB,
                uint32_t renderPassId, const uint32_t& width, const uint32_t& height, uint32_t* ids);
            void DestroyFrameBuffer(uint32_t* pid, const uint32_t& count);

            uint32_t CreateCommandPool(Core::Enums::PipelineType* pipelineType, Core::Enums::CommandPoolProperty* prop);
            void DestroyCommandPool(const uint32_t& poolId);

            uint32_t CreateFence(bool isSignaled);
            uint32_t Create_Semaphore(bool isSignaled);

            void DestroyFence(const uint32_t& id);
            void DestroySemaphore(const uint32_t& id);

            void SetRenderpassBeginInfo(Core::Wrappers::RenderPassBeginInfo* beginInfo, const uint32_t& renderPassId);

            //uint32_t GetAvailableSwapchainIndex(const uint32_t& fenceId, const uint32_t& semaphoreId);
            uint32_t GetAvailableSwapchainIndex(const std::optional<uint32_t>& fenceId, const std::optional<uint32_t>& semaphoreId);

            uint32_t CreateCommandBuffer(const Core::Enums::QueueType& queueType);
            void DestroyCommandBuffer(uint32_t id, const Core::Enums::QueueType& queueType);
            void DestroyCommandBuffer(uint32_t id, uint32_t poolId);
            void ResetCommandBuffer(const uint32_t& id, const uint32_t& poolId);
            void BeginCommandBufferRecording(const uint32_t& id, const Core::Enums::QueueType& queueType,
                const Core::Enums::CommandBufferUsage& usage, const std::optional<Core::Wrappers::CommandBufferInheritanceInfo>& inheritanceInfo);
            void EndCommandBufferRecording(const uint32_t& id, const Core::Enums::QueueType& queueType);

            void SubmitJob(const Core::Wrappers::QueueWrapper* queueWrapper, const Core::Wrappers::SubmitInfo* info, const uint32_t& submitInfoCount, const uint32_t& fenceId);
            void SubmitJob(const Core::Wrappers::SubmitInfo* info, const uint32_t& submitInfoCount, const std::optional<uint32_t>& fenceId);

            uint32_t GetSwapchainImageId(uint32_t swapchainIndex);
            void PresentSwapchainImage(const Core::Wrappers::QueueWrapper* queueWrapper, const Core::Wrappers::PresentInfo* info, const uint32_t& presentQueueId);
            void PresentSwapchainImage(const Core::Wrappers::PresentInfo& info);

            bool IsApplicationSafeForClosure();
            // =============================BUFFER=========================================
            uint32_t CreateBuffer(const Core::Wrappers::BufferCreateInfo& info, const std::string& name);

            size_t GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize);
            // returns memid
            uint32_t AllocateAndBindBufferMemory(uint32_t bufferId,
                Core::Enums::MemoryType* userReq,
                const uint32_t& numUserReq,
                bool isMemoryShared,
                std::optional<uint32_t> allocationSize
                );
            uint32_t AllocateMemory(const Core::Wrappers::MemoryRequirementInfo& memReq, Core::Enums::MemoryType* userReq, const uint32_t& numUserReq, const size_t& allocationSize);
            Core::Wrappers::MemoryRequirementInfo GetImageMemoryRequirement(const uint32_t& imageId);
            void CopyBufferDataToMemory(const uint32_t& bufId, const VkDeviceSize& dataSize, const VkDeviceSize& memAlignedSize, void* data, VkDeviceSize memoryOffset, bool keepMemoryMounted);
            void DestroyBuffer(uint32_t* ids, const uint32_t& count, bool freeMemory);

            // =============================BUFFER=========================================

            void FreeMemory(uint32_t* ids, const uint32_t& count);
            uint32_t* AllocateDescriptorsSet(Core::Wrappers::SetWrapper* set, const uint32_t& numDescriptors);
            void BindImageMemory(const uint32_t& imageId, const uint32_t& memId, const size_t& offset);

            void GetShaderIds(char** shaderName, Core::Enums::ShaderType* type, uint32_t* id, const uint32_t& shaderCount);
            uint32_t CreateVertexInputState(const Core::Utility::VertexInputWrapper& vertexInputWrapper);
            uint32_t CreateInputAssemblyState(const Core::Utility::InputAssemblyWrapper& InputAssemblyWrapper);
            uint32_t CreateShaderState(const Core::Utility::ShaderStateWrapper& shaderStateWrapper);
            uint32_t CreateTessellationState(const Core::Utility::TessellationStateWrapper& wrapper);
            uint32_t CreateViewportState(const Core::Utility::ViewPortStateWrapper& wrapper);
            uint32_t CreateRasterisationState(const Core::Utility::RasterizationStateWrapper& wrapper);
            uint32_t CreateDepthStencilState(const Core::Utility::DepthStencilStateWrapper& wrapper);
            uint32_t CreateColorBlendState(const Core::Utility::ColorBlendStateWrapper& wrapper);
            uint32_t CreateMultiSampleState(const Core::Utility::MultiSampleStateWrapper& wrapper);
            uint32_t CreateDynamicState(const Core::Utility::DynamicStateWrapper& wrapper);
            void CreatePipeline(Core::Wrappers::PipelineCreateInfo* info, const uint32_t& pipelineCount, uint32_t* pipelineId);
            uint32_t CreatePipeline(const Core::Wrappers::PipelineCreateInfo& info);
            uint32_t CreatePipeline(const Core::Wrappers::ComputePipelineCreateInfo& info);
            //uint32_t CreatePipeline(uint32_t pipelineId);

            const Core::Utility::EffectId& GetEffectId(const std::string& effectName);
            const Core::Utility::TechniqueId& GetTechniqueId(const Core::Utility::EffectId& effectId, const std::string& techName);
            const Core::Utility::TaskId& GetTaskId(const Core::Utility::EffectId& effectId, const Core::Utility::TechniqueId& techId, const std::string& taskName);

            uint32_t GetVertexInputStateId(const Core::Utility::TaskId& taskId);
            uint32_t GetPipelineLayoutId(const Core::Utility::TaskId& taskId);
            uint32_t GetShaderStateId(const Core::Utility::TaskId& taskId);
            const std::vector<Core::Wrappers::VertexBindingTypeInfo>& GetVertexBindingTypeInfo(const Core::Utility::TaskId& taskId);

            std::vector<Core::Wrappers::SetWrapper*> GetSetsForShaders(const std::vector<std::string>& shaderNames);
            uint32_t CreatePipelineLayout(Core::Wrappers::SetWrapper** setWrapperList, const size_t& numSets);
            std::vector<Core::Wrappers::SetWrapper*>* GetSetWrapperList();
            std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>>* GetPerSetSetwrapperList();
            void LinkSetBindingToResources(Core::Utility::DescriptorSetInfo desc, const uint32_t& numDescriptorSets);

            bool IsSampleRateShadingAvailable();
            Core::Enums::Samples GetMaxUsableSampleCount();

            uint32_t CreateSampler(const Core::Wrappers::SamplerCreateInfo& info);
            uint32_t CreateCubemapSampler();

            uint32_t CreateBarrier(
                const std::vector<Core::Wrappers::ImageBarrier2>& imageBarriers,
                const std::vector<Core::Wrappers::BufferBarrier2>& bufferBarriers,
                const std::vector<Core::Wrappers::MemoryBarrier2>& memoryBarriers
                );
            uint32_t CreateRenderingInfo(const Core::Wrappers::RenderingInfo& renderingInfo);

            
        }
    }
}

namespace VulkanInterfaceAlias = Renderer::Utility::VulkanInterface;

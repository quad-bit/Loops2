#pragma once
#include <stdint.h>
#include <vector>
#include <vulkan\vulkan.h>
#include "RenderingWrapper.h"

//using namespace std;

struct ImageInfo;
struct RenderPassAttachmentInfo;
struct SubpassInfo;
struct SubpassDependency;
struct RenderPassBeginInfo;
struct ShaderBindingDescription;

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

struct CommandBufferInheritanceInfo;
struct SubmitInfo;
struct PresentInfo;
struct QueueWrapper;
struct BufferInfo;

struct SetWrapper;
struct VertexInputAttributeInfo;
struct VertexInputBindingInfo;
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
struct PipelineCreateInfo;

class VkDrawCommandBuffer;

class VulkanInterface
{
private:

    VkAttachmentDescription* UnwrapAttachmentDesc(const RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t & attachmentCount);
    VkSubpassDescription* UnwrapSubpassDesc(const SubpassInfo* subpassList, const uint32_t & subpassCount);
    VkSubpassDependency* UnwrapSubpassDependency(const SubpassDependency* dependencyList, const uint32_t & dependencyCount);

    //VkImageCreateInfo UnwrapImageInfo(ImageInfo * info);
    //VkImageViewCreateInfo UnwrapImageViewInfo(ImageInfo * info);
    VkCommandBufferUsageFlagBits UnwrapCommandBufferUsage(const CommandBufferUsage * info);
    VkSubmitInfo * UnwrapSubmitInfo(const SubmitInfo * info);
    VkPipelineStageFlags * UnwrapStageFlags(const PipelineStage * pipelineStage);
    //deprecated
    VkMemoryPropertyFlags UnwrapMemoryProperty(const MemoryType * memType);
    VkBufferUsageFlags UnwrapBufferUsageFlags(const BufferType * type);

    Format WrapFormat(VkFormat format);
    ColorSpace WrapColorSpace(VkColorSpaceKHR space);

public:
    VulkanInterface();
    ~VulkanInterface();

    void Init();
    void DeInit();
    
    Format GetWindowSurfaceFormat();
    ColorSpace GetWindowColorSpace();

    int FindBestDepthFormat(Format * imageFormat, const uint32_t & count);

    void SetupPresentationEngine(ImageInfo info);
    void CreateRenderTarget(ImageInfo * info, ImageViewInfo * viewInfo, uint32_t & count, bool defaultTarget,
        uint32_t * ids);
    void CreateDefaultRenderTarget(ImageInfo info, ImageViewInfo viewInfo, uint32_t & count, uint32_t * ids);
    //deprecated.
    void DestroyRenderTarget(std::vector<uint32_t>* ids, bool defaultTarget);
    
    void CreateAttachment(ImageInfo * info, const uint32_t & count, uint32_t * ids);
    //deprecated.
    void DestroyDepthTarget(std::vector<uint32_t>* ids, bool defaultTarget);

    void DestroyAttachment(uint32_t * ids, bool * destroyImageView, bool * freeImageMemory, const uint32_t & count);
    void FreeAttachmentMemory(uint32_t * imageIds, const uint32_t & count);
    void DestroySwapChainImageViews(uint32_t * ids, const uint32_t & count);

    void CreateImageView(ImageViewInfo * viewInfo, uint32_t & count);

    void CreateRenderPass(
        const RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t & attachmentCount,
        const SubpassInfo* subpassList, const uint32_t & subpassCount,
        const SubpassDependency* dependencyList, const uint32_t & dependencyCount,
        uint32_t& renderPassId);

    void DestroyRenderPass(const uint32_t & id );

    void CreateFrameBuffer(uint32_t numFrameBuffers, uint32_t * imageViewId, const uint32_t & viewsPerFB,
        uint32_t renderPassId, const uint32_t & width, const uint32_t & height, uint32_t * ids);
    void DestroyFrameBuffer(uint32_t * pid, const uint32_t & count);

    uint32_t CreateCommandPool(PipelineType* pipelineType, CommandPoolProperty * prop);
    void DestroyCommandPool(const uint32_t & poolId);

    uint32_t CreateFence(bool isSignaled);
    uint32_t Create_Semaphore(bool isSignaled);

    void DestroyFence(const uint32_t & id );
    void DestroySemaphore(const uint32_t & id );

    void SetRenderpassBeginInfo(RenderPassBeginInfo * beginInfo, const uint32_t & renderPassId);

    uint32_t GetAvailableSwapchainIndex(const uint32_t & fenceId, const uint32_t & semaphoreId);
    
    void ActivateCommandBuffer(const uint32_t & id);
    
#if (RENDERING_API == VULKAN)
    VkDrawCommandBuffer * CreateCommandBuffer(const uint32_t & poolId, uint32_t * cmdBufferId, CommandBufferLevel* commandBufferLevel, PipelineType bufferType);
#elif (RENDERING_API == DX)
    apiInterface = new DxInterface();
#endif

    void DestroyCommandBuffer(const uint32_t & id );
    void ResetCommandBuffer(const uint32_t & id , const uint32_t & poolId);
    void BeginCommandBufferRecording(const uint32_t & id , const CommandBufferUsage * usage, const CommandBufferInheritanceInfo * inheritanceInfo);
    void EndCommandBufferRecording(const uint32_t & id );

    void SubmitJob(const QueueWrapper * queueWrapper, const SubmitInfo * info, const uint32_t & submitInfoCount, const uint32_t & fenceId);
    void SubmitJob(const SubmitInfo * info, const uint32_t & submitInfoCount);

    void PresentSwapchainImage(const QueueWrapper * queueWrapper, const PresentInfo * info, const uint32_t & presentQueueId);

    bool IsApplicationSafeForClosure();

    //Deprecated.
    //uint32_t * CreateBuffer(BufferInfo * info, const uint32_t & count);
    //deprecated
    uint32_t * CreateBuffers(BufferInfo * info, const uint32_t & count);
    void CreateBuffers(BufferCreateInfo * info, const uint32_t & count, uint32_t * out_buffIds, size_t * out_bufferMemRequirements);

    size_t GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize);
    uint32_t * AllocateBufferMemory(uint32_t * bufferId, const uint32_t & bufCount);
    //deprecated
    uint32_t AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq, const size_t & allocationSize);
    uint32_t AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq, const uint32_t & numUserReq, const size_t & allocationSize);
    MemoryRequirementInfo GetImageMemoryRequirement(const uint32_t & imageId);
    void CopyBufferDataToMemory(const uint32_t & bufId, const VkDeviceSize & dataSize, const VkDeviceSize & memAlignedSize, void * data, VkDeviceSize memoryOffset, bool keepMemoryMounted);
    //void CopyBufferDataToMemory(const uint32_t & bufId, VkDeviceSize dataSize, void * data, VkDeviceSize memoryOffset, bool keepMemoryMounted = false);
    void DestroyBuffer(uint32_t * ids, const uint32_t & count);
    void FreeMemory(uint32_t * ids, const uint32_t & count);
    uint32_t * AllocateDescriptorsSet(SetWrapper * set, const uint32_t & numDescriptors);
    void BindImageMemory(const uint32_t & imageId, const uint32_t & memId, const size_t & offset);

    void GetShaderIds(char ** shaderName, ShaderType * type, uint32_t * id, const uint32_t & shaderCount );
    void CreateVertexInputState(const VertexInputWrapper * vertexInputWrapper);
    void CreateInputAssemblyState(const InputAssemblyWrapper * InputAssemblyWrapper);
    void CreateShaderState(const ShaderStateWrapper * shaderStateWrapper);
    void CreateTessellationState(const TessellationStateWrapper * wrapper);
    void CreateViewportState(const ViewPortStateWrapper * wrapper);
    void CreateRasterisationState(const RasterizationStateWrapper * wrapper);
    void CreateDepthStencilState(const DepthStencilStateWrapper * wrapper);
    void CreateColorBlendState(const ColorBlendStateWrapper * wrapper);
    void CreateMultiSampleState(const MultiSampleStateWrapper * wrapper);
    void CreateDynamicState(const DynamicStateWrapper * wrapper);
    void CreatePipeline(PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineId);

    std::vector<SetWrapper *> GetSetsForShaders(const std::vector<std::string> & shaderNames);
    uint32_t CreatePipelineLayout(SetWrapper ** setWrapperList, const size_t & numSets);
    std::vector<SetWrapper*> * GetSetWrapperList();
    void LinkSetBindingToResources(ShaderBindingDescription * desc, const uint32_t & numBindings);

    bool IsSampleRateShadingAvailable();
    Samples GetMaxUsableSampleCount();

    uint32_t CreateSampler(const SamplerCreateInfo & info);
};


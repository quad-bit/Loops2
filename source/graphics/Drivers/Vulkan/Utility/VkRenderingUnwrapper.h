#ifndef RenderingUnwrapper_H
#define RenderingUnwrapper_H

#include <RenderingWrapper.h>
#include <Assertion.h>
#include <vulkan\vulkan.h>

namespace VulkanUnwrap
{

    VkColorSpaceKHR UnWrapColorSpace(ColorSpace colorSpace);

    //VkImageViewType  UnWrapImageViewDegree(Dimensions degree);

    //VkImageType  UnWrapImageDegree(Dimensions degree);

    VkFormat UnWrapFormat(Format format);

    VkImageLayout UnWrapImageLayout(ImageLayout layout);

    VkAttachmentLoadOp UnWrapLoadOp(LoadOperation op);

    VkAttachmentStoreOp UnWrapStoreOp(StoreOperation op);

    VkAttachmentReference UnWrapAttachmentRef(AttachmentRef ref);

    VkSampleCountFlagBits UnWrapSampleCount(Samples sampleCount);
    Samples UnWrapSampleCount(VkSampleCountFlags vkSampleCount);
    VkImageType UnWrapImageType(ImageType type);
    VkImageUsageFlagBits UnwrapUsage(AttachmentUsage usage);
    VkImageUsageFlags UnwrapUsage(std::vector<AttachmentUsage>& usage);

    VkRenderPassBeginInfo UnwrapRenderPassBeginInfo(RenderPassBeginInfo info);

    VkShaderStageFlags UnwrapShaderStage(ShaderType type);
    VkShaderStageFlags UnwrapShaderStage(ShaderType * type, const uint32_t & count);

    VkImageViewCreateInfo UnWrapImageViewCreateInfo(ImageViewInfo * info);
    VkImageCreateInfo UnWrapImageCreateInfo(ImageInfo * info);
    VkComponentSwizzle UnWrapSwizzle(ComponentSwizzle obj);
    VkImageViewType UnwrapImageViewType(ImageViewType type);
    VkImageAspectFlags UnwrapAspectMask(ImageAspectFlag flag);
    VkMemoryRequirements UnwrapMemoryRequirements(MemoryRequirementInfo * info);
    VkMemoryPropertyFlags UnwrapMemoryProperty(const MemoryType * memType);
    VkPipelineStageFlags const UnwrapPipelineStageFlags(const PipelineStage * stages, const uint32_t & count);
    VkAccessFlags const UnwrapAccessFlags(const AccessFlagBits * stages, const uint32_t & count);
    VkDependencyFlags const UnwrapDependencyFlags(const DependencyFlagBits * stages, const uint32_t & count);
    VkBufferCreateInfo * const UnwrapBufferCreateInfo(const BufferCreateInfo * info, const uint32_t & count);
    VkBufferUsageFlags const UnwrapBufferUsage(const std::vector<BufferUsage> & usage);
    VkMemoryPropertyFlags UnwrapMemoryProperty(const MemoryType * memType, const uint32_t & count);

    VkSamplerCreateInfo const UnwrapSamplerCreateInfo(const SamplerCreateInfo & info);
    VkBorderColor const UnwrapBorderColor(const BorderColor & color);
    VkCompareOp const UnwrapCompareOp(const CompareOp & op);
    VkFilter const UnwrapFilter(const Filter & filter);
    VkSamplerMipmapMode const UnwrapMipMapMode(const SamplerMipmapMode & mode);
    VkSamplerAddressMode const UnwrapAddressMode(const SamplerAddressMode & mode);
}

#endif RenderingUnwrapper_H
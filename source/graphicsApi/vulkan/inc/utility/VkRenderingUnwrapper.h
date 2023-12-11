#ifndef RenderingUnwrapper_H
#define RenderingUnwrapper_H

#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <Platform/Assertion.h>
#include <vulkan/vulkan.h>

namespace GfxVk
{
    namespace Unwrap
    {
        VkColorSpaceKHR UnWrapColorSpace(Core::Enums::ColorSpace colorSpace);

        //VkImageViewType  UnWrapImageViewDegree(Dimensions degree);

        //VkImageType  UnWrapImageDegree(Dimensions degree);

        VkFormat UnWrapFormat(Core::Enums::Format format);

        VkImageLayout UnWrapImageLayout(Core::Enums::ImageLayout layout);

        VkAttachmentLoadOp UnWrapLoadOp(Core::Enums::LoadOperation op);

        VkAttachmentStoreOp UnWrapStoreOp(Core::Enums::StoreOperation op);

        VkAttachmentReference UnWrapAttachmentRef(Core::Wrappers::AttachmentRef ref);

        VkSampleCountFlagBits UnWrapSampleCount(Core::Enums::Samples sampleCount);
        Core::Enums::Samples UnWrapSampleCount(VkSampleCountFlags vkSampleCount);
        VkImageType UnWrapImageType(Core::Enums::ImageType type);
        VkImageUsageFlagBits UnwrapUsage(Core::Enums::ImageUsage usage);
        VkImageUsageFlags UnwrapUsage(const std::vector<Core::Enums::ImageUsage>& usage);

        VkRenderPassBeginInfo UnwrapRenderPassBeginInfo(Core::Wrappers::RenderPassBeginInfo info);

        VkShaderStageFlags UnwrapShaderStage(Core::Enums::ShaderType type);
        VkShaderStageFlags UnwrapShaderStage(Core::Enums::ShaderType* type, const uint32_t& count);

        VkImageViewCreateInfo UnWrapImageViewCreateInfo(const Core::Wrappers::ImageViewCreateInfo* info, const VkImage& image);
        VkImageCreateInfo UnWrapImageCreateInfo(const Core::Wrappers::ImageCreateInfo* info);
        VkComponentSwizzle UnWrapSwizzle(Core::Enums::ComponentSwizzle obj);
        VkImageViewType UnwrapImageViewType(Core::Enums::ImageViewType type);
        VkImageAspectFlags UnwrapAspectMask(Core::Enums::ImageAspectFlag flag);
        VkImageAspectFlags UnwrapAspectMask(const std::vector<Core::Enums::ImageAspectFlag>& flags);
        VkMemoryRequirements UnwrapMemoryRequirements(Core::Wrappers::MemoryRequirementInfo* info);
        VkMemoryPropertyFlags UnwrapMemoryProperty(const Core::Enums::MemoryType* memType);
        VkPipelineStageFlags const UnwrapPipelineStageFlags(const Core::Enums::PipelineStage* stages, const uint32_t& count);
        VkPipelineStageFlags2 const UnwrapPipelineStageFlags2(const Core::Enums::PipelineStageFlagBits2* stages, const uint32_t& count);
        VkAccessFlags const UnwrapAccessFlags(const Core::Enums::AccessFlagBits* stages, const uint32_t& count);
        VkAccessFlags2 const UnwrapAccessFlags2(const Core::Enums::PipelineAccessFlagBits2* stages, const uint32_t& count);
        VkDependencyFlags const UnwrapDependencyFlags(const Core::Enums::DependencyFlagBits* stages, const uint32_t& count);
        VkBufferCreateInfo* const UnwrapBufferCreateInfo(const Core::Wrappers::BufferCreateInfo* info, const uint32_t& count);
        VkBufferUsageFlags const UnwrapBufferUsage(const std::vector<Core::Enums::BufferUsage>& usage);
        VkMemoryPropertyFlags UnwrapMemoryProperty(const Core::Enums::MemoryType* memType, const uint32_t& count);

        VkSamplerCreateInfo const UnwrapSamplerCreateInfo(const Core::Wrappers::SamplerCreateInfo& info);
        VkBorderColor const UnwrapBorderColor(const Core::Enums::BorderColor& color);
        VkCompareOp const UnwrapCompareOp(const Core::Enums::CompareOp& op);
        VkFilter const UnwrapFilter(const Core::Enums::Filter& filter);
        VkSamplerMipmapMode const UnwrapMipMapMode(const Core::Enums::SamplerMipmapMode& mode);
        VkSamplerAddressMode const UnwrapAddressMode(const Core::Enums::SamplerAddressMode& mode);
    }
}
#endif RenderingUnwrapper_H
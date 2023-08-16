#include "VkRenderingUnwrapper.h"
#include "VkFrameBufferFactory.h"
#include "VkRenderPassFactory.h"

VkColorSpaceKHR VulkanUnwrap::UnWrapColorSpace(ColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    default:
        ASSERT_MSG_DEBUG(0, "colorspace converter not found ");
        std::exit(-1);
    }
}
/*
VkImageViewType  VulkanUnwrap::UnWrapImageViewDegree(Dimensions degree)
{
    switch (degree)
    {
    case Dimensions::Dim1:
        return VK_IMAGE_VIEW_TYPE_1D;
        break;
    case Dimensions::Dim2:
        return VK_IMAGE_VIEW_TYPE_2D;
        break;
    default:
        ASSERT_MSG_DEBUG(0, "Converter not found ");
        std::exit(-1);
    }
}

VkImageType  VulkanUnwrap::UnWrapImageDegree(Dimensions degree)
{
    switch (degree)
    {
    case Dimensions::Dim1:
        return VK_IMAGE_TYPE_1D;
        break;
    case Dimensions::Dim2:
        return VK_IMAGE_TYPE_2D;
        break;
    default:
        ASSERT_MSG_DEBUG(0, "Converter not found ");
        std::exit(-1);
    }
}
*/
VkFormat VulkanUnwrap::UnWrapFormat(Format format)
{
    switch (format)
    {
    case Format::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
        break;

    case Format::B8G8R8A8_UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
        break;

    case Format::D16_UNORM:
        return VK_FORMAT_D16_UNORM;
        break;

    case Format::D16_UNORM_S8_UINT:
        return VK_FORMAT_D16_UNORM_S8_UINT;
        break;

    case Format::D24_UNORM_S8_UINT:
        return VK_FORMAT_D24_UNORM_S8_UINT;
        break;

    case Format::D32_SFLOAT:
        return VK_FORMAT_D32_SFLOAT;
        break;

    case Format::D32_SFLOAT_S8_UINT:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;

    case Format::R32G32B32A32_SFLOAT:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
        break;

    case Format::R32G32B32_SFLOAT:
        return VK_FORMAT_R32G32B32_SFLOAT;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Converter not found ");
        std::exit(-1);
    }
}

VkImageLayout VulkanUnwrap::UnWrapImageLayout(ImageLayout layout)
{
    switch (layout)
    {
    case    ImageLayout::LAYOUT_UNDEFINED:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    case    ImageLayout::LAYOUT_GENERAL:
        return VK_IMAGE_LAYOUT_GENERAL;
    case    ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case    ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case    ImageLayout::LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case    ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case    ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case    ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case    ImageLayout::LAYOUT_PREINITIALIZED:
        return VK_IMAGE_LAYOUT_PREINITIALIZED;
    case    ImageLayout::LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
    case    ImageLayout::LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
    case    ImageLayout::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    case    ImageLayout::LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
    case    ImageLayout::LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
        return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
    case    ImageLayout::LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
        return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
    case    ImageLayout::LAYOUT_PRESENT_SRC_KHR:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    default:
        ASSERT_MSG_DEBUG(0, "Layout converter not found ");
        std::exit(-1);
    }
}

VkAttachmentLoadOp VulkanUnwrap::UnWrapLoadOp(LoadOperation op)
{
    switch (op)
    {
    case   LoadOperation::LOAD_OP_LOAD:
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    case   LoadOperation::LOAD_OP_CLEAR:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case   LoadOperation::LOAD_OP_DONT_CARE:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    default:
        ASSERT_MSG_DEBUG(0, "loadop converter not found ");
        std::exit(-1);
    }
}

VkAttachmentStoreOp VulkanUnwrap::UnWrapStoreOp(StoreOperation op)
{
    switch (op)
    {
    case   StoreOperation::STORE_OP_STORE:
        return VK_ATTACHMENT_STORE_OP_STORE;
    case   StoreOperation::STORE_OP_DONT_CARE:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default:
        ASSERT_MSG_DEBUG(0, "Store op converter not found ");
        std::exit(-1);
    }
}

VkAttachmentReference VulkanUnwrap::UnWrapAttachmentRef(AttachmentRef ref)
{
    VkAttachmentReference obj = {};
    obj.attachment = ref.index;
    obj.layout = UnWrapImageLayout(ref.layoutInSubPass);

    return obj;
}

VkSampleCountFlagBits VulkanUnwrap::UnWrapSampleCount(Samples sampleCount)
{
    switch (sampleCount)
    {
    case  Samples::SAMPLE_COUNT_1_BIT:
        return VK_SAMPLE_COUNT_1_BIT;

    case  Samples::SAMPLE_COUNT_2_BIT:
        return VK_SAMPLE_COUNT_2_BIT;

    case  Samples::SAMPLE_COUNT_4_BIT:
        return VK_SAMPLE_COUNT_4_BIT;

    case  Samples::SAMPLE_COUNT_8_BIT:
        return VK_SAMPLE_COUNT_8_BIT;

    case  Samples::SAMPLE_COUNT_16_BIT:
        return VK_SAMPLE_COUNT_16_BIT;

    case  Samples::SAMPLE_COUNT_32_BIT:
        return VK_SAMPLE_COUNT_32_BIT;

    case  Samples::SAMPLE_COUNT_64_BIT:
        return VK_SAMPLE_COUNT_64_BIT;

    default:
        ASSERT_MSG_DEBUG(0, "Sample converter not found ");
        std::exit(-1);
    }
}

Samples VulkanUnwrap::UnWrapSampleCount(VkSampleCountFlags vkSampleCount)
{
    switch (vkSampleCount)
    {
    case  VK_SAMPLE_COUNT_1_BIT:
        return Samples::SAMPLE_COUNT_1_BIT;

    case  VK_SAMPLE_COUNT_2_BIT:
        return Samples::SAMPLE_COUNT_2_BIT ;

    case  VK_SAMPLE_COUNT_4_BIT:
        return Samples::SAMPLE_COUNT_4_BIT ;

    case  VK_SAMPLE_COUNT_8_BIT:
        return Samples::SAMPLE_COUNT_8_BIT ;

    case  VK_SAMPLE_COUNT_16_BIT:
        return Samples::SAMPLE_COUNT_16_BIT ;

    case  VK_SAMPLE_COUNT_32_BIT:
        return Samples::SAMPLE_COUNT_32_BIT ;

    case  VK_SAMPLE_COUNT_64_BIT:
        return Samples::SAMPLE_COUNT_64_BIT ;

    default:
        ASSERT_MSG_DEBUG(0, "Sample converter not found ");
        std::exit(-1);
    }

    return Samples();
}

VkImageType VulkanUnwrap::UnWrapImageType(ImageType type)
{
    switch (type)
    {
    case ImageType::IMAGE_TYPE_1D:
        return VkImageType::VK_IMAGE_TYPE_1D;

    case ImageType::IMAGE_TYPE_2D:
        return VkImageType::VK_IMAGE_TYPE_2D;

    case ImageType::IMAGE_TYPE_3D:
        return VkImageType::VK_IMAGE_TYPE_3D;

    default:
        ASSERT_MSG_DEBUG(0, "Sample converter not found ");
        std::exit(-1);

    }
    return VkImageType::VK_IMAGE_TYPE_1D;
}

VkImageUsageFlagBits VulkanUnwrap::UnwrapUsage(AttachmentUsage usage)
{
    switch (usage)
    {
    case AttachmentUsage::USAGE_TRANSFER_DST_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    case AttachmentUsage::USAGE_SAMPLED_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;

    case AttachmentUsage::USAGE_STORAGE_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;

    case AttachmentUsage::USAGE_COLOR_ATTACHMENT_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    case AttachmentUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    case AttachmentUsage::USAGE_TRANSIENT_ATTACHMENT_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

    case AttachmentUsage::USAGE_INPUT_ATTACHMENT_BIT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    case AttachmentUsage::USAGE_SHADING_RATE_IMAGE_BIT_NV:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV;

    case AttachmentUsage::USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT:
        return VkImageUsageFlagBits::VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    default:
        ASSERT_MSG_DEBUG(0, "Usage converter not found ");
        std::exit(-1);
    }
}

VkImageUsageFlags VulkanUnwrap::UnwrapUsage(std::vector<AttachmentUsage>& usage)
{
    VkImageUsageFlags flag = 0;
    for (uint32_t i = 0; i < usage.size(); i++)
    {
        flag |= UnwrapUsage(usage[i]);
    }

    return flag;
}

VkRenderPassBeginInfo VulkanUnwrap::UnwrapRenderPassBeginInfo(RenderPassBeginInfo beginInfo)
{
    VkRenderPassBeginInfo vkBeginInfo = {};
    vkBeginInfo.framebuffer = *VkFrameBufferFactory::GetInstance()->GetFrameBuffer(beginInfo.frameBufferId);
    vkBeginInfo.renderArea.extent.width = (uint32_t)beginInfo.renderAreaExtent[0];
    vkBeginInfo.renderArea.extent.height = (uint32_t)beginInfo.renderAreaExtent[1];
    vkBeginInfo.renderArea.offset.x = (int32_t)beginInfo.renderAreaPosition[0];
    vkBeginInfo.renderArea.offset.y = (int32_t)beginInfo.renderAreaPosition[1];
    vkBeginInfo.renderPass = *VkRenderPassFactory::GetInstance()->GetRenderPass(beginInfo.renderPassId);
    vkBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    
    /*if (beginInfo.clearColorValue.size() == 0)
    {
        vkBeginInfo.pClearValues = VkRenderPassFactory::GetInstance()->GetClearValue(beginInfo.renderPassId);
        vkBeginInfo.clearValueCount = VkRenderPassFactory::GetInstance()->GetClearValueCount(beginInfo.renderPassId);
    }
    else*/
    {
        // color attachments + 1 depth
        const uint32_t numClearVals = (uint32_t)beginInfo.clearColorValue.size() + 1;
        VkClearValue * clearVal = new VkClearValue[numClearVals];

        for (uint32_t i = 0; i < numClearVals - 1; i++)
        {
            clearVal[i].color.float32[0] = beginInfo.clearColorValue[i].at(0);
            clearVal[i].color.float32[1] = beginInfo.clearColorValue[i].at(1);
            clearVal[i].color.float32[2] = beginInfo.clearColorValue[i].at(2);
            clearVal[i].color.float32[3] = beginInfo.clearColorValue[i].at(3);
        }

        clearVal[numClearVals - 1].depthStencil.depth = beginInfo.depthClearValue;
        clearVal[numClearVals - 1].depthStencil.stencil = (uint32_t)beginInfo.stencilClearValue;

        vkBeginInfo.pClearValues = clearVal;
        vkBeginInfo.clearValueCount = numClearVals;
    }

    return vkBeginInfo;
}

VkShaderStageFlags VulkanUnwrap::UnwrapShaderStage(ShaderType type)
{
    VkShaderStageFlags shaderFlag;

    switch (type)
    {
    case    ShaderType::VERTEX:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        break;
    
    case    ShaderType::TESSELLATION:
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
        break;

    case    ShaderType::GEOMETRY:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        break;

    case    ShaderType::FRAGMENT:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;

    case    ShaderType::COMPUTE:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    }

    return shaderFlag;
}

VkShaderStageFlags VulkanUnwrap::UnwrapShaderStage(ShaderType * type, const uint32_t & count)
{
    VkShaderStageFlags shaderFlag = 0;
    //shaderflag needs to be filled with something to be OR-ED with subsequent stages
    /*switch (type[0])
    {
    case  ShaderType::VERTEX:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        break;

    case    ShaderType::TESSELLATION:
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
        break;

    case    ShaderType::GEOMETRY:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        break;

    case    ShaderType::FRAGMENT:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;

    case    ShaderType::COMPUTE:
        shaderFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    }
*/

    for(uint32_t i = 0; i < count; i++)
    switch (type[i])
    {
    case    ShaderType::VERTEX:
        shaderFlag |= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        break;

    case    ShaderType::TESSELLATION:
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
        break;

    case    ShaderType::GEOMETRY:
        shaderFlag |= VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        break;

    case    ShaderType::FRAGMENT:
        shaderFlag |= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;

    case    ShaderType::COMPUTE:
        shaderFlag |= VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    }

    return shaderFlag;
}

VkImageViewCreateInfo VulkanUnwrap::UnWrapImageViewCreateInfo(ImageViewInfo * info)
{
    VkImageViewCreateInfo vkInfo = {};
    vkInfo.components.a = UnWrapSwizzle(info->components[0]);
    vkInfo.components.b = UnWrapSwizzle(info->components[1]);
    vkInfo.components.g = UnWrapSwizzle(info->components[2]);
    vkInfo.components.r = UnWrapSwizzle(info->components[3]);
    vkInfo.format = UnWrapFormat(info->format) ;
    vkInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vkInfo.viewType = UnwrapImageViewType(info->viewType);
    vkInfo.subresourceRange.aspectMask = UnwrapAspectMask(info->imageAspect);
    vkInfo.subresourceRange.baseArrayLayer = info->baseArrayLayer;
    vkInfo.subresourceRange.baseMipLevel = info->baseMipLevel;
    vkInfo.subresourceRange.layerCount = info->layerCount;
    vkInfo.subresourceRange.levelCount = info->levelCount;

    return vkInfo;
}

VkImageCreateInfo VulkanUnwrap::UnWrapImageCreateInfo(ImageInfo * info)
{
    VkImageCreateInfo vkInfo = {};
    vkInfo.arrayLayers = info->layers;
    vkInfo.extent.width = info->width;
    vkInfo.extent.height = info->height;
    vkInfo.extent.depth = info->depth;
    vkInfo.format = UnWrapFormat(info->format);
    vkInfo.imageType = UnWrapImageType(info->imageType) ;
    vkInfo.initialLayout = UnWrapImageLayout(info->initialLayout);
    vkInfo.mipLevels = info->mips;
    vkInfo.pQueueFamilyIndices = nullptr ;
    vkInfo.queueFamilyIndexCount = 0;
    vkInfo.samples = UnWrapSampleCount(info->sampleCount);
    vkInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    vkInfo.usage = UnwrapUsage(info->usage);

    return vkInfo;
}

VkComponentSwizzle VulkanUnwrap::UnWrapSwizzle(ComponentSwizzle obj)
{
    switch (obj)
    {
    case ComponentSwizzle::COMPONENT_SWIZZLE_A:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;

    case ComponentSwizzle::COMPONENT_SWIZZLE_B:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;

    case ComponentSwizzle::COMPONENT_SWIZZLE_G:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;

    case ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

    case ComponentSwizzle::COMPONENT_SWIZZLE_ONE:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ONE;

    case ComponentSwizzle::COMPONENT_SWIZZLE_R:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;

    case ComponentSwizzle::COMPONENT_SWIZZLE_ZERO:
        return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
    
    default: ASSERT_MSG(0, "invalid option");
    }

    return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
}

VkImageViewType VulkanUnwrap::UnwrapImageViewType(ImageViewType type)
{
    switch (type)
    {
    
    case ImageViewType::IMAGE_VIEW_TYPE_1D:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;

    case ImageViewType::IMAGE_VIEW_TYPE_1D_ARRAY:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;

    case ImageViewType::IMAGE_VIEW_TYPE_2D:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

    case ImageViewType::IMAGE_VIEW_TYPE_2D_ARRAY:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

    case ImageViewType::IMAGE_VIEW_TYPE_3D:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;

    case ImageViewType::IMAGE_VIEW_TYPE_CUBE:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;

    case ImageViewType::IMAGE_VIEW_TYPE_CUBE_ARRAY:
        return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

    default: ASSERT_MSG(0, "invalid option");

    }

    return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
}

VkImageAspectFlags VulkanUnwrap::UnwrapAspectMask(ImageAspectFlag flag)
{
    switch (flag)
    {
    case ImageAspectFlag::IMAGE_ASPECT_COLOR_BIT:
        return VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

    case ImageAspectFlag::IMAGE_ASPECT_DEPTH_BIT:
        return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;

    case ImageAspectFlag::IMAGE_ASPECT_STENCIL_BIT:
        return VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;

    default: ASSERT_MSG(0, "Case not implemented");

    }
    return VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
}

VkMemoryRequirements VulkanUnwrap::UnwrapMemoryRequirements(MemoryRequirementInfo * info)
{
    VkMemoryRequirements req = {};
    req.alignment = info->alignment;
    req.memoryTypeBits = info->memoryTypeBits;
    req.size = info->size;

    return req;
}

VkMemoryPropertyFlags VulkanUnwrap::UnwrapMemoryProperty(const MemoryType * memType)
{
    VkMemoryPropertyFlags memProp;

    switch (*memType)
    {
    case MemoryType::DEVICE_LOCAL_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;

    case MemoryType::HOST_VISIBLE_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        break;

    case MemoryType::HOST_COHERENT_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;

    case MemoryType::HOST_CACHED_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;

    case MemoryType::LAZILY_ALLOCATED_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
        break;

    case MemoryType::PROTECTED_BIT:
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
        break;
    }

    return memProp;
}

VkPipelineStageFlags const VulkanUnwrap::UnwrapPipelineStageFlags(const PipelineStage * stages, const uint32_t & count)
{
    VkPipelineStageFlags flags = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        switch (stages[i])
        {
            case PipelineStage::ALL_COMMANDS_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                break;

            case PipelineStage::ALL_GRAPHICS_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
                break;

            case PipelineStage::BOTTOM_OF_PIPE_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                break;

            case PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;

            case PipelineStage::COMPUTE_SHADER_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;

            case PipelineStage::DRAW_INDIRECT_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                break;

            case PipelineStage::EARLY_FRAGMENT_TESTS_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                break;

            case PipelineStage::FRAGMENT_SHADER_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;

            case PipelineStage::GEOMETRY_SHADER_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
                break;

            case PipelineStage::HOST_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT;
                break;

            case PipelineStage::LATE_FRAGMENT_TESTS_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;

            case PipelineStage::TOP_OF_PIPE_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;

            case PipelineStage::VERTEX_INPUT_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                break;

            case PipelineStage::VERTEX_SHADER_BIT:
                flags |= VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                break;

            default: ASSERT_MSG(0, "Type yet to be handled");
        }
    }

    return flags;
}

VkAccessFlags const VulkanUnwrap::UnwrapAccessFlags(const AccessFlagBits * stages, const uint32_t & count)
{
    VkAccessFlags flags = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        switch (stages[i])
        {
        case AccessFlagBits::ACCESS_COLOR_ATTACHMENT_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_COLOR_ATTACHMENT_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_HOST_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_HOST_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_HOST_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_INDEX_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_INDEX_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_INPUT_ATTACHMENT_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_MEMORY_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_MEMORY_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_MEMORY_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_SHADER_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_SHADER_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_TRANSFER_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case AccessFlagBits::ACCESS_TRANSFER_WRITE_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case AccessFlagBits::ACCESS_UNIFORM_READ_BIT:
            flags |= VkAccessFlagBits::VK_ACCESS_UNIFORM_READ_BIT;
            break;

        default: ASSERT_MSG(0, "Type not handled");
        }
    }

    return flags;
}

VkDependencyFlags const VulkanUnwrap::UnwrapDependencyFlags(const DependencyFlagBits * stages, const uint32_t & count)
{
    VkDependencyFlags flags = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        switch (stages[i])
        {
        case DependencyFlagBits::DEPENDENCY_BY_REGION_BIT :
            flags |= VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;
            break;

        case DependencyFlagBits::DEPENDENCY_DEVICE_GROUP_BIT:
            flags |= VkDependencyFlagBits::VK_DEPENDENCY_DEVICE_GROUP_BIT;
            break;

        case DependencyFlagBits::DEPENDENCY_VIEW_LOCAL_BIT:
            flags |= VkDependencyFlagBits::VK_DEPENDENCY_VIEW_LOCAL_BIT;
            break;

        default: ASSERT_MSG(0, "Type not handled");
        }
    }

    return flags;
}

VkBufferCreateInfo * const VulkanUnwrap::UnwrapBufferCreateInfo(const BufferCreateInfo * info, const uint32_t & count)
{
    VkBufferCreateInfo * vkInfo = new VkBufferCreateInfo[count];

    for (uint32_t i = 0; i < count; i++)
    {
        vkInfo[i].flags = 0;
        vkInfo[i].pNext = nullptr;
        vkInfo[i].pQueueFamilyIndices = nullptr;
        vkInfo[i].queueFamilyIndexCount = 0;
        vkInfo[i].sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        vkInfo[i].size = info[i].size;
        vkInfo[i].sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkInfo[i].usage = UnwrapBufferUsage(info[i].usage);
    }

    return vkInfo;
}

VkBufferUsageFlags const VulkanUnwrap::UnwrapBufferUsage(const std::vector<BufferUsage>& usage)
{
    VkBufferUsageFlags vkUsage = 0;

    for each(auto obj in usage)
    {
        switch (obj)
        {
        case BufferUsage::BUFFER_USAGE_INDEX_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_INDIRECT_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_TRANSFER_DST_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_TRANSFER_SRC_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            break;

        case BufferUsage::BUFFER_USAGE_VERTEX_BUFFER_BIT:
            vkUsage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;

        default:
            ASSERT_MSG(0, "invalid buffer usage");
        }
    }

    return vkUsage;
}

VkMemoryPropertyFlags VulkanUnwrap::UnwrapMemoryProperty(const MemoryType * memType, const uint32_t & count)
{
    VkMemoryPropertyFlags memProp = 0;

    for (uint32_t i = 0; i < count; i++)
        switch (memType[i])
        {
        case MemoryType::DEVICE_LOCAL_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;

        case MemoryType::HOST_VISIBLE_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            break;

        case MemoryType::HOST_COHERENT_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;

        case MemoryType::HOST_CACHED_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;

        case MemoryType::LAZILY_ALLOCATED_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
            break;

        case MemoryType::PROTECTED_BIT:
            memProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
            break;
        }

    return memProp;
}

VkSamplerCreateInfo const VulkanUnwrap::UnwrapSamplerCreateInfo(const SamplerCreateInfo & info)
{
    VkSamplerCreateInfo vkInfo = {};

    vkInfo.addressModeU = UnwrapAddressMode(info.addressModeU);
    vkInfo.addressModeV = UnwrapAddressMode(info.addressModeV);
    vkInfo.addressModeW = UnwrapAddressMode(info.addressModeW);
    vkInfo.anisotropyEnable = info.anisotropyEnable;
    vkInfo.borderColor = UnwrapBorderColor(info.borderColor);
    vkInfo.compareEnable = info.compareEnable;
    vkInfo.compareOp = UnwrapCompareOp(info.compareOp);
    vkInfo.magFilter = UnwrapFilter(info.magFilter);
    vkInfo.maxAnisotropy = info.maxAnisotropy;
    vkInfo.maxLod = info.maxLod;
    vkInfo.minFilter = UnwrapFilter(info.minFilter);
    vkInfo.minLod = info.minLod;
    vkInfo.mipLodBias = info.mipLodBias;
    vkInfo.mipmapMode = UnwrapMipMapMode(info.mipmapMode);
    vkInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    vkInfo.unnormalizedCoordinates = info.unnormalizedCoordinates;

    return vkInfo;
}

VkBorderColor const VulkanUnwrap::UnwrapBorderColor(const BorderColor & color)
{
    VkBorderColor vkColor;
    switch (color)
    {
    case BorderColor::BORDER_COLOR_FLOAT_OPAQUE_BLACK :
        vkColor = VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        break;

    case BorderColor::BORDER_COLOR_FLOAT_OPAQUE_WHITE:
        vkColor = VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        break;

    case BorderColor::BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
        vkColor = VkBorderColor::VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        break;

    case BorderColor::BORDER_COLOR_INT_OPAQUE_BLACK:
        vkColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        break;

    case BorderColor::BORDER_COLOR_INT_OPAQUE_WHITE:
        vkColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        break;

    case BorderColor::BORDER_COLOR_INT_TRANSPARENT_BLACK:
        vkColor = VkBorderColor::VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        break;

    default:
        ASSERT_MSG(0, "Invalid option");

    }

    return vkColor;
}

VkCompareOp const VulkanUnwrap::UnwrapCompareOp(const CompareOp  & op)
{
    VkCompareOp vkOp;
    switch (op)
    {
    case CompareOp::COMPARE_OP_ALWAYS:
        vkOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        break;

    case CompareOp::COMPARE_OP_EQUAL:
        vkOp = VkCompareOp::VK_COMPARE_OP_EQUAL;
        break;

    case CompareOp::COMPARE_OP_GREATER:
        vkOp = VkCompareOp::VK_COMPARE_OP_GREATER;
        break;

    case CompareOp::COMPARE_OP_GREATER_OR_EQUAL:
        vkOp = VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
        break;

    case CompareOp::COMPARE_OP_LESS:
        vkOp = VkCompareOp::VK_COMPARE_OP_LESS;
        break;

    case CompareOp::COMPARE_OP_LESS_OR_EQUAL:
        vkOp = VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
        break;

    case CompareOp::COMPARE_OP_MAX_ENUM:
        vkOp = VkCompareOp::VK_COMPARE_OP_MAX_ENUM;
        break;

    case CompareOp::COMPARE_OP_NEVER:
        vkOp = VkCompareOp::VK_COMPARE_OP_NEVER;
        break;

    case CompareOp::COMPARE_OP_NOT_EQUAL:
        vkOp = VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
        break;

    default: ASSERT_MSG_DEBUG(0, "Mode not found");

    }
    return vkOp;
}

VkFilter const VulkanUnwrap::UnwrapFilter(const Filter & filter)
{
    VkFilter vkFilter;
    switch (filter)
    {
    case Filter::FILTER_LINEAR:
        vkFilter = VkFilter::VK_FILTER_LINEAR;
        break;

    case Filter::FILTER_NEAREST:
        vkFilter = VkFilter::VK_FILTER_NEAREST;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "invalide option");
    }
    return vkFilter;
}

VkSamplerMipmapMode const VulkanUnwrap::UnwrapMipMapMode(const SamplerMipmapMode & mode)
{
    VkSamplerMipmapMode vkMode;

    switch(mode)
    {
    case SamplerMipmapMode::SAMPLER_MIPMAP_MODE_LINEAR:
        vkMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;

    case SamplerMipmapMode::SAMPLER_MIPMAP_MODE_NEAREST:
        vkMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "invalide option");
    }

    return vkMode;
}

VkSamplerAddressMode const VulkanUnwrap::UnwrapAddressMode(const SamplerAddressMode & mode)
{
    VkSamplerAddressMode vkMode;

    switch (mode)
    {
    case SamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
        vkMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;

    case SamplerAddressMode::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
        vkMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;

    case SamplerAddressMode::SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
        vkMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;

    case SamplerAddressMode::SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
        vkMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        break;

    case SamplerAddressMode::SAMPLER_ADDRESS_MODE_REPEAT:
        vkMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;

    default :
        ASSERT_MSG_DEBUG(0, "invalide option");
    }
    return vkMode;
}


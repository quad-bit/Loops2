#include "VulkanInterface.h"
#include <VulkanManager.h>
#include <windowing/WindowManager.h>
#include <PresentationEngine.h>
#include <VkAttachmentFactory.h>
#include <VkRenderPassFactory.h>
#include <VkFrameBufferFactory.h>
#include <utility/VkRenderingUnwrapper.h>
#include <VkCommandBufferFactory.h>
#include <synchronisation/VkSynchroniserFactory.h>
#include <VkQueueFactory.h>
#include <utility/VulkanUtility.h>
#include <RendererSettings.h>
#include <shading/VkBufferFactory.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <pipeline/VulkanGraphicsPipelineFactory.h>
#include <shading/VkShaderFactory.h>
#include <CorePrecompiled.h>
#include <shading/VkShaderResourceManager.h>
#include <utility/VulkanMemoryManager.h>
#include <VkSamplerFactory.h>
#include <utility/VkRenderingUnwrapper.h>

namespace
{
    VkAttachmentDescription* UnwrapAttachmentDesc(const Core::Wrappers::RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t& attachmentCount)
    {
        VkAttachmentDescription* attachmentDescriptions = new VkAttachmentDescription[attachmentCount];

        for (uint32_t i = 0; i < attachmentCount; i++)
        {
            const Core::Wrappers::RenderPassAttachmentInfo obj = renderpassAttachmentList[i];
            attachmentDescriptions[i].initialLayout = GfxVk::Unwrap::UnWrapImageLayout(obj.initialLayout);
            attachmentDescriptions[i].finalLayout = GfxVk::Unwrap::UnWrapImageLayout(obj.finalLayout);
            attachmentDescriptions[i].format = GfxVk::Unwrap::UnWrapFormat(obj.format);
            attachmentDescriptions[i].loadOp = GfxVk::Unwrap::UnWrapLoadOp(obj.loadOp);
            attachmentDescriptions[i].storeOp = GfxVk::Unwrap::UnWrapStoreOp(obj.storeOp);
            attachmentDescriptions[i].stencilLoadOp = GfxVk::Unwrap::UnWrapLoadOp(obj.stencilLoadOp);
            attachmentDescriptions[i].stencilStoreOp = GfxVk::Unwrap::UnWrapStoreOp(obj.stencilLStoreOp);
            attachmentDescriptions[i].samples = GfxVk::Unwrap::UnWrapSampleCount(obj.sampleCount);
            attachmentDescriptions[i].flags = 0;
        }

        return attachmentDescriptions;
    }

    VkSubpassDescription* UnwrapSubpassDesc(const Core::Wrappers::SubpassInfo* subpassList, const uint32_t& subpassCount)
    {
        VkSubpassDescription* subpassDescriptions = new VkSubpassDescription[subpassCount];

        //for each (SubpassInfo* obj in subpassList)
        for (uint32_t i = 0; i < subpassCount; i++)
        {
            const Core::Wrappers::SubpassInfo* obj = subpassList + i;

            //Color
            if (obj->colorAttachmentCount > 0)
            {
                subpassDescriptions[i].colorAttachmentCount = obj->colorAttachmentCount;
                VkAttachmentReference* refCol = new VkAttachmentReference[obj->colorAttachmentCount];
                for (uint32_t j = 0; j < subpassDescriptions[i].colorAttachmentCount; j++)
                {
                    Core::Wrappers::AttachmentRef* aref = obj->pColorAttachments + j;
                    refCol[i] = GfxVk::Unwrap::UnWrapAttachmentRef(*aref);
                    //refs.push_back(ref);
                }
                subpassDescriptions[i].pColorAttachments = refCol;
                //refCounter += obj->colorAttachmentCount;
            }
            else
            {
                subpassDescriptions[i].pColorAttachments = nullptr;
                subpassDescriptions[i].colorAttachmentCount = 0;
            }

            //Input
            if (obj->inputAttachmentCount > 0)
            {
                subpassDescriptions[i].inputAttachmentCount = obj->inputAttachmentCount;
                VkAttachmentReference* refInp = new VkAttachmentReference[obj->inputAttachmentCount];

                for (uint32_t j = 0; j < subpassDescriptions[i].inputAttachmentCount; j++)
                {
                    Core::Wrappers::AttachmentRef* aref = obj->pInputAttachments + j;
                    refInp[i] = GfxVk::Unwrap::UnWrapAttachmentRef(*aref);
                }
                subpassDescriptions[i].pInputAttachments = refInp;
                //refCounter += obj->inputAttachmentCount;
            }
            else
            {
                subpassDescriptions[i].pInputAttachments = nullptr;
                subpassDescriptions[i].inputAttachmentCount = 0;
            }

            //Depth
            if (obj->pDepthStencilAttachment != nullptr)
            {
                VkAttachmentReference* refDepth = new VkAttachmentReference;
                *refDepth = GfxVk::Unwrap::UnWrapAttachmentRef(*obj->pDepthStencilAttachment);

                subpassDescriptions[i].pDepthStencilAttachment = refDepth;
            }
            else
            {
                subpassDescriptions[i].pDepthStencilAttachment = nullptr;
            }

            //resolve, 
            VkAttachmentReference* refResolve;

            if (obj->pResolveAttachments != nullptr)
            {
                refResolve = new VkAttachmentReference;
                *refResolve = GfxVk::Unwrap::UnWrapAttachmentRef(*obj->pResolveAttachments);
            }
            else
            {
                refResolve = nullptr;
            }

            subpassDescriptions[i].pResolveAttachments = refResolve;
            subpassDescriptions[i].preserveAttachmentCount = 0;
            subpassDescriptions[i].pPreserveAttachments = nullptr;

            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescriptions[i].flags = 0;
        }

        return subpassDescriptions;
    }

    VkSubpassDependency* UnwrapSubpassDependency(const Core::Wrappers::SubpassDependency* dependencyList, const uint32_t& dependencyCount)
    {
        VkSubpassDependency* dependencies = new VkSubpassDependency[dependencyCount];

        for (uint32_t i = 0; i < dependencyCount; i++)
        {
            if (dependencyList[i].srcSubpass == -1)
                dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
            else
                dependencies[i].srcSubpass = dependencyList[i].srcSubpass;

            if (dependencyList[i].dstSubpass == -1)
                dependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
            else
                dependencies[i].dstSubpass = dependencyList[i].dstSubpass;

            dependencies[i].srcStageMask = GfxVk::Unwrap::UnwrapPipelineStageFlags(dependencyList[i].srcStageMask.data(), (uint32_t)dependencyList[i].srcStageMask.size());
            dependencies[i].dstStageMask = GfxVk::Unwrap::UnwrapPipelineStageFlags(dependencyList[i].dstStageMask.data(), (uint32_t)dependencyList[i].dstStageMask.size());

            if (dependencyList[i].srcAccessMask.size() > 0)
                dependencies[i].srcAccessMask = GfxVk::Unwrap::UnwrapAccessFlags(dependencyList[i].srcAccessMask.data(), (uint32_t)dependencyList[i].srcAccessMask.size());
            else
                dependencies[i].srcAccessMask = {};

            if (dependencyList[i].dstAccessMask.size() > 0)
                dependencies[i].dstAccessMask = GfxVk::Unwrap::UnwrapAccessFlags(dependencyList[i].dstAccessMask.data(), (uint32_t)dependencyList[i].dstAccessMask.size());
            else
                dependencies[i].dstAccessMask = {};

            dependencies[i].dependencyFlags = GfxVk::Unwrap::UnwrapDependencyFlags(dependencyList[i].dependencyFlags.data(), (uint32_t)dependencyList[i].dependencyFlags.size());
        }

        return dependencies;
    }

    VkCommandBufferUsageFlagBits UnwrapCommandBufferUsage(const Core::Enums::CommandBufferUsage* info)
    {
        switch (*info)
        {
        case Core::Enums::CommandBufferUsage::RENDER_PASS_CONTINUE_BIT:
            return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            break;

        case Core::Enums::CommandBufferUsage::SIMULTANEOUS_USE_BIT:
            return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            break;

        case Core::Enums::CommandBufferUsage::USAGE_ONE_TIME_SUBMIT_BIT:
            return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            break;

        default:
            ASSERT_MSG_DEBUG(0, "No usage found.");
        }

        return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }

    VkPipelineStageFlags* UnwrapStageFlags(const Core::Enums::PipelineStage* pipelineStage)
    {
        VkPipelineStageFlags* flags = new VkPipelineStageFlags;

        switch (*pipelineStage)
        {
        case Core::Enums::PipelineStage::TOP_OF_PIPE_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;

        case Core::Enums::PipelineStage::ACCELERATION_STRUCTURE_BUILD_BIT_KHR:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            break;

        case Core::Enums::PipelineStage::ALL_COMMANDS_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            break;

        case Core::Enums::PipelineStage::ALL_GRAPHICS_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            break;

        case Core::Enums::PipelineStage::BOTTOM_OF_PIPE_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            break;

        case Core::Enums::PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;

        case Core::Enums::PipelineStage::COMMAND_PREPROCESS_BIT_NV:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
            break;

        case Core::Enums::PipelineStage::COMPUTE_SHADER_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            break;

        case Core::Enums::PipelineStage::CONDITIONAL_RENDERING_BIT_EXT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
            break;

        case Core::Enums::PipelineStage::DRAW_INDIRECT_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            break;

        case Core::Enums::PipelineStage::EARLY_FRAGMENT_TESTS_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;

        case Core::Enums::PipelineStage::FLAG_BITS_MAX_ENUM:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
            break;

        case Core::Enums::PipelineStage::FRAGMENT_DENSITY_PROCESS_BIT_EXT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
            break;

        case Core::Enums::PipelineStage::FRAGMENT_SHADER_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;

        case Core::Enums::PipelineStage::GEOMETRY_SHADER_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
            break;

        case Core::Enums::PipelineStage::HOST_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT;
            break;

        case Core::Enums::PipelineStage::LATE_FRAGMENT_TESTS_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;

        case Core::Enums::PipelineStage::TRANSFER_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;

        case Core::Enums::PipelineStage::VERTEX_INPUT_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            break;

        case Core::Enums::PipelineStage::VERTEX_SHADER_BIT:
            *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            break;
        }

        return flags;
    }

    VkSubmitInfo* UnwrapSubmitInfo(const Core::Wrappers::SubmitInfo* info)
    {
        VkSubmitInfo* vkSubmitInfo = new VkSubmitInfo;
        vkSubmitInfo->waitSemaphoreCount = info->waitSemaphoreCount;
        vkSubmitInfo->pWaitSemaphores = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetSemaphore(info->waitSemaphoreIds, info->waitSemaphoreCount);
        vkSubmitInfo->signalSemaphoreCount = info->signalSemaphoreCount;
        vkSubmitInfo->pSignalSemaphores = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetSemaphore(info->signalSemaphoreIds, info->signalSemaphoreCount);
        vkSubmitInfo->commandBufferCount = info->commandBufferCount;
        vkSubmitInfo->pCommandBuffers = GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->GetCommandBuffer(info->commandBufferIds, info->commandBufferCount);
        vkSubmitInfo->pWaitDstStageMask = UnwrapStageFlags(&info->pipelineStage);
        vkSubmitInfo->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkSubmitInfo->pNext = nullptr;

        return vkSubmitInfo;
    }

    
    VkMemoryPropertyFlags UnwrapMemoryProperty(const Core::Enums::MemoryType* memType)
    {
        VkMemoryPropertyFlags memProp;

        switch (*memType)
        {
        case Core::Enums::MemoryType::DEVICE_LOCAL_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;

        case Core::Enums::MemoryType::HOST_VISIBLE_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            break;

        case Core::Enums::MemoryType::HOST_COHERENT_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;

        case Core::Enums::MemoryType::HOST_CACHED_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;

        case Core::Enums::MemoryType::LAZILY_ALLOCATED_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
            break;

        case Core::Enums::MemoryType::PROTECTED_BIT:
            memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
            break;
        }

        return memProp;
    }



    VkBufferUsageFlags UnwrapBufferUsageFlags(const Core::Enums::BufferType* type)
    {
        VkBufferUsageFlags bufferUsage;

        switch (*type)
        {
        case Core::Enums::BufferType::TRANSFER_SRC_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            break;

        case Core::Enums::BufferType::TRANSFER_DST_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            break;

        case Core::Enums::BufferType::UNIFORM_TEXEL_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::STORAGE_TEXEL_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::UNIFORM_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::STORAGE_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::INDEX_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::VERTEX_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;

        case Core::Enums::BufferType::INDIRECT_BUFFER_BIT:
            bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            break;
        }

        return bufferUsage;
    }

    Core::Enums::ColorSpace WrapColorSpace(VkColorSpaceKHR space)
    {
        switch (space)
        {
        case VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
        default:
            ASSERT_MSG_DEBUG(0, "colorspace converter not found ");
            std::exit(-1);
        }
        return Core::Enums::ColorSpace();
    }

    Core::Enums::Format WrapFormat(VkFormat format)
    {
        switch (format)
        {
        case VkFormat::VK_FORMAT_UNDEFINED:
            return Core::Enums::Format::UNDEFINED;
            break;

        case VkFormat::VK_FORMAT_B8G8R8A8_UNORM:
            return Core::Enums::Format::B8G8R8A8_UNORM;
            break;

        case VkFormat::VK_FORMAT_D16_UNORM:
            return Core::Enums::Format::D16_UNORM;
            break;

        case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT:
            return Core::Enums::Format::D16_UNORM_S8_UINT;
            break;

        case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
            return Core::Enums::Format::D24_UNORM_S8_UINT;
            break;

        case VkFormat::VK_FORMAT_D32_SFLOAT:
            return Core::Enums::Format::D32_SFLOAT;
            break;

        case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
            return Core::Enums::Format::D32_SFLOAT_S8_UINT;
            break;

        case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
            return Core::Enums::Format::R32G32B32A32_SFLOAT;
            break;

        case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
            return Core::Enums::Format::R32G32B32_SFLOAT;
            break;

        default:
            ASSERT_MSG_DEBUG(0, "Converter not found ");
            std::exit(-1);
        }

        return Core::Enums::Format();
    }
}

Core::Enums::Format Renderer::Utility::VulkanInterface::GetWindowSurfaceFormat()
{
    //VkSurfaceFormatKHR * vkFormat = GfxVk::Utility::VulkanManager::GetInstance()->GetSurfaceFormat();
    return WrapFormat(GfxVk::Utility::VulkanDeviceInfo::GetSurfaceFormat().format);
}

Core::Enums::ColorSpace Renderer::Utility::VulkanInterface::GetWindowColorSpace()
{

    //VkSurfaceFormatKHR * vkFormat = GfxVk::Utility::VulkanManager::GetInstance()->GetSurfaceFormat();
    return WrapColorSpace(GfxVk::Utility::VulkanDeviceInfo::GetSurfaceFormat().colorSpace);
}

int Renderer::Utility::VulkanInterface::FindBestDepthFormat(Core::Enums::Format * imageFormat, const uint32_t & count)
{
    return GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->FindBestDepthFormat(imageFormat, count);
}

void Renderer::Utility::VulkanInterface::SetupPresentationEngine(Core::Wrappers::ImageInfo info)
{
    GfxVk::Utility::PresentationEngine::GetInstance()->CreateSwapChain(info);
}

void Renderer::Utility::VulkanInterface::CreateRenderTarget(Core::Wrappers::ImageInfo * info, Core::Wrappers::ImageViewInfo * viewInfo, uint32_t & count, bool defaultTarget,
    uint32_t * ids)
{
    //VkAttachmentFactory::GetInstance()->CreateColorAttachment(info, count, defaultTarget, ids);
}

void Renderer::Utility::VulkanInterface::CreateDefaultRenderTarget(Core::Wrappers::ImageInfo info, Core::Wrappers::ImageViewInfo viewInfo, uint32_t & count, uint32_t * ids)
{
    VkImageCreateInfo vkImageCreateInfo = GfxVk::Unwrap::UnWrapImageCreateInfo(&info);
    VkImageViewCreateInfo vkImageViewCreateInfo = GfxVk::Unwrap::UnWrapImageViewCreateInfo(&viewInfo);

    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->CreateSwapChainImage(vkImageCreateInfo, vkImageViewCreateInfo, count, ids);
}

void Renderer::Utility::VulkanInterface::DestroyRenderTarget(std::vector<uint32_t>* ids, bool defaultTarget)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->DestroyAttachment(*ids, defaultTarget);
}

void Renderer::Utility::VulkanInterface::CreateAttachment(Core::Wrappers::ImageInfo * info, const uint32_t & count, uint32_t * ids)
{
    /*VkImageCreateInfo * imageCreateInfo = new VkImageCreateInfo[count];
    VkImageViewCreateInfo * viewCreateInfo = new VkImageViewCreateInfo[count];

    for (uint32_t i = 0; i < count; i++)
    {
        imageCreateInfo[i] = GfxVk::Unwrap::UnWrapImageCreateInfo(info);
        viewCreateInfo[i]  = GfxVk::Unwrap::UnWrapImageViewCreateInfo(viewInfo);
    }

    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->CreateDepthAttachment(imageCreateInfo, count, viewCreateInfo,
        stencilRequired, ids);

    delete[] imageCreateInfo;
    delete[] viewCreateInfo;
*/
    std::vector<VkImageCreateInfo> vkInfo;
    for(uint32_t i = 0 ;i < count;i++)
       vkInfo.push_back(GfxVk::Unwrap::UnWrapImageCreateInfo(info));

   GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->CreateAttachment(vkInfo.data(), count, ids);
}

void Renderer::Utility::VulkanInterface::DestroyDepthTarget(std::vector<uint32_t>* ids, bool defaultTarget)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->DestroyAttachment(*ids, defaultTarget);
}

void Renderer::Utility::VulkanInterface::DestroyAttachment(uint32_t * ids, bool * destroyImageView, bool * freeImageMemory, const uint32_t & count)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->DestroyAttachment(ids, destroyImageView, freeImageMemory, count);
}

void Renderer::Utility::VulkanInterface::FreeAttachmentMemory(uint32_t * imageIds, const uint32_t & count)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->FreeAttachmentMemory(imageIds, count);
}

void Renderer::Utility::VulkanInterface::DestroySwapChainImageViews(uint32_t * ids, const uint32_t & count)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->DestroySwapChainImageViews(ids, count);
}

void Renderer::Utility::VulkanInterface::CreateImageView(Core::Wrappers::ImageViewInfo * viewInfo, uint32_t & count)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->CreateImageView(viewInfo, count);
}

void Renderer::Utility::VulkanInterface::CreateRenderPass(
    const Core::Wrappers::RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t & attachmentCount,
    const Core::Wrappers::SubpassInfo* subpassList, const uint32_t & subpassCount,
    const Core::Wrappers::SubpassDependency* dependencyList, const uint32_t & dependencyCount,
    uint32_t& renderPassId)
{
    /*VkRenderPassFactory::GetInstance()->CreateRenderPass(renderpassAttachmentList, attachmentCount,
        subpassList, subpassCount, dependencyList, dependencyCount, renderPassId
    );*/

    VkAttachmentDescription* attachmentDescList = UnwrapAttachmentDesc(renderpassAttachmentList, attachmentCount);
    VkSubpassDescription* subpassDescList = UnwrapSubpassDesc(subpassList, subpassCount);
    VkSubpassDependency* subpassDependencyList = UnwrapSubpassDependency(dependencyList, dependencyCount);

    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->CreateRenderPass(attachmentDescList, attachmentCount,
        subpassDescList, subpassCount, subpassDependencyList, dependencyCount, renderPassId);

    if (attachmentDescList != nullptr)
        delete[] attachmentDescList;

    if (subpassDescList->pColorAttachments != nullptr)
        delete[] subpassDescList->pColorAttachments;

    if (subpassDescList->pDepthStencilAttachment != nullptr)
        delete subpassDescList->pDepthStencilAttachment;

    if (subpassDescList->pInputAttachments != nullptr)
        delete[] subpassDescList->pInputAttachments;

    if (subpassDescList->pResolveAttachments != nullptr)
        delete subpassDescList->pResolveAttachments;

    if (subpassDescList->pPreserveAttachments != nullptr)
        delete[] subpassDescList->pPreserveAttachments;

    if (subpassDescList != nullptr)
        delete[] subpassDescList;
}

void Renderer::Utility::VulkanInterface::DestroyRenderPass(const uint32_t & id)
{
    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->DestroyRenderPass(id);
}

void Renderer::Utility::VulkanInterface::CreateFrameBuffer(uint32_t numFrameBuffers, uint32_t * imageViewId, const uint32_t & viewsPerFB,
    uint32_t renderPassId, const uint32_t & width, const uint32_t & height, uint32_t * ids)
{
    VkRenderPass * renderPass = GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->GetRenderPass(renderPassId);

    std::vector<VkImageView> viewList(numFrameBuffers * viewsPerFB);// = new VkImageView[numFrameBuffers * viewsPerFB];
    for (uint32_t i = 0; i < numFrameBuffers * viewsPerFB; i++)
    {
        viewList[i]= GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->GetImageView(imageViewId[i]);
    }

    GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance()->CreateFrameBuffer(numFrameBuffers, viewList.data(),
        viewsPerFB, renderPass, width, height, ids);
        
    //delete [] viewList;
}

void Renderer::Utility::VulkanInterface::DestroyFrameBuffer(uint32_t * pid, const uint32_t & count)
{
    GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance()->DestroyFrameBuffer(pid, count);
}

void Renderer::Utility::VulkanInterface::DestroyCommandBuffer(const uint32_t & id)
{
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DestroyCommandBuffer(id);
}

void Renderer::Utility::VulkanInterface::ResetCommandBuffer(const uint32_t & id, const uint32_t & poolId)
{
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->ResetCommandBuffer(id, poolId);
}

void Renderer::Utility::VulkanInterface::BeginCommandBufferRecording(const uint32_t & id,
    const Core::Enums::CommandBufferUsage * usage, const Core::Wrappers::CommandBufferInheritanceInfo * inheritanceInfo)
{
    VkCommandBufferUsageFlagBits vkUsage = UnwrapCommandBufferUsage(usage);
    VkCommandBufferInheritanceInfo * vkInheritanceInfo = nullptr;

    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->BeginCommandBufferRecording(id, vkUsage, vkInheritanceInfo);

    if(vkInheritanceInfo != nullptr)
        delete vkInheritanceInfo;
}

void Renderer::Utility::VulkanInterface::EndCommandBufferRecording(const uint32_t & id)
{
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->EndCommandBufferRecording(id);
}

void Renderer::Utility::VulkanInterface::SubmitJob(const Core::Wrappers::QueueWrapper * queueWrapper, const Core::Wrappers::SubmitInfo * info, const uint32_t & submitInfoCount, const uint32_t & fenceId)
{
    VkSubmitInfo * vkSubmitInfo = UnwrapSubmitInfo(info);
    VkFence * fence = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetFence(fenceId);

    GfxVk::Utility::VkQueueFactory::GetInstance()->SubmitQueue(queueWrapper, vkSubmitInfo, submitInfoCount, fence);

    delete vkSubmitInfo->pWaitDstStageMask;
    delete vkSubmitInfo;
}

void Renderer::Utility::VulkanInterface::SubmitJob(const Core::Wrappers::SubmitInfo * info, const uint32_t & submitInfoCount)
{
    VkSubmitInfo * vkSubmitInfo = UnwrapSubmitInfo(info);
    GfxVk::Utility::VkQueueFactory::GetInstance()->SubmitQueue(info->queueId, info->queueType, vkSubmitInfo, submitInfoCount, VK_NULL_HANDLE);
    
    if(vkSubmitInfo->pWaitDstStageMask != nullptr)
        delete vkSubmitInfo->pWaitDstStageMask;

    delete vkSubmitInfo;
}

void Renderer::Utility::VulkanInterface::PresentSwapchainImage(const Core::Wrappers::QueueWrapper * queueWrapper, const Core::Wrappers::PresentInfo * info, const uint32_t & presentQueueId)
{
    VkResult presentationResult;

    VkPresentInfoKHR vkPresentInfo = {};
    vkPresentInfo.pImageIndices = info->pImageIndices;
    //vkPresentInfo.swapchainCount = 1; // completed in PresentationFactory
    //vkPresentInfo.pSwapchains = PresentationEngine::GetInstance()->GetSwapchain(); // completed in PresentationFactory
    vkPresentInfo.waitSemaphoreCount = info->waitSemaphoreCount;
    vkPresentInfo.pWaitSemaphores = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetSemaphore(info->pWaitSemaphoreIds, info->waitSemaphoreCount);
    vkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    vkPresentInfo.pResults = &presentationResult;

    //TODO : Expose presentation queue
    //VkQueue * presentQueue = VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::presentationQueuedId);
    VkQueue * presentQueue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(queueWrapper);
    GfxVk::Utility::PresentationEngine::GetInstance()->PresentSwapchainImage(&vkPresentInfo, presentQueue);
}

bool Renderer::Utility::VulkanInterface::IsApplicationSafeForClosure()
{
    GfxVk::Utility::VkQueueFactory::GetInstance()->WaitForAllQueues();
    vkDeviceWaitIdle(DeviceInfo::GetLogicalDevice());

    return true;
}

/*
uint32_t * CreateBuffer(BufferInfo * info, const uint32_t & count)
{
    VkBufferUsageFlags * bufferUsage = new VkBufferUsageFlags[count]; 
    VkMemoryPropertyFlags * memProp = new VkMemoryPropertyFlags[count];

    uint32_t * ids = new uint32_t[count];

    for (uint32_t i = 0; i < count; i++)
    {
        bufferUsage[i] = UnwrapBufferUsageFlags(info->bufType);
        for (uint32_t k = 0; k < info->memTypeCount; k++)
        {
            if(k == 0)
                memProp[i] = UnwrapMemoryProperty(info->memType);
            else
                memProp[i] |= UnwrapMemoryProperty(info->memType);
        }

        ids[i] = *VkBufferFactory::GetInstance()->CreateBuffer(&bufferUsage[i], &memProp[i], info->data, info->dataSize, info->pGpuMem);
    }
    
    delete[] memProp;
    return ids;
}
*/

//deprecated
uint32_t * Renderer::Utility::VulkanInterface::CreateBuffers(Core::Wrappers::BufferInfo * info, const uint32_t & count)
{
    VkBufferUsageFlags * bufferUsage = new VkBufferUsageFlags[count];
    VkMemoryPropertyFlags * memProp = new VkMemoryPropertyFlags[count];
    size_t * dataSizes = new size_t[count];
    uint32_t * ids;

    for (uint32_t i = 0; i < count; i++)
    {
        dataSizes[i] = info[i].dataSize;
        bufferUsage[i] = UnwrapBufferUsageFlags(info[i].bufType);
        for (uint32_t k = 0; k < info[i].memTypeCount; k++)
        {
            if (k == 0)
                memProp[i] = UnwrapMemoryProperty(info[i].memType);
            else
                memProp[i] |= UnwrapMemoryProperty(info[i].memType);
        }
    }

    ids = GfxVk::Shading::VkBufferFactory::GetInstance()->CreateBuffers(count, bufferUsage, memProp, dataSizes);

    delete[] bufferUsage;
    delete[] memProp;
    delete[] dataSizes;
    return ids;
}

void Renderer::Utility::VulkanInterface::CreateBuffers(Core::Wrappers::BufferCreateInfo * info, const uint32_t & count, uint32_t * out_buffIds, size_t * out_bufferMemRequirements)
{
    GfxVk::Shading::VkBufferFactory::GetInstance()->CreateBuffers(count, info, out_buffIds, out_bufferMemRequirements);
}

size_t Renderer::Utility::VulkanInterface::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return GfxVk::Shading::VkBufferFactory::GetInstance()->GetMemoryAlignedDataSizeForBuffer(dataSize);
}

uint32_t * Renderer::Utility::VulkanInterface::AllocateBufferMemory(uint32_t * bufferId, const uint32_t & bufCount)
{
    return GfxVk::Shading::VkBufferFactory::GetInstance()->AllocateBufferMemory(bufferId, bufCount);
}

void Renderer::Utility::VulkanInterface::CopyBufferDataToMemory(const uint32_t & bufId, const VkDeviceSize & dataSize, const VkDeviceSize & memAlignedSize, void * data, VkDeviceSize memoryOffset, bool keepMemoryMounted)
{
    GfxVk::Shading::VkBufferFactory::GetInstance()->CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

void Renderer::Utility::VulkanInterface::DestroyBuffer(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        GfxVk::Shading::VkBufferFactory::GetInstance()->DestroyBuffer(ids[i]);
    }
}

void Renderer::Utility::VulkanInterface::FreeMemory(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory(ids[i]);
    }
}

uint32_t * Renderer::Utility::VulkanInterface::AllocateDescriptorsSet(Core::Wrappers::SetWrapper * set, const uint32_t & numDescriptors)
{
    return GfxVk::Shading::VkShaderResourceManager::GetInstance()->AllocateDescriptorSets(set, numDescriptors);
}

void Renderer::Utility::VulkanInterface::BindImageMemory(const uint32_t & imageId, const uint32_t & memId, const size_t & offset)
{
    GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->BindImageMemory(imageId, memId, offset);
}

uint32_t Renderer::Utility::VulkanInterface::AllocateMemory(Core::Wrappers::MemoryRequirementInfo * memReq, Core::Enums::MemoryType * userReq, const size_t & allocationSize)
{
    VkMemoryRequirements req = GfxVk::Unwrap::UnwrapMemoryRequirements(memReq);
    VkMemoryPropertyFlags flags = GfxVk::Unwrap::UnwrapMemoryProperty(userReq);

    uint32_t memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, flags, allocationSize);

    return memId;
}

uint32_t Renderer::Utility::VulkanInterface::AllocateMemory(Core::Wrappers::MemoryRequirementInfo * memReq, Core::Enums::MemoryType * userReq, const uint32_t & numUserReq, const size_t & allocationSize)
{
    VkMemoryRequirements req = GfxVk::Unwrap::UnwrapMemoryRequirements(memReq);
    VkMemoryPropertyFlags flags = GfxVk::Unwrap::UnwrapMemoryProperty(userReq, numUserReq);

    uint32_t memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, flags, allocationSize);

    return memId;
}

Core::Wrappers::MemoryRequirementInfo Renderer::Utility::VulkanInterface::GetImageMemoryRequirement(const uint32_t & imageId)
{
    Core::Wrappers::MemoryRequirementInfo req = GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()->GetImageMemoryRequirement(imageId);
    return req;
}

//
//void InitiateGraphicsPipelineCreation(const uint32_t & meshId, VertexInputAttributeInfo * attribInfo, const uint32_t & attribCount, VertexInputBindingInfo * bindingInfo, const uint32_t & bindingCount )
//{
//    VulkanGraphicsPipelineFactory::GetInstance()->InitiatePipelineCreation(meshId, attribInfo, attribCount, bindingInfo, bindingCount);
//}
//
//void SetInputAssemblyInfo(const uint32_t & meshId, PrimtiveType * primitive, bool isPrimitiveRestartEnabled)
//{
//    VulkanGraphicsPipelineFactory::GetInstance()->SetInputAssemblyInfo(meshId, primitive, isPrimitiveRestartEnabled);
//}

void Renderer::Utility::VulkanInterface::GetShaderIds(char ** shaderName, Core::Enums::ShaderType * type, uint32_t * id, const uint32_t & shaderCount)
{
    GfxVk::Shading::VkShaderFactory::GetInstance()->GetShaderIds(shaderName, type, id, shaderCount);
}

void Renderer::Utility::VulkanInterface::CreateVertexInputState(const Core::Utility::VertexInputWrapper * vertexInputWrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateVertexInputState(vertexInputWrapper);
}

void Renderer::Utility::VulkanInterface::CreateInputAssemblyState(const Core::Utility::InputAssemblyWrapper * inputAssemblyWrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateInputAssemblyState(inputAssemblyWrapper);
}

void Renderer::Utility::VulkanInterface::CreateShaderState(const Core::Utility::ShaderStateWrapper * shaderStateWrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateShaderState(shaderStateWrapper);
}

void Renderer::Utility::VulkanInterface::CreateTessellationState(const Core::Utility::TessellationStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateTessellationState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateViewportState(const Core::Utility::ViewPortStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateViewportState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateRasterisationState(const Core::Utility::RasterizationStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateRasterisationState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateDepthStencilState(const Core::Utility::DepthStencilStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateDepthStencilState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateColorBlendState(const Core::Utility::ColorBlendStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateColorBlendState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateMultiSampleState(const Core::Utility::MultiSampleStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateMultiSampleState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreateDynamicState(const Core::Utility::DynamicStateWrapper * wrapper)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreateDynamicState(wrapper);
}

void Renderer::Utility::VulkanInterface::CreatePipeline(Core::Wrappers::PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineId)
{
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->CreatePipeline(info, pipelineCount, pipelineId);
}

std::vector<Core::Wrappers::SetWrapper*> Renderer::Utility::VulkanInterface::GetSetsForShaders(const std::vector<std::string>& shaderNames)
{
    return GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetSetsForShaders(shaderNames);
}

uint32_t Renderer::Utility::VulkanInterface::CreatePipelineLayout(Core::Wrappers::SetWrapper ** setWrapperList, const size_t & numSets)
{
    return GfxVk::Shading::VkShaderResourceManager::GetInstance()->CreatePipelineLayout(setWrapperList, numSets);
}

std::vector<Core::Wrappers::SetWrapper*>* Renderer::Utility::VulkanInterface::GetSetWrapperList()
{
    return GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetSetWrapperList();
}

std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>>* Renderer::Utility::VulkanInterface::GetPerSetSetwrapperList()
{
    return GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPerSetSetwrapperMap();
}

void Renderer::Utility::VulkanInterface::LinkSetBindingToResources(Core::Utility::DescriptorSetInfo desc, const uint32_t & numDescriptorSets)
{
    GfxVk::Shading::VkShaderResourceManager::GetInstance()->LinkSetBindingToResources(desc, numDescriptorSets);
}

bool Renderer::Utility::VulkanInterface::IsSampleRateShadingAvailable()
{
    ASSERT_MSG_DEBUG(0, "CHECK THIS");
    return GfxVk::Utility::VulkanDeviceInfo::IsSampleRateShadingAvailable();
}

Core::Enums::Samples Renderer::Utility::VulkanInterface::GetMaxUsableSampleCount()
{
    ASSERT_MSG_DEBUG(0, "CHECK THIS");
    return GfxVk::Unwrap::UnWrapSampleCount(GfxVk::Utility::VulkanDeviceInfo::GetMaxSampleCount());
}

uint32_t Renderer::Utility::VulkanInterface::CreateSampler(const Core::Wrappers::SamplerCreateInfo & info)
{
    return GfxVk::Shading::VkSamplerFactory::GetInstance()->CreateSampler(info);
}

uint32_t Renderer::Utility::VulkanInterface::CreateCommandPool(Core::Enums::PipelineType * pipelineType, Core::Enums::CommandPoolProperty * prop)
{
    uint32_t poolId = 0;

    VkQueueFlagBits queueFlag;
    switch (*pipelineType)
    {
    case Core::Enums::PipelineType::COMPUTE :
        queueFlag = VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
        break;

    case Core::Enums::PipelineType::GRAPHICS:
        queueFlag = VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT;
        break;

    case Core::Enums::PipelineType::TRANSFER:
        queueFlag = VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue/pipeline type not found");
    }

    VkCommandPoolCreateFlagBits poolCreateFlag;
    switch (*prop)
    {
    case Core::Enums::CommandPoolProperty::RESET_COMMAND_BUFFER_BIT:
        poolCreateFlag = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ;
        break;

    case Core::Enums::CommandPoolProperty::TRANSIENT_BIT:
        poolCreateFlag = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        break;

    case Core::Enums::CommandPoolProperty::TRANS_RESET:
        poolCreateFlag = (VkCommandPoolCreateFlagBits)(VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue/pipeline type not found");
    }

    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->CreateCommandPool(poolCreateFlag, queueFlag, poolId);

    return poolId;
}

void Renderer::Utility::VulkanInterface::DestroyCommandPool(const uint32_t & poolId)
{
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DestroyCommandPool(poolId);
}

uint32_t Renderer::Utility::VulkanInterface::CreateFence(bool isSignaled)
{
    return GfxVk::Sync::VkSynchroniserFactory::GetInstance()->CreateFence(isSignaled);
}

uint32_t Renderer::Utility::VulkanInterface::Create_Semaphore(bool isSignaled)
{
    return GfxVk::Sync::VkSynchroniserFactory::GetInstance()->Create_Semaphore(isSignaled);
}

void Renderer::Utility::VulkanInterface::DestroyFence(const uint32_t & id)
{
    return GfxVk::Sync::VkSynchroniserFactory::GetInstance()->DestroyFence(id);
}

void Renderer::Utility::VulkanInterface::DestroySemaphore(const uint32_t & id)
{
    return GfxVk::Sync::VkSynchroniserFactory::GetInstance()->DestroySemaphore(id);
}

void Renderer::Utility::VulkanInterface::SetRenderpassBeginInfo(Core::Wrappers::RenderPassBeginInfo * beginInfo, const uint32_t & renderPassId)
{
    VkRenderPassBeginInfo info = GfxVk::Unwrap::UnwrapRenderPassBeginInfo(*beginInfo);
    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->SetRenderPassBeginInfo(info, renderPassId);
}

uint32_t Renderer::Utility::VulkanInterface::GetAvailableSwapchainIndex(const uint32_t & fenceId, const uint32_t & semaphoreId)
{
    VkFence * fence = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetFence(fenceId);
    VkSemaphore * semaphore = GfxVk::Sync::VkSynchroniserFactory::GetInstance()->GetSemaphore(semaphoreId);

    return GfxVk::Utility::PresentationEngine::GetInstance()->VkGetAvailableSwapChainId(fence, semaphore);
}

void Renderer::Utility::VulkanInterface::ActivateCommandBuffer(const uint32_t & index)
{
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->ActivateCommandBuffer(index);
}

GfxVk::CommandPool::VkDrawCommandBuffer * Renderer::Utility::VulkanInterface::CreateCommandBuffer(const uint32_t & poolId, uint32_t * cmdBufferId, Core::Enums::CommandBufferLevel* commandBufferLevel, Core::Enums::PipelineType bufferType)
{
    VkCommandBufferLevel level;
    switch (*commandBufferLevel)
    {
    case Core::Enums::CommandBufferLevel::PRIMARY:
        level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        break;

    case Core::Enums::CommandBufferLevel::SECONDARY:
        level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Wrong command level");
    }
    return GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->CreateCommandBuffer(poolId, level, bufferType, cmdBufferId);
}

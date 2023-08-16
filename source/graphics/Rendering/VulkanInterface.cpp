#include "VulkanInterface.h"
#include <VulkanManager.h>
#include <WindowManager.h>
#include <PresentationEngine.h>
#include <VkAttachmentFactory.h>
#include <VkRenderPassFactory.h>
#include <VkFrameBufferFactory.h>
#include <VkRenderingUnwrapper.h>
#include <VkCommandBufferFactory.h>
#include <VkSynchroniserFactory.h>
#include <VkQueueFactory.h>
#include <VulkanUtility.h>
#include <RendererSettings.h>
#include <VkBufferFactory.h>
#include <RenderingWrapper.h>
#include <VulkanGraphicsPipelineFactory.h>
#include <VkShaderFactory.h>
#include <CorePrecompiled.h>
#include <VkShaderResourceManager.h>
#include <VulkanMemoryManager.h>
#include <VkRenderingUnwrapper.h>
#include <VkSamplerFactory.h>

VkAttachmentDescription * VulkanInterface::UnwrapAttachmentDesc(const RenderPassAttachmentInfo * renderpassAttachmentList, const uint32_t & attachmentCount)
{
    VkAttachmentDescription* attachmentDescriptions = new VkAttachmentDescription[attachmentCount];

    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        const RenderPassAttachmentInfo obj = renderpassAttachmentList[i];
        attachmentDescriptions[i].initialLayout = VulkanUnwrap::UnWrapImageLayout(obj.initialLayout);
        attachmentDescriptions[i].finalLayout = VulkanUnwrap::UnWrapImageLayout(obj.finalLayout);
        attachmentDescriptions[i].format = VulkanUnwrap::UnWrapFormat(obj.format);
        attachmentDescriptions[i].loadOp = VulkanUnwrap::UnWrapLoadOp(obj.loadOp);
        attachmentDescriptions[i].storeOp = VulkanUnwrap::UnWrapStoreOp(obj.storeOp);
        attachmentDescriptions[i].stencilLoadOp = VulkanUnwrap::UnWrapLoadOp(obj.stencilLoadOp);
        attachmentDescriptions[i].stencilStoreOp = VulkanUnwrap::UnWrapStoreOp(obj.stencilLStoreOp);
        attachmentDescriptions[i].samples = VulkanUnwrap::UnWrapSampleCount(obj.sampleCount);
        attachmentDescriptions[i].flags = 0;
    }

    return attachmentDescriptions;
}

VkSubpassDescription * VulkanInterface::UnwrapSubpassDesc(const SubpassInfo * subpassList, const uint32_t & subpassCount)
{
    VkSubpassDescription * subpassDescriptions = new VkSubpassDescription[subpassCount];

    //for each (SubpassInfo* obj in subpassList)
    for (uint32_t i = 0; i < subpassCount; i++)
    {
        const SubpassInfo* obj = subpassList + i;

        //Color
        if (obj->colorAttachmentCount > 0)
        {
            subpassDescriptions[i].colorAttachmentCount = obj->colorAttachmentCount;
            VkAttachmentReference* refCol = new VkAttachmentReference[obj->colorAttachmentCount];
            for (uint32_t j = 0; j < subpassDescriptions[i].colorAttachmentCount; j++)
            {
                AttachmentRef * aref = obj->pColorAttachments + j;
                refCol[i] = VulkanUnwrap::UnWrapAttachmentRef(*aref);
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
                AttachmentRef * aref = obj->pInputAttachments + j;
                refInp[i] = VulkanUnwrap::UnWrapAttachmentRef(*aref);
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
            VkAttachmentReference *refDepth = new VkAttachmentReference;
            *refDepth = VulkanUnwrap::UnWrapAttachmentRef(*obj->pDepthStencilAttachment);

            subpassDescriptions[i].pDepthStencilAttachment = refDepth;
        }
        else
        {
            subpassDescriptions[i].pDepthStencilAttachment = nullptr;
        }

        //resolve, 
        VkAttachmentReference *refResolve;
        
        if (obj->pResolveAttachments != nullptr)
        {
            refResolve = new VkAttachmentReference;
            *refResolve = VulkanUnwrap::UnWrapAttachmentRef(*obj->pResolveAttachments);
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

VkSubpassDependency * VulkanInterface::UnwrapSubpassDependency(const SubpassDependency * dependencyList, const uint32_t & dependencyCount)
{
    VkSubpassDependency * dependencies = new VkSubpassDependency[dependencyCount];
    
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
        
        dependencies[i].srcStageMask = VulkanUnwrap::UnwrapPipelineStageFlags(dependencyList[i].srcStageMask.data(), (uint32_t)dependencyList[i].srcStageMask.size());
        dependencies[i].dstStageMask = VulkanUnwrap::UnwrapPipelineStageFlags(dependencyList[i].dstStageMask.data(), (uint32_t)dependencyList[i].dstStageMask.size());
        
        if (dependencyList[i].srcAccessMask.size() > 0)
            dependencies[i].srcAccessMask = VulkanUnwrap::UnwrapAccessFlags(dependencyList[i].srcAccessMask.data(), (uint32_t)dependencyList[i].srcAccessMask.size());
        else
            dependencies[i].srcAccessMask = {};

        if (dependencyList[i].dstAccessMask.size() > 0)
            dependencies[i].dstAccessMask = VulkanUnwrap::UnwrapAccessFlags(dependencyList[i].dstAccessMask.data(), (uint32_t)dependencyList[i].dstAccessMask.size());
        else
            dependencies[i].dstAccessMask = {};

        dependencies[i].dependencyFlags = VulkanUnwrap::UnwrapDependencyFlags(dependencyList[i].dependencyFlags.data(), (uint32_t)dependencyList[i].dependencyFlags.size());
    }
    
    return dependencies;
}

VkCommandBufferUsageFlagBits VulkanInterface::UnwrapCommandBufferUsage(const CommandBufferUsage * info)
{
    switch (*info)
    {
    case CommandBufferUsage::RENDER_PASS_CONTINUE_BIT :
        return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        break;

    case CommandBufferUsage::SIMULTANEOUS_USE_BIT:
        return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        break;

    case CommandBufferUsage::USAGE_ONE_TIME_SUBMIT_BIT:
        return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "No usage found.");
    }

    return VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
}

VkSubmitInfo * VulkanInterface::UnwrapSubmitInfo(const SubmitInfo * info)
{
    VkSubmitInfo * vkSubmitInfo = new VkSubmitInfo;
    vkSubmitInfo->waitSemaphoreCount = info->waitSemaphoreCount;
    vkSubmitInfo->pWaitSemaphores = VkSynchroniserFactory::GetInstance()->GetSemaphore(info->waitSemaphoreIds, info->waitSemaphoreCount);
    vkSubmitInfo->signalSemaphoreCount = info->signalSemaphoreCount;
    vkSubmitInfo->pSignalSemaphores = VkSynchroniserFactory::GetInstance()->GetSemaphore(info->signalSemaphoreIds, info->signalSemaphoreCount);
    vkSubmitInfo->commandBufferCount = info->commandBufferCount;
    vkSubmitInfo->pCommandBuffers = VkCommandBufferFactory::GetInstance()->GetCommandBuffer(info->commandBufferIds, info->commandBufferCount);
    vkSubmitInfo->pWaitDstStageMask = UnwrapStageFlags(&info->pipelineStage);
    vkSubmitInfo->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vkSubmitInfo->pNext = nullptr;

    return vkSubmitInfo;
}

VkPipelineStageFlags * VulkanInterface::UnwrapStageFlags(const PipelineStage * pipelineStage)
{
    VkPipelineStageFlags * flags = new VkPipelineStageFlags;

    switch (*pipelineStage)
    {
    case PipelineStage::TOP_OF_PIPE_BIT :
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        break;

    case PipelineStage::ACCELERATION_STRUCTURE_BUILD_BIT_KHR:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
        break;

    case PipelineStage::ALL_COMMANDS_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        break;

    case PipelineStage::ALL_GRAPHICS_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        break;

    case PipelineStage::BOTTOM_OF_PIPE_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        break;

    case PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;

    case PipelineStage::COMMAND_PREPROCESS_BIT_NV:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
        break;

    case PipelineStage::COMPUTE_SHADER_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        break;

    case PipelineStage::CONDITIONAL_RENDERING_BIT_EXT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
        break;

    case PipelineStage::DRAW_INDIRECT_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        break;

    case PipelineStage::EARLY_FRAGMENT_TESTS_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        break;

    case PipelineStage::FLAG_BITS_MAX_ENUM:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        break;

    case PipelineStage::FRAGMENT_DENSITY_PROCESS_BIT_EXT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
        break;

    case PipelineStage::FRAGMENT_SHADER_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;

    case PipelineStage::GEOMETRY_SHADER_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        break;

    case PipelineStage::HOST_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_HOST_BIT;
        break;

    case PipelineStage::LATE_FRAGMENT_TESTS_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        break;

    case PipelineStage::TRANSFER_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;

    case PipelineStage::VERTEX_INPUT_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        break;

    case PipelineStage::VERTEX_SHADER_BIT:
        *flags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        break;
    }

    return flags;
}

VkMemoryPropertyFlags VulkanInterface::UnwrapMemoryProperty(const MemoryType * memType)
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

    case MemoryType::HOST_CACHED_BIT :
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;

    case MemoryType::LAZILY_ALLOCATED_BIT :
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
        break;

    case MemoryType::PROTECTED_BIT :
        memProp = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_PROTECTED_BIT;
        break;
    }

    return memProp;
}



VkBufferUsageFlags VulkanInterface::UnwrapBufferUsageFlags(const BufferType * type)
{
    VkBufferUsageFlags bufferUsage;

    switch (*type)
    {
    case BufferType::TRANSFER_SRC_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        break;

    case BufferType::TRANSFER_DST_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;

    case BufferType::UNIFORM_TEXEL_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        break;

    case BufferType::STORAGE_TEXEL_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        break;

    case BufferType::UNIFORM_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;

    case BufferType::STORAGE_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;

    case BufferType::INDEX_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;

    case BufferType::VERTEX_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;

    case BufferType::INDIRECT_BUFFER_BIT:
        bufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        break;
    }

    return bufferUsage;
}

ColorSpace VulkanInterface::WrapColorSpace(VkColorSpaceKHR space)
{
    switch (space)
    {
    case VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR :
        return ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
    default:
        ASSERT_MSG_DEBUG(0, "colorspace converter not found ");
        std::exit(-1);
    }
    return ColorSpace();
}

Format VulkanInterface::WrapFormat(VkFormat format)
{
    switch (format)
    {
    case VkFormat::VK_FORMAT_UNDEFINED :
        return Format::UNDEFINED;
        break;

    case VkFormat::VK_FORMAT_B8G8R8A8_UNORM :
        return Format::B8G8R8A8_UNORM;
        break;

    case VkFormat::VK_FORMAT_D16_UNORM :
        return Format::D16_UNORM;
        break;

    case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT :
        return Format::D16_UNORM_S8_UINT;
        break;

    case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT :
        return Format::D24_UNORM_S8_UINT;
        break;

    case VkFormat::VK_FORMAT_D32_SFLOAT :
        return Format::D32_SFLOAT;
        break;

    case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT :
        return Format::D32_SFLOAT_S8_UINT;
        break;

    case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT :
        return Format::R32G32B32A32_SFLOAT;
        break;

    case VkFormat::VK_FORMAT_R32G32B32_SFLOAT :
        return Format::R32G32B32_SFLOAT;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Converter not found ");
        std::exit(-1);
    }

    return Format();
}

VulkanInterface::VulkanInterface()
{
}

void VulkanInterface::Init()
{
    PLOGD << "Vulkan interface Init";

    VulkanManager::GetInstance()->Init(RendererSettings::queueReq , RendererSettings::queueRequirementCount );
    VulkanManager::GetInstance()->CreateSurface(WindowManager::GetInstance()->glfwWindow);
    VulkanManager::GetInstance()->InitializeFactories();
    /*VulkanManager::GetInstance()->CreateSurface(WindowManager::GetInstance()->glfwWindow);
    PresentationEngine::GetInstance()->Init(VulkanManager::GetInstance()->GetSurface(), VulkanManager::GetInstance()->GetSurfaceFormat());
    VkAttachmentFactory::GetInstance()->Init();
    VkRenderPassFactory::GetInstance()->Init();
    VkFrameBufferFactory::GetInstance()->Init();
    
    VkCommandBufferFactory::GetInstance()->Init();
    VkSynchroniserFactory::GetInstance()->Init();
    VulkanGraphicsPipelineFactory::GetInstance()->Init();*/


}

void VulkanInterface::DeInit()
{
    /*PLOGD << "Vulkan interface DeInit";

    VulkanGraphicsPipelineFactory::GetInstance()->DeInit();
    delete VulkanGraphicsPipelineFactory::GetInstance();

    VkSynchroniserFactory::GetInstance()->DeInit();
    delete VkSynchroniserFactory::GetInstance();

    VkCommandBufferFactory::GetInstance()->DeInit();
    delete VkCommandBufferFactory::GetInstance();

    VkFrameBufferFactory::GetInstance()->DeInit();
    delete VkFrameBufferFactory::GetInstance();

    VkRenderPassFactory::GetInstance()->DeInit();
    delete VkRenderPassFactory::GetInstance();

    VkAttachmentFactory::GetInstance()->DeInit();
    delete VkAttachmentFactory::GetInstance();

    PresentationEngine::GetInstance()->DeInit();
    delete PresentationEngine::GetInstance();
*/
    VulkanManager::GetInstance()->DeInit();
    delete VulkanManager::GetInstance();
}

Format VulkanInterface::GetWindowSurfaceFormat()
{
    VkSurfaceFormatKHR * vkFormat = VulkanManager::GetInstance()->GetSurfaceFormat();
    Format format = WrapFormat(vkFormat->format);
    return format;
}

ColorSpace VulkanInterface::GetWindowColorSpace()
{
    VkSurfaceFormatKHR * vkFormat = VulkanManager::GetInstance()->GetSurfaceFormat();
    return WrapColorSpace(vkFormat->colorSpace);
}

int VulkanInterface::FindBestDepthFormat(Format * imageFormat, const uint32_t & count)
{
    return VkAttachmentFactory::GetInstance()->FindBestDepthFormat(imageFormat, count);
}

void VulkanInterface::SetupPresentationEngine(ImageInfo info)
{
    PresentationEngine::GetInstance()->CreateSwapChain(info);
}

void VulkanInterface::CreateRenderTarget(ImageInfo * info, ImageViewInfo * viewInfo, uint32_t & count, bool defaultTarget,
    uint32_t * ids)
{
    //VkAttachmentFactory::GetInstance()->CreateColorAttachment(info, count, defaultTarget, ids);
}

void VulkanInterface::CreateDefaultRenderTarget(ImageInfo info, ImageViewInfo viewInfo, uint32_t & count, uint32_t * ids)
{
    VkImageCreateInfo vkImageCreateInfo = VulkanUnwrap::UnWrapImageCreateInfo(&info);
    VkImageViewCreateInfo vkImageViewCreateInfo = VulkanUnwrap::UnWrapImageViewCreateInfo(&viewInfo);

    VkAttachmentFactory::GetInstance()->CreateSwapChainImage(vkImageCreateInfo, vkImageViewCreateInfo, count, ids);
}

void VulkanInterface::DestroyRenderTarget(std::vector<uint32_t>* ids, bool defaultTarget)
{
    VkAttachmentFactory::GetInstance()->DestroyAttachment(*ids, defaultTarget);
}

void VulkanInterface::CreateAttachment(ImageInfo * info, const uint32_t & count, uint32_t * ids)
{
    /*VkImageCreateInfo * imageCreateInfo = new VkImageCreateInfo[count];
    VkImageViewCreateInfo * viewCreateInfo = new VkImageViewCreateInfo[count];

    for (uint32_t i = 0; i < count; i++)
    {
        imageCreateInfo[i] = VulkanUnwrap::UnWrapImageCreateInfo(info);
        viewCreateInfo[i]  = VulkanUnwrap::UnWrapImageViewCreateInfo(viewInfo);
    }

    VkAttachmentFactory::GetInstance()->CreateDepthAttachment(imageCreateInfo, count, viewCreateInfo,
        stencilRequired, ids);

    delete[] imageCreateInfo;
    delete[] viewCreateInfo;
*/
    std::vector<VkImageCreateInfo> vkInfo;
    for(uint32_t i = 0 ;i < count;i++)
       vkInfo.push_back(VulkanUnwrap::UnWrapImageCreateInfo(info));

    VkAttachmentFactory::GetInstance()->CreateAttachment(vkInfo.data(), count, ids);
}

void VulkanInterface::DestroyDepthTarget(std::vector<uint32_t>* ids, bool defaultTarget)
{
    VkAttachmentFactory::GetInstance()->DestroyAttachment(*ids, defaultTarget);
}

void VulkanInterface::DestroyAttachment(uint32_t * ids, bool * destroyImageView, bool * freeImageMemory, const uint32_t & count)
{
    VkAttachmentFactory::GetInstance()->DestroyAttachment(ids, destroyImageView, freeImageMemory, count);
}

void VulkanInterface::FreeAttachmentMemory(uint32_t * imageIds, const uint32_t & count)
{
    VkAttachmentFactory::GetInstance()->FreeAttachmentMemory(imageIds, count);
}

void VulkanInterface::DestroySwapChainImageViews(uint32_t * ids, const uint32_t & count)
{
    VkAttachmentFactory::GetInstance()->DestroySwapChainImageViews(ids, count);
}

void VulkanInterface::CreateImageView(ImageViewInfo * viewInfo, uint32_t & count)
{
    VkAttachmentFactory::GetInstance()->CreateImageView(viewInfo, count);
}

void VulkanInterface::CreateRenderPass(
    const RenderPassAttachmentInfo* renderpassAttachmentList, const uint32_t & attachmentCount,
    const SubpassInfo* subpassList, const uint32_t & subpassCount,
    const SubpassDependency* dependencyList, const uint32_t & dependencyCount,
    uint32_t& renderPassId)
{
    /*VkRenderPassFactory::GetInstance()->CreateRenderPass(renderpassAttachmentList, attachmentCount,
        subpassList, subpassCount, dependencyList, dependencyCount, renderPassId
    );*/

    VkAttachmentDescription* attachmentDescList = UnwrapAttachmentDesc(renderpassAttachmentList, attachmentCount);
    VkSubpassDescription* subpassDescList = UnwrapSubpassDesc(subpassList, subpassCount);
    VkSubpassDependency* subpassDependencyList = UnwrapSubpassDependency(dependencyList, dependencyCount);

    VkRenderPassFactory::GetInstance()->CreateRenderPass(attachmentDescList, attachmentCount,
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

void VulkanInterface::DestroyRenderPass(const uint32_t & id)
{
    VkRenderPassFactory::GetInstance()->DestroyRenderPass(id);
}

void VulkanInterface::CreateFrameBuffer(uint32_t numFrameBuffers, uint32_t * imageViewId, const uint32_t & viewsPerFB,
    uint32_t renderPassId, const uint32_t & width, const uint32_t & height, uint32_t * ids)
{
    VkRenderPass * renderPass = VkRenderPassFactory::GetInstance()->GetRenderPass(renderPassId);

    std::vector<VkImageView> viewList(numFrameBuffers * viewsPerFB);// = new VkImageView[numFrameBuffers * viewsPerFB];
    for (uint32_t i = 0; i < numFrameBuffers * viewsPerFB; i++)
    {
        viewList[i]= VkAttachmentFactory::GetInstance()->GetImageView(imageViewId[i]);
    }

    VkFrameBufferFactory::GetInstance()->CreateFrameBuffer(numFrameBuffers, viewList.data(), 
        viewsPerFB, renderPass, width, height, ids);
        
    //delete [] viewList;
}

void VulkanInterface::DestroyFrameBuffer(uint32_t * pid, const uint32_t & count)
{
    VkFrameBufferFactory::GetInstance()->DestroyFrameBuffer(pid, count);
}

void VulkanInterface::DestroyCommandBuffer(const uint32_t & id)
{
    VkCommandBufferFactory::GetInstance()->DestroyCommandBuffer(id);
}

void VulkanInterface::ResetCommandBuffer(const uint32_t & id, const uint32_t & poolId)
{
    VkCommandBufferFactory::GetInstance()->ResetCommandBuffer(id, poolId);
}

void VulkanInterface::BeginCommandBufferRecording(const uint32_t & id, 
    const CommandBufferUsage * usage, const CommandBufferInheritanceInfo * inheritanceInfo)
{
    VkCommandBufferUsageFlagBits vkUsage = UnwrapCommandBufferUsage(usage);
    VkCommandBufferInheritanceInfo * vkInheritanceInfo = nullptr;

    VkCommandBufferFactory::GetInstance()->BeginCommandBufferRecording(id, vkUsage, vkInheritanceInfo);

    if(vkInheritanceInfo != nullptr)
        delete vkInheritanceInfo;
}

void VulkanInterface::EndCommandBufferRecording(const uint32_t & id)
{
    VkCommandBufferFactory::GetInstance()->EndCommandBufferRecording(id);
}

void VulkanInterface::SubmitJob(const QueueWrapper * queueWrapper, const SubmitInfo * info, const uint32_t & submitInfoCount, const uint32_t & fenceId)
{
    VkSubmitInfo * vkSubmitInfo = UnwrapSubmitInfo(info);
    VkFence * fence = VkSynchroniserFactory::GetInstance()->GetFence(fenceId);
    
    VkQueueFactory::GetInstance()->SubmitQueue(queueWrapper, vkSubmitInfo, submitInfoCount, fence);

    delete vkSubmitInfo->pWaitDstStageMask;
    delete vkSubmitInfo;
}

void VulkanInterface::SubmitJob(const SubmitInfo * info, const uint32_t & submitInfoCount)
{
    VkSubmitInfo * vkSubmitInfo = UnwrapSubmitInfo(info);
    VkQueueFactory::GetInstance()->SubmitQueue(info->queueId, info->queueType, vkSubmitInfo, submitInfoCount, VK_NULL_HANDLE);
    
    if(vkSubmitInfo->pWaitDstStageMask != nullptr)
        delete vkSubmitInfo->pWaitDstStageMask;

    delete vkSubmitInfo;
}

void VulkanInterface::PresentSwapchainImage(const QueueWrapper * queueWrapper, const PresentInfo * info, const uint32_t & presentQueueId)
{
    VkResult presentationResult;

    VkPresentInfoKHR vkPresentInfo = {};
    vkPresentInfo.pImageIndices = info->pImageIndices;
    //vkPresentInfo.swapchainCount = 1; // completed in PresentationFactory
    //vkPresentInfo.pSwapchains = PresentationEngine::GetInstance()->GetSwapchain(); // completed in PresentationFactory
    vkPresentInfo.waitSemaphoreCount = info->waitSemaphoreCount;
    vkPresentInfo.pWaitSemaphores = VkSynchroniserFactory::GetInstance()->GetSemaphore(info->pWaitSemaphoreIds, info->waitSemaphoreCount);
    vkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    vkPresentInfo.pResults = &presentationResult;
    
    //TODO : Expose presentation queue
    //VkQueue * presentQueue = VkQueueFactory::GetInstance()->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::presentationQueuedId);
    VkQueue * presentQueue = VkQueueFactory::GetInstance()->GetQueue(queueWrapper);
    PresentationEngine::GetInstance()->PresentSwapchainImage(&vkPresentInfo, presentQueue);
}

bool VulkanInterface::IsApplicationSafeForClosure()
{
    VkQueueFactory::GetInstance()->WaitForAllQueues();
    vkDeviceWaitIdle(*CoreObjects::logicalDeviceObj);

    return true;
}

/*
uint32_t * VulkanInterface::CreateBuffer(BufferInfo * info, const uint32_t & count)
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
uint32_t * VulkanInterface::CreateBuffers(BufferInfo * info, const uint32_t & count)
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

    ids = VkBufferFactory::GetInstance()->CreateBuffers(count, bufferUsage, memProp, dataSizes);

    delete[] bufferUsage;
    delete[] memProp;
    delete[] dataSizes;
    return ids;
}

void VulkanInterface::CreateBuffers(BufferCreateInfo * info, const uint32_t & count, uint32_t * out_buffIds, size_t * out_bufferMemRequirements)
{
    VkBufferFactory::GetInstance()->CreateBuffers(count, info, out_buffIds, out_bufferMemRequirements);
}

size_t VulkanInterface::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return VkBufferFactory::GetInstance()->GetMemoryAlignedDataSizeForBuffer(dataSize);
}

uint32_t * VulkanInterface::AllocateBufferMemory(uint32_t * bufferId, const uint32_t & bufCount)
{
    return VkBufferFactory::GetInstance()->AllocateBufferMemory(bufferId, bufCount);
}

void VulkanInterface::CopyBufferDataToMemory(const uint32_t & bufId, const VkDeviceSize & dataSize, const VkDeviceSize & memAlignedSize, void * data, VkDeviceSize memoryOffset, bool keepMemoryMounted)
{
    VkBufferFactory::GetInstance()->CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

void VulkanInterface::DestroyBuffer(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        VkBufferFactory::GetInstance()->DestroyBuffer(ids[i]);
    }
}

void VulkanInterface::FreeMemory(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        VulkanMemoryManager::GetSingleton()->FreeMemory(ids[i]);
    }
}

uint32_t * VulkanInterface::AllocateDescriptorsSet(SetWrapper * set, const uint32_t & numDescriptors)
{
    return VkShaderResourceManager::GetInstance()->AllocateDescriptorSets(set, numDescriptors);
}

void VulkanInterface::BindImageMemory(const uint32_t & imageId, const uint32_t & memId, const size_t & offset)
{
    VkAttachmentFactory::GetInstance()->BindImageMemory(imageId, memId, offset);
}

uint32_t VulkanInterface::AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq, const size_t & allocationSize)
{
    VkMemoryRequirements req = VulkanUnwrap::UnwrapMemoryRequirements(memReq);
    VkMemoryPropertyFlags flags = VulkanUnwrap::UnwrapMemoryProperty(userReq);

    uint32_t memId = VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, flags, allocationSize);

    return memId;
}

uint32_t VulkanInterface::AllocateMemory(MemoryRequirementInfo * memReq, MemoryType * userReq, const uint32_t & numUserReq, const size_t & allocationSize)
{
    VkMemoryRequirements req = VulkanUnwrap::UnwrapMemoryRequirements(memReq);
    VkMemoryPropertyFlags flags = VulkanUnwrap::UnwrapMemoryProperty(userReq, numUserReq);

    uint32_t memId = VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, flags, allocationSize);

    return memId;
}

MemoryRequirementInfo VulkanInterface::GetImageMemoryRequirement(const uint32_t & imageId)
{
    MemoryRequirementInfo req = VkAttachmentFactory::GetInstance()->GetImageMemoryRequirement(imageId);
    return req;
}

//
//void VulkanInterface::InitiateGraphicsPipelineCreation(const uint32_t & meshId, VertexInputAttributeInfo * attribInfo, const uint32_t & attribCount, VertexInputBindingInfo * bindingInfo, const uint32_t & bindingCount )
//{
//    VulkanGraphicsPipelineFactory::GetInstance()->InitiatePipelineCreation(meshId, attribInfo, attribCount, bindingInfo, bindingCount);
//}
//
//void VulkanInterface::SetInputAssemblyInfo(const uint32_t & meshId, PrimtiveType * primitive, bool isPrimitiveRestartEnabled)
//{
//    VulkanGraphicsPipelineFactory::GetInstance()->SetInputAssemblyInfo(meshId, primitive, isPrimitiveRestartEnabled);
//}

void VulkanInterface::GetShaderIds(char ** shaderName, ShaderType * type, uint32_t * id, const uint32_t & shaderCount)
{
    VkShaderFactory::GetInstance()->GetShaderIds(shaderName, type, id, shaderCount);
}

void VulkanInterface::CreateVertexInputState(const VertexInputWrapper * vertexInputWrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateVertexInputState(vertexInputWrapper);
}

void VulkanInterface::CreateInputAssemblyState(const InputAssemblyWrapper * inputAssemblyWrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateInputAssemblyState(inputAssemblyWrapper);
}

void VulkanInterface::CreateShaderState(const ShaderStateWrapper * shaderStateWrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateShaderState(shaderStateWrapper);
}

void VulkanInterface::CreateTessellationState(const TessellationStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateTessellationState(wrapper);
}

void VulkanInterface::CreateViewportState(const ViewPortStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateViewportState(wrapper);
}

void VulkanInterface::CreateRasterisationState(const RasterizationStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateRasterisationState(wrapper);
}

void VulkanInterface::CreateDepthStencilState(const DepthStencilStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateDepthStencilState(wrapper);
}

void VulkanInterface::CreateColorBlendState(const ColorBlendStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateColorBlendState(wrapper);
}

void VulkanInterface::CreateMultiSampleState(const MultiSampleStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateMultiSampleState(wrapper);
}

void VulkanInterface::CreateDynamicState(const DynamicStateWrapper * wrapper)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreateDynamicState(wrapper);
}

void VulkanInterface::CreatePipeline(PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineId)
{
    VulkanGraphicsPipelineFactory::GetInstance()->CreatePipeline(info, pipelineCount, pipelineId);
}

std::vector<SetWrapper*> VulkanInterface::GetSetsForShaders(const std::vector<std::string>& shaderNames)
{
    return VkShaderResourceManager::GetInstance()->GetSetsForShaders(shaderNames);
}

uint32_t VulkanInterface::CreatePipelineLayout(SetWrapper ** setWrapperList, const size_t & numSets)
{
    return VkShaderResourceManager::GetInstance()->CreatePipelineLayout(setWrapperList, numSets);
}

std::vector<SetWrapper*>* VulkanInterface::GetSetWrapperList()
{
    return VkShaderResourceManager::GetInstance()->GetSetWrapperList();
}

void VulkanInterface::LinkSetBindingToResources(ShaderBindingDescription * desc, const uint32_t & numBindings)
{
    VkShaderResourceManager::GetInstance()->LinkSetBindingToResources(desc, numBindings);
}

bool VulkanInterface::IsSampleRateShadingAvailable()
{
    return VulkanManager::GetInstance()->IsSampleRateShadingAvailable();
}

Samples VulkanInterface::GetMaxUsableSampleCount()
{
    return VulkanManager::GetInstance()->GetMaxUsableSampleCount();
}

uint32_t VulkanInterface::CreateSampler(const SamplerCreateInfo & info)
{
    return VkSamplerFactory::GetInstance()->CreateSampler(info);
}

uint32_t VulkanInterface::CreateCommandPool(PipelineType * pipelineType, CommandPoolProperty * prop)
{
    uint32_t poolId = 0;

    VkQueueFlagBits queueFlag;
    switch (*pipelineType)
    {
    case PipelineType::COMPUTE :
        queueFlag = VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
        break;

    case PipelineType::GRAPHICS:
        queueFlag = VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT;
        break;

    case PipelineType::TRANSFER:
        queueFlag = VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue/pipeline type not found");
    }

    VkCommandPoolCreateFlagBits poolCreateFlag;
    switch (*prop)
    {
    case CommandPoolProperty::RESET_COMMAND_BUFFER_BIT:
        poolCreateFlag = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ;
        break;

    case CommandPoolProperty::TRANSIENT_BIT:
        poolCreateFlag = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        break;

    case CommandPoolProperty::TRANS_RESET:
        poolCreateFlag = (VkCommandPoolCreateFlagBits)(VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue/pipeline type not found");
    }

    VkCommandBufferFactory::GetInstance()->CreateCommandPool(poolCreateFlag, queueFlag, poolId);

    return poolId;
}

void VulkanInterface::DestroyCommandPool(const uint32_t & poolId)
{
    VkCommandBufferFactory::GetInstance()->DestroyCommandPool(poolId);
}

uint32_t VulkanInterface::CreateFence(bool isSignaled)
{
    return VkSynchroniserFactory::GetInstance()->CreateFence(isSignaled);
}

uint32_t VulkanInterface::Create_Semaphore(bool isSignaled)
{
    return VkSynchroniserFactory::GetInstance()->Create_Semaphore(isSignaled);
}

void VulkanInterface::DestroyFence(const uint32_t & id)
{
    return VkSynchroniserFactory::GetInstance()->DestroyFence(id);
}

void VulkanInterface::DestroySemaphore(const uint32_t & id)
{
    return VkSynchroniserFactory::GetInstance()->DestroySemaphore(id);
}

void VulkanInterface::SetRenderpassBeginInfo(RenderPassBeginInfo * beginInfo, const uint32_t & renderPassId)
{
    VkRenderPassBeginInfo info = VulkanUnwrap::UnwrapRenderPassBeginInfo(*beginInfo);
    VkRenderPassFactory::GetInstance()->SetRenderPassBeginInfo(info, renderPassId);
}

uint32_t VulkanInterface::GetAvailableSwapchainIndex(const uint32_t & fenceId, const uint32_t & semaphoreId)
{
    VkFence * fence = VkSynchroniserFactory::GetInstance()->GetFence(fenceId);
    VkSemaphore * semaphore = VkSynchroniserFactory::GetInstance()->GetSemaphore(semaphoreId);

    return PresentationEngine::GetInstance()->VkGetAvailableSwapChainId(fence, semaphore);
}

void VulkanInterface::ActivateCommandBuffer(const uint32_t & index)
{
    VkCommandBufferFactory::GetInstance()->ActivateCommandBuffer(index);
}

VkDrawCommandBuffer * VulkanInterface::CreateCommandBuffer(const uint32_t & poolId, uint32_t * cmdBufferId, CommandBufferLevel* commandBufferLevel, PipelineType bufferType)
{
    VkCommandBufferLevel level;
    switch (*commandBufferLevel)
    {
    case CommandBufferLevel::PRIMARY:
        level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        break;

    case CommandBufferLevel::SECONDARY:
        level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Wrong command level");
    }
    return VkCommandBufferFactory::GetInstance()->CreateCommandBuffer(poolId, level, bufferType, cmdBufferId);
}

VulkanInterface::~VulkanInterface()
{

}

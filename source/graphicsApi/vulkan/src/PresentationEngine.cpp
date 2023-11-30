#include "PresentationEngine.h"
#include "utility/VkImageFactory.h"
#include "utility/VulkanUtility.h"
#include <Settings.h>
#include <Platform/Assertion.h>
#include <vector>
#include <CorePrecompiled.h>
#include "utility/VkRenderingUnwrapper.h"
#include <VkQueueFactory.h>

typedef GfxVk::Utility::VulkanDeviceInfo DeviceInfo;

GfxVk::Utility::PresentationEngine* GfxVk::Utility::PresentationEngine::instance = nullptr;

void GfxVk::Utility::PresentationEngine::ChangeImageLayout()
{
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo info{};
    info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    info.pNext = nullptr;
    info.queueFamilyIndex = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT, m_presentationQueueId) ;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    ErrorCheck(vkCreateCommandPool(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &pool));

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
    ErrorCheck(vkAllocateCommandBuffers(DeviceInfo::GetLogicalDevice(), &allocInfo, &cmdBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    beginInfo.pNext = nullptr;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);

#if 1
    std::vector<VkImageMemoryBarrier2> list;
    for (auto& image : m_swapChainImageList)
    {
        VkImageMemoryBarrier2 imgBarrier{};
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        imgBarrier.image = image;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgBarrier.pNext = nullptr;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseArrayLayer = 0;
        imgBarrier.subresourceRange.baseMipLevel = 0;
        imgBarrier.subresourceRange.layerCount = 1;
        imgBarrier.subresourceRange.levelCount = 1;
        list.push_back(imgBarrier);
    }

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencyInfo.imageMemoryBarrierCount = list.size();
    dependencyInfo.pImageMemoryBarriers = list.data();
    dependencyInfo.pNext = nullptr;
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

    vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
#else
    std::vector<VkImageMemoryBarrier> list;
    for (auto& image : m_swapChainImageList)
    {
        VkImageMemoryBarrier imgBarrier{};
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.image = image;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgBarrier.pNext = nullptr;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseArrayLayer = 0;
        imgBarrier.subresourceRange.baseMipLevel = 0;
        imgBarrier.subresourceRange.layerCount = 1;
        imgBarrier.subresourceRange.levelCount = 1;
        list.push_back(imgBarrier);
    }

    vkCmdPipelineBarrier(
        cmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0, nullptr,
        0, nullptr,
        list.size(), list.data()
    );
#endif
    vkEndCommandBuffer(cmdBuffer);

    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.pNext = nullptr;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        ErrorCheck(vkCreateFence(DeviceInfo::GetLogicalDevice(), &fenceInfo, DeviceInfo::GetAllocationCallback(), &fence));

    VkQueue* queue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VK_QUEUE_GRAPHICS_BIT, m_presentationQueueId);

    VkSubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    ErrorCheck(vkQueueSubmit(*queue, 1, &submitInfo, fence));

    ErrorCheck(vkWaitForFences(DeviceInfo::GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(DeviceInfo::GetLogicalDevice(), fence, DeviceInfo::GetAllocationCallback());
    vkDestroyCommandPool(DeviceInfo::GetLogicalDevice(), pool, DeviceInfo::GetAllocationCallback());
}

void GfxVk::Utility::PresentationEngine::Init(VkSurfaceKHR surfaceObj, VkSurfaceFormatKHR surfaceFormat, uint32_t& swapbufferCount, uint32_t presentationQueueId)
{
    PLOGD << "PresentationEngine Init";

    this->surfaceObj = surfaceObj;
    this->surfaceFormat = surfaceFormat;
    m_presentationQueueId = presentationQueueId;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(DeviceInfo::GetPhysicalDevice(), surfaceObj, &surfaceCapabilities);

    if (surfaceCapabilities.maxImageCount > 0)
        if (m_swapchainImageCount > surfaceCapabilities.maxImageCount)
            m_swapchainImageCount = surfaceCapabilities.maxImageCount;

    if (m_swapchainImageCount < surfaceCapabilities.minImageCount + 1)
        m_swapchainImageCount = surfaceCapabilities.minImageCount + 1;

    presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    {
        uint32_t count = 0;
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(DeviceInfo::GetPhysicalDevice(), surfaceObj, &count, nullptr));
        std::vector<VkPresentModeKHR> presentModeList(count);
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(DeviceInfo::GetPhysicalDevice(), surfaceObj, &count, presentModeList.data()));

        for (VkPresentModeKHR obj : presentModeList)
        {
            if (obj == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = obj;
                m_swapchainImageCount = 3;
                break;
            }
        }
    }

    swapbufferCount = m_swapchainImageCount;
}

void GfxVk::Utility::PresentationEngine::CreateSwapChain(VkSwapchainCreateInfoKHR swapChainCreateInfo)
{
    ErrorCheck(vkCreateSwapchainKHR(DeviceInfo::GetLogicalDevice(), &swapChainCreateInfo, DeviceInfo::GetAllocationCallback(), &swapchainObj));
}

void GfxVk::Utility::PresentationEngine::CreateSwapChain(Core::Wrappers::ImageCreateInfo info)
{
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.clipped = VK_TRUE; // dont render parts of swapchain image that are out of the frustrum
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1; // 2 meant for sterescopic rendering
    swapChainCreateInfo.imageColorSpace = GfxVk::Unwrap::UnWrapColorSpace(info.m_colorSpace);
    swapChainCreateInfo.imageExtent.height = info.m_height;
    swapChainCreateInfo.imageExtent.width = info.m_width;
    swapChainCreateInfo.imageFormat = GfxVk::Unwrap::UnWrapFormat(info.m_format);
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = GfxVk::Unwrap::UnwrapUsage(info.m_usages);
    swapChainCreateInfo.minImageCount = m_swapchainImageCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surfaceObj;

    CreateSwapChain(swapChainCreateInfo);

    CreateSwapchainImages();

    std::vector<VkImageViewCreateInfo> infoList;
    for (auto& image : m_swapChainImageList)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY };
        createInfo.format = surfaceFormat.format;
        createInfo.image = image;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        infoList.push_back(createInfo);
    }
    CreateSwapchainImageViews(infoList);

    ChangeImageLayout();
}

std::vector<VkImage>* GfxVk::Utility::PresentationEngine::CreateSwapchainImage(Core::Wrappers::AttachmentInfo * info, uint32_t count)
{
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.clipped = VK_TRUE; // dont render parts of swapchain image that are out of the frustrum
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1; // 2 meant for sterescopic rendering
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.minImageCount = m_swapchainImageCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surfaceObj;

    ErrorCheck(vkCreateSwapchainKHR(DeviceInfo::GetLogicalDevice(), &swapChainCreateInfo, DeviceInfo::GetAllocationCallback(), &swapchainObj));

    ErrorCheck(vkGetSwapchainImagesKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, &m_swapchainImageCount, nullptr));

    ASSERT_MSG_DEBUG(count == m_swapchainImageCount, "Swapchain count mis match");

    m_swapChainImageList.resize(m_swapchainImageCount);
    m_swapChainImageViewList.resize(m_swapchainImageCount);

    ErrorCheck(vkGetSwapchainImagesKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, &m_swapchainImageCount, m_swapChainImageList.data()));

    return &m_swapChainImageList;
}

std::vector<VkImageView>* GfxVk::Utility::PresentationEngine::CreateSwapchainImageViews(Core::Wrappers::AttachmentInfo * info, uint32_t count)
{
    for (uint32_t i = 0; i <m_swapchainImageCount; i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY };
        createInfo.format = surfaceFormat.format;
        createInfo.image = m_swapChainImageList[i];
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &createInfo, DeviceInfo::GetAllocationCallback(), &m_swapChainImageViewList[i]));
    }

    return &m_swapChainImageViewList;
}

void GfxVk::Utility::PresentationEngine::CreateSwapchainImages()
{
    std::vector<VkImage> imageList;
    uint32_t count = 0;
    ErrorCheck(vkGetSwapchainImagesKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, &count, nullptr));
    imageList.resize(count);
    ErrorCheck(vkGetSwapchainImagesKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, &count, imageList.data()));

    for each(auto image in imageList)
    {
        m_swapChainImageList.push_back(image);
    }
}

void GfxVk::Utility::PresentationEngine::CreateSwapchainImageViews(const std::vector<VkImageViewCreateInfo>& info)
{
    for (auto& createInfo : info)
    {
        VkImageView view = VK_NULL_HANDLE;
        ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &createInfo, DeviceInfo::GetAllocationCallback(),            &view));
        m_swapChainImageViewList.push_back(std::move(view));
    }
}

void GfxVk::Utility::PresentationEngine::DestroySwapChain()
{
    for (auto& view : m_swapChainImageViewList)
    {
        vkDestroyImageView(DeviceInfo::GetLogicalDevice(), view, DeviceInfo::GetAllocationCallback());
    }
    vkDestroySwapchainKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, DeviceInfo::GetAllocationCallback());

    m_swapChainImageList.clear();
    m_swapChainImageViewList.clear();
}

//deprecated.
void GfxVk::Utility::PresentationEngine::DestroySwapChainImageView()
{
    DEPRECATED;
    for (uint32_t i = 0; i <m_swapchainImageCount; i++)
    {
        vkDestroyImageView(DeviceInfo::GetLogicalDevice(), m_swapChainImageViewList[i], DeviceInfo::GetAllocationCallback());
    }
    m_swapChainImageViewList.clear();
}

void GfxVk::Utility::PresentationEngine::DeInit()
{
    PLOGD << "GfxVk::Utility::PresentationEngine DeInit";
    DestroySwapChain();
}

void GfxVk::Utility::PresentationEngine::Update()
{

}

GfxVk::Utility::PresentationEngine * GfxVk::Utility::PresentationEngine::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Utility::PresentationEngine();
    }
    return instance;
}

GfxVk::Utility::PresentationEngine::~PresentationEngine()
{

}

uint32_t GfxVk::Utility::PresentationEngine::VkGetAvailableSwapChainId(VkFence * fence, VkSemaphore * semaphore)
{
    ErrorCheck(vkWaitForFences(DeviceInfo::GetLogicalDevice(), 1, fence, VK_TRUE, UINT64_MAX));
    ErrorCheck(vkResetFences(DeviceInfo::GetLogicalDevice(), 1, fence));

    ErrorCheck(vkAcquireNextImageKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, UINT64_MAX,
        *semaphore, VK_NULL_HANDLE, &activeSwapchainImageID));

    return activeSwapchainImageID;
}

uint32_t GfxVk::Utility::PresentationEngine::VkGetAvailableSwapChainId(const VkFence& fence, const VkSemaphore& semaphore)
{
    ErrorCheck(vkWaitForFences(DeviceInfo::GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX));
    ErrorCheck(vkResetFences(DeviceInfo::GetLogicalDevice(), 1, &fence));

    ErrorCheck(vkAcquireNextImageKHR(DeviceInfo::GetLogicalDevice(), swapchainObj, UINT64_MAX,
        semaphore, VK_NULL_HANDLE, &activeSwapchainImageID));

    return activeSwapchainImageID;
}

void GfxVk::Utility::PresentationEngine::PresentSwapchainImage(VkPresentInfoKHR * info, VkQueue * presentationQueue)
{
    info->swapchainCount = 1;
    info->pSwapchains = &swapchainObj;

    ErrorCheck(vkQueuePresentKHR(*presentationQueue, info));
    ErrorCheck(*info->pResults);
}

void GfxVk::Utility::PresentationEngine::PresentSwapchainImage(VkPresentInfoKHR& info, const VkQueue& presentationQueue)
{
    if (!info.pSwapchains)
    {
        info.swapchainCount = 1;
        info.pSwapchains = &swapchainObj;
    }

    ErrorCheck(vkQueuePresentKHR(presentationQueue, &info));
    ErrorCheck(*info.pResults);
}

const VkImage& GfxVk::Utility::PresentationEngine::GetSwapchainImage(uint32_t swapbufferIndex)
{
    return m_swapChainImageList[swapbufferIndex];
}

const VkImageView& GfxVk::Utility::PresentationEngine::GetSwapchainImageView(uint32_t swapbufferIndex)
{
    return m_swapChainImageViewList[swapbufferIndex];
}

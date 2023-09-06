#include "PresentationEngine.h"
#include "VkAttachmentFactory.h"
#include "utility/VulkanUtility.h"
#include <Settings.h>
#include <Platform/Assertion.h>
#include <vector>
#include <CorePrecompiled.h>
#include "utility/VkRenderingUnwrapper.h"


GfxVk::Utility::PresentationEngine* GfxVk::Utility::PresentationEngine::instance = nullptr;

void GfxVk::Utility::PresentationEngine::Init(VkSurfaceKHR* surfaceObj, VkSurfaceFormatKHR * surfaceFormat, Core::Settings* settings)
{
    PLOGD << "PresentationEngine Init";

    this->surfaceObj = surfaceObj;
    this->surfaceFormat = surfaceFormat;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*GfxVk::Utility::CoreObjects::physicalDeviceObj, *surfaceObj, &surfaceCapabilities);

    if (surfaceCapabilities.maxImageCount > 0)
        if (m_swapchainImageCount > surfaceCapabilities.maxImageCount)
            m_swapchainImageCount = surfaceCapabilities.maxImageCount;

    if (m_swapchainImageCount < surfaceCapabilities.minImageCount + 1)
        m_swapchainImageCount = surfaceCapabilities.minImageCount + 1;

    presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    {
        uint32_t count = 0;
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(*GfxVk::Utility::CoreObjects::physicalDeviceObj, *surfaceObj, &count, nullptr));
        std::vector<VkPresentModeKHR> presentModeList(count);
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(*GfxVk::Utility::CoreObjects::physicalDeviceObj, *surfaceObj, &count, presentModeList.data()));

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

    settings->SetSwapBufferCount(m_swapchainImageCount);
}

void GfxVk::Utility::PresentationEngine::CreateSwapChain(VkSwapchainCreateInfoKHR swapChainCreateInfo)
{
    ErrorCheck(vkCreateSwapchainKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &swapChainCreateInfo, CoreObjects::pAllocator, &swapchainObj));
}

void GfxVk::Utility::PresentationEngine::CreateSwapChain(Core::Wrappers::ImageInfo info)
{
    VkImageUsageFlags usageFlag = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.clipped = VK_TRUE; // dont render parts of swapchain image that are out of the frustrum
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1; // 2 meant for sterescopic rendering
    swapChainCreateInfo.imageColorSpace = GfxVk::Unwrap::UnWrapColorSpace(info.colorSpace);
    swapChainCreateInfo.imageExtent.height = info.height;
    swapChainCreateInfo.imageExtent.width = info.width;
    swapChainCreateInfo.imageFormat = GfxVk::Unwrap::UnWrapFormat(info.format);
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = GfxVk::Unwrap::UnwrapUsage(info.usage);
    swapChainCreateInfo.minImageCount = m_swapchainImageCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = *surfaceObj;

    CreateSwapChain(swapChainCreateInfo);
}

std::vector<VkImage>* GfxVk::Utility::PresentationEngine::CreateSwapchainImage(Core::Wrappers::AttachmentInfo * info, uint32_t count)
{
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.clipped = VK_TRUE; // dont render parts of swapchain image that are out of the frustrum
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1; // 2 meant for sterescopic rendering
    swapChainCreateInfo.imageColorSpace = surfaceFormat->colorSpace;
    swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapChainCreateInfo.imageFormat = surfaceFormat->format;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.minImageCount = m_swapchainImageCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = *surfaceObj;

    ErrorCheck(vkCreateSwapchainKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &swapChainCreateInfo, CoreObjects::pAllocator, &swapchainObj));

    ErrorCheck(vkGetSwapchainImagesKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, &m_swapchainImageCount, nullptr));

    ASSERT_MSG_DEBUG(count == m_swapchainImageCount, "Swapchain count mis match");

    swapChainImageList.resize(m_swapchainImageCount);
    swapChainImageViewList.resize(m_swapchainImageCount);

    ErrorCheck(vkGetSwapchainImagesKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, &m_swapchainImageCount, swapChainImageList.data()));

    return &swapChainImageList;
}

std::vector<VkImageView>* GfxVk::Utility::PresentationEngine::CreateSwapchainImageViews(Core::Wrappers::AttachmentInfo * info, uint32_t count)
{
    for (uint32_t i = 0; i <m_swapchainImageCount; i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY };
        createInfo.format = surfaceFormat->format;
        createInfo.image = swapChainImageList[i];
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        ErrorCheck(vkCreateImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &createInfo, CoreObjects::pAllocator, &swapChainImageViewList[i]));
    }

    return &swapChainImageViewList;
}

std::vector<VkImage> GfxVk::Utility::PresentationEngine::CreateSwapchainImages(const VkImageCreateInfo & info, uint32_t count)
{
    std::vector<VkImage> imageList;

    ErrorCheck(vkGetSwapchainImagesKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, &count, nullptr));

    imageList.resize(count);

    ErrorCheck(vkGetSwapchainImagesKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, &count, imageList.data()));

    for each(auto image in imageList)
    {
        swapChainImageList.push_back(image);
    }

    return imageList;
}

std::vector<VkImageView> GfxVk::Utility::PresentationEngine::CreateSwapchainImageViews(VkImageViewCreateInfo info, VkImage * images, uint32_t count)
{
    std::vector<VkImageView> imageviewList;
    imageviewList.resize(count);

    for (uint32_t i = 0; i <count; i++)
    {
        info.image = images[i];
        ErrorCheck(vkCreateImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &info, CoreObjects::pAllocator, 
            &imageviewList[i]));
        swapChainImageViewList.push_back(imageviewList[i]);
    }

    return imageviewList;
}

void GfxVk::Utility::PresentationEngine::DestroySwapChain()
{
    vkDestroySwapchainKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, CoreObjects::pAllocator);
    swapChainImageList.clear();
    swapChainImageViewList.clear();
}
//deprecated.
void GfxVk::Utility::PresentationEngine::DestroySwapChainImageView()
{
    DEPRECATED;
    for (uint32_t i = 0; i <m_swapchainImageCount; i++)
    {
        vkDestroyImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapChainImageViewList[i], CoreObjects::pAllocator);
    }
    swapChainImageViewList.clear();
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
    ErrorCheck(vkWaitForFences(*GfxVk::Utility::CoreObjects::logicalDeviceObj, 1, fence, VK_TRUE, UINT64_MAX));
    ErrorCheck(vkResetFences(*GfxVk::Utility::CoreObjects::logicalDeviceObj, 1, fence));

    ErrorCheck(vkAcquireNextImageKHR(*GfxVk::Utility::CoreObjects::logicalDeviceObj, swapchainObj, UINT64_MAX,
        *semaphore, VK_NULL_HANDLE, &activeSwapchainImageID));

    return activeSwapchainImageID;
}

void GfxVk::Utility::PresentationEngine::PresentSwapchainImage(VkPresentInfoKHR * info, VkQueue * presentationQueue)
{
    info->swapchainCount = 1;
    info->pSwapchains = &swapchainObj;

    ErrorCheck(vkQueuePresentKHR(*presentationQueue, info));
    ErrorCheck(*info->pResults);
}

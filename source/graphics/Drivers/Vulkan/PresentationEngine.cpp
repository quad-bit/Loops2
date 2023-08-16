#include "PresentationEngine.h"
#include "VkAttachmentFactory.h"
#include "VulkanUtility.h"
#include <Settings.h>
#include <Assertion.h>
#include <vector>
#include <CorePrecompiled.h>
#include "VkRenderingUnwrapper.h"

//using namespace std;

PresentationEngine* PresentationEngine::instance = nullptr;

void PresentationEngine::Init(VkSurfaceKHR* surfaceObj, VkSurfaceFormatKHR * surfaceFormat)
{
    PLOGD << "PresentationEngine Init";

    this->surfaceObj = surfaceObj;
    this->surfaceFormat = surfaceFormat;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*CoreObjects::physicalDeviceObj, *surfaceObj, &surfaceCapabilities);

    if (surfaceCapabilities.maxImageCount > 0)
        if (swapChainImageCount > surfaceCapabilities.maxImageCount)
            swapChainImageCount = surfaceCapabilities.maxImageCount;

    if (swapChainImageCount < surfaceCapabilities.minImageCount + 1)
        swapChainImageCount = surfaceCapabilities.minImageCount + 1;

    presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    {
        uint32_t count = 0;
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(*CoreObjects::physicalDeviceObj, *surfaceObj, &count, nullptr));
        std::vector<VkPresentModeKHR> presentModeList(count);
        ErrorCheck(vkGetPhysicalDeviceSurfacePresentModesKHR(*CoreObjects::physicalDeviceObj, *surfaceObj, &count, presentModeList.data()));

        for (VkPresentModeKHR obj : presentModeList)
        {
            if (obj == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = obj;
                swapChainImageCount = 3;
                break;
            }
        }
    }

    Settings::swapBufferCount = swapChainImageCount;
}

void PresentationEngine::CreateSwapChain(VkSwapchainCreateInfoKHR swapChainCreateInfo)
{
    ErrorCheck(vkCreateSwapchainKHR(*CoreObjects::logicalDeviceObj, &swapChainCreateInfo, CoreObjects::pAllocator, &swapchainObj));
}

void PresentationEngine::CreateSwapChain(ImageInfo info)
{
    VkImageUsageFlags usageFlag = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.clipped = VK_TRUE; // dont render parts of swapchain image that are out of the frustrum
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.imageArrayLayers = 1; // 2 meant for sterescopic rendering
    swapChainCreateInfo.imageColorSpace = VulkanUnwrap::UnWrapColorSpace(info.colorSpace);
    swapChainCreateInfo.imageExtent.height = info.height;
    swapChainCreateInfo.imageExtent.width = info.width;
    swapChainCreateInfo.imageFormat = VulkanUnwrap::UnWrapFormat(info.format);
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = VulkanUnwrap::UnwrapUsage(info.usage);
    swapChainCreateInfo.minImageCount = Settings::swapBufferCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = *surfaceObj;

    CreateSwapChain(swapChainCreateInfo);
}

std::vector<VkImage>* PresentationEngine::CreateSwapchainImage(AttachmentInfo * info, uint32_t count)
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
    swapChainCreateInfo.minImageCount = swapChainImageCount;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // useful when resizing the window
    swapChainCreateInfo.queueFamilyIndexCount = 0; // as its not shared between multiple queues
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = *surfaceObj;

    ErrorCheck(vkCreateSwapchainKHR(*CoreObjects::logicalDeviceObj, &swapChainCreateInfo, CoreObjects::pAllocator, &swapchainObj));

    ErrorCheck(vkGetSwapchainImagesKHR(*CoreObjects::logicalDeviceObj, swapchainObj, &swapChainImageCount, nullptr));

    ASSERT_MSG_DEBUG(count == swapChainImageCount, "Swapchain count mis match");

    swapChainImageList.resize(swapChainImageCount);
    swapChainImageViewList.resize(swapChainImageCount);

    ErrorCheck(vkGetSwapchainImagesKHR(*CoreObjects::logicalDeviceObj, swapchainObj, &swapChainImageCount, swapChainImageList.data()));

    return &swapChainImageList;
}

std::vector<VkImageView>* PresentationEngine::CreateSwapchainImageViews(AttachmentInfo * info, uint32_t count)
{
    for (uint32_t i = 0; i <swapChainImageCount; i++)
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

        ErrorCheck(vkCreateImageView(*CoreObjects::logicalDeviceObj, &createInfo, CoreObjects::pAllocator, &swapChainImageViewList[i]));
    }

    return &swapChainImageViewList;
}

std::vector<VkImage> PresentationEngine::CreateSwapchainImages(const VkImageCreateInfo & info, uint32_t count)
{
    std::vector<VkImage> imageList;

    ErrorCheck(vkGetSwapchainImagesKHR(*CoreObjects::logicalDeviceObj, swapchainObj, &count, nullptr));

    imageList.resize(count);

    ErrorCheck(vkGetSwapchainImagesKHR(*CoreObjects::logicalDeviceObj, swapchainObj, &count, imageList.data()));

    for each(auto image in imageList)
    {
        swapChainImageList.push_back(image);
    }

    return imageList;
}

std::vector<VkImageView> PresentationEngine::CreateSwapchainImageViews(VkImageViewCreateInfo info, VkImage * images, uint32_t count)
{
    std::vector<VkImageView> imageviewList;
    imageviewList.resize(count);

    for (uint32_t i = 0; i <count; i++)
    {
        info.image = images[i];
        ErrorCheck(vkCreateImageView(*CoreObjects::logicalDeviceObj, &info, CoreObjects::pAllocator, 
            &imageviewList[i]));
        swapChainImageViewList.push_back(imageviewList[i]);
    }

    return imageviewList;
}

void PresentationEngine::DestroySwapChain()
{
    vkDestroySwapchainKHR(*CoreObjects::logicalDeviceObj, swapchainObj, CoreObjects::pAllocator);
    swapChainImageList.clear();
    swapChainImageViewList.clear();
}
//deprecated.
void PresentationEngine::DestroySwapChainImageView()
{
    DEPRECATED;
    for (uint32_t i = 0; i <swapChainImageCount; i++)
    {
        vkDestroyImageView(*CoreObjects::logicalDeviceObj, swapChainImageViewList[i], CoreObjects::pAllocator);
    }
    swapChainImageViewList.clear();
}

void PresentationEngine::DeInit()
{
    PLOGD << "PresentationEngine DeInit";
    DestroySwapChain();
}

void PresentationEngine::Update()
{

}

PresentationEngine * PresentationEngine::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new PresentationEngine();
    }
    return instance;
}

PresentationEngine::~PresentationEngine()
{

}

uint32_t PresentationEngine::VkGetAvailableSwapChainId(VkFence * fence, VkSemaphore * semaphore)
{
    ErrorCheck(vkWaitForFences(*CoreObjects::logicalDeviceObj, 1, fence, VK_TRUE, UINT64_MAX));
    ErrorCheck(vkResetFences(*CoreObjects::logicalDeviceObj, 1, fence));

    ErrorCheck(vkAcquireNextImageKHR(*CoreObjects::logicalDeviceObj, swapchainObj, UINT64_MAX,
        *semaphore, VK_NULL_HANDLE, &activeSwapchainImageID));

    return activeSwapchainImageID;
}

void PresentationEngine::PresentSwapchainImage(VkPresentInfoKHR * info, VkQueue * presentationQueue)
{
    info->swapchainCount = 1;
    info->pSwapchains = &swapchainObj;
    
    ErrorCheck(vkQueuePresentKHR(*presentationQueue, info));
    ErrorCheck(*info->pResults);
}

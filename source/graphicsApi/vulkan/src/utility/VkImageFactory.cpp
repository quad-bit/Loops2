#include "utility/VkImageFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanMemoryManager.h"
#include "PresentationEngine.h"
#include <CorePrecompiled.h>
#include <utility/VkDebugMarkerUtil.h>

GfxVk::Utility::VkImageFactory* GfxVk::Utility::VkImageFactory::s_instance = nullptr;
uint32_t GfxVk::Utility::VkImageFactory::s_imageIdCounter = 0;

void GfxVk::Utility::VkImageFactory::Init(uint32_t swapBufferCount)
{
    PLOGD << "VkImageFactory init";

    // Create image wrapper for swapchain images
    for (uint32_t i = 0; i < swapBufferCount; i++)
    {
        ImageWrapper obj{};
        obj.m_image = PresentationEngine::GetInstance()->GetSwapchainImage(i);
        obj.m_imageId = GetId();
        obj.m_imageView = PresentationEngine::GetInstance()->GetSwapchainImageView(i);;
        obj.m_name = "SwapchainImage_" + std::to_string(i);
        m_swapchainImageList.insert({ obj.m_imageId, obj });
    }
}

void GfxVk::Utility::VkImageFactory::DeInit()
{
    PLOGD << "VkImageFactory Deinit";

    m_imageList.clear();
}

void GfxVk::Utility::VkImageFactory::Update()
{

}

GfxVk::Utility::VkImageFactory* GfxVk::Utility::VkImageFactory::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new GfxVk::Utility::VkImageFactory();
    }
    return s_instance;
}

GfxVk::Utility::VkImageFactory::~VkImageFactory()
{
}

uint32_t GfxVk::Utility::VkImageFactory::CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const Core::Wrappers::ImageViewCreateInfo& viewInfo, const std::string& name)
{
    VkImageCreateInfo info = Unwrap::UnWrapImageCreateInfo(&imageInfo);
    VkImage image;
    GfxVk::Utility::ErrorCheck(vkCreateImage(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &image));

    VkMemoryRequirements req = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&image);
    auto memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.size);
    VkDeviceMemory mem = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);
    GfxVk::Utility::ErrorCheck(vkBindImageMemory(DeviceInfo::GetLogicalDevice(), image, mem, 0));

    VkImageView imageView;
    VkImageViewCreateInfo vkViewInfo = Unwrap::UnWrapImageViewCreateInfo(&viewInfo, image);
    GfxVk::Utility::ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &vkViewInfo, DeviceInfo::GetAllocationCallback(), &imageView));

    ImageWrapper wrapper;
    wrapper.m_image = image;
    wrapper.m_imageView = imageView;
    wrapper.m_imageId = GetId();
    wrapper.m_name = name;
    wrapper.m_memId = memId;
    m_imageList.insert({ wrapper.m_imageId, wrapper });

    GfxVk::DebugMarker::SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)image, name.c_str());

    return wrapper.m_imageId;
}

uint32_t GfxVk::Utility::VkImageFactory::CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name)
{
    VkImageCreateInfo info = Unwrap::UnWrapImageCreateInfo(&imageInfo);
    VkImage image;
    GfxVk::Utility::ErrorCheck(vkCreateImage(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &image));

    ImageWrapper wrapper;
    wrapper.m_image = image;
    wrapper.m_imageId = GetId();
    wrapper.m_name = name;
    m_imageList.insert({ wrapper.m_imageId, wrapper });

    GfxVk::DebugMarker::SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)image, name.c_str());

    return wrapper.m_imageId;
}

void GfxVk::Utility::VkImageFactory::CreateImageView(const Core::Wrappers::ImageViewCreateInfo& viewInfo, uint32_t imageId)
{
    VkImageView imageView;
    auto& image = m_imageList[imageId].m_image;
    VkImageViewCreateInfo vkViewInfo = Unwrap::UnWrapImageViewCreateInfo(&viewInfo, image);
    GfxVk::Utility::ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &vkViewInfo, DeviceInfo::GetAllocationCallback(), &imageView));
    m_imageList[imageId].m_imageView = imageView;
}

void GfxVk::Utility::VkImageFactory::DestroyImage(uint32_t imageId, bool freeImageMemory)
{
    ASSERT_MSG_DEBUG(m_imageList.find(imageId) != m_imageList.end(), "Image id not found");
    {
        vkDestroyImage(DeviceInfo::GetLogicalDevice(), m_imageList[imageId].m_image, DeviceInfo::GetAllocationCallback());
        m_imageList[imageId].m_image = VK_NULL_HANDLE;
    }

    if (m_imageList[imageId].m_imageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(DeviceInfo::GetLogicalDevice(), m_imageList[imageId].m_imageView, DeviceInfo::GetAllocationCallback());
        m_imageList[imageId].m_imageView = VK_NULL_HANDLE;
    }

    if (freeImageMemory == true && m_imageList[imageId].m_memId.has_value())
    {
        GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory(m_imageList[imageId].m_memId.value());
    }

    {
        m_imageList.erase(imageId);
    }
}

Core::Wrappers::MemoryRequirementInfo GfxVk::Utility::VkImageFactory::GetImageMemoryRequirement(uint32_t id)
{
    ASSERT_MSG(m_imageList.find(id) != m_imageList.end(), "Image not found");

    VkMemoryRequirements req = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&m_imageList[id].m_image);

    Core::Wrappers::MemoryRequirementInfo info = {};
    info.alignment = req.alignment;
    info.memoryTypeBits = req.memoryTypeBits;
    info.size = req.size;

    return info;
}


void GfxVk::Utility::VkImageFactory::BindImageMemory(uint32_t imageId, uint32_t memId, size_t offset)
{
    ASSERT_MSG(m_imageList.find(imageId) != m_imageList.end(), "Image not found");
    m_imageList[imageId].m_memId = memId;
    VkDeviceMemory mem = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);

    GfxVk::Utility::ErrorCheck(vkBindImageMemory(DeviceInfo::GetLogicalDevice(), m_imageList[imageId].m_image, mem, offset));
}

const uint32_t& GfxVk::Utility::VkImageFactory::GetSwapchainImage(uint32_t swapchainIndex)
{
    return m_swapchainImageList[swapchainIndex].m_imageId;
}

uint32_t GfxVk::Utility::VkImageFactory::GetId()
{
    return s_imageIdCounter++;
}

const VkImageView& GfxVk::Utility::VkImageFactory::GetImageView(uint32_t id)
{
    // reserved for swapchain images
    if (id < (uint32_t)m_swapchainImageList.size())
    {
        ASSERT_MSG(m_swapchainImageList.find(id) != m_swapchainImageList.end(), "Image not found");
        return m_swapchainImageList[id].m_imageView;
    }
    else
        ASSERT_MSG(m_imageList.find(id) != m_imageList.end(), "Image not found");

    return m_imageList[id].m_imageView;
}

const VkImage& GfxVk::Utility::VkImageFactory::GetImage(uint32_t id)
{
    // reserved for swapchain images
    if (id < (uint32_t)m_swapchainImageList.size())
    {
        ASSERT_MSG(m_swapchainImageList.find(id) != m_swapchainImageList.end(), "Image not found");
        return m_swapchainImageList[id].m_image;
    }
    else
    {
        ASSERT_MSG(m_imageList.find(id) != m_imageList.end(), "Image not found");
        return m_imageList[id].m_image;
    }

    ASSERT_MSG(0, "Image not found");
    return m_swapchainImageList[0].m_image;;
}
#include "utility/VkImageFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanMemoryManager.h"
#include <CorePrecompiled.h>

GfxVk::Utility::VkImageFactory* GfxVk::Utility::VkImageFactory::s_instance = nullptr;
uint32_t GfxVk::Utility::VkImageFactory::s_imageIdCounter = 0;

void GfxVk::Utility::VkImageFactory::Init()
{
    PLOGD << "VkImageFactory init";
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

    VkImageView imageView;
    VkImageViewCreateInfo vkViewInfo = Unwrap::UnWrapImageViewCreateInfo(&viewInfo);
    GfxVk::Utility::ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &vkViewInfo, DeviceInfo::GetAllocationCallback(), &imageView));

    ImageWrapper wrapper;
    wrapper.m_image = image;
    wrapper.m_imageView = imageView;
    wrapper.m_imageId = GetId();
    wrapper.m_name = name;

    m_imageList.push_back(wrapper);

    return wrapper.m_imageId;
}

void GfxVk::Utility::VkImageFactory::DestroyImage(uint32_t imageId, bool freeImageMemory)
{
    std::vector<GfxVk::Utility::ImageWrapper>::iterator it;
    it = std::find_if(m_imageList.begin(), m_imageList.end(), [&](GfxVk::Utility::ImageWrapper e) { return e.m_imageId == imageId; });

    ASSERT_MSG_DEBUG(it != m_imageList.end(), "Image id not found");
    if ((*it).m_image != VK_NULL_HANDLE)
    {
        vkDestroyImage(DeviceInfo::GetLogicalDevice(), (*it).m_image, DeviceInfo::GetAllocationCallback());
        (*it).m_image = VK_NULL_HANDLE;
    }

    if ((*it).m_imageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(DeviceInfo::GetLogicalDevice(), (*it).m_imageView, DeviceInfo::GetAllocationCallback());
        (*it).m_imageView = VK_NULL_HANDLE;
    }

    if (freeImageMemory == true && (*it).m_memId.has_value())
    {
        GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory((*it).m_memId.value());
    }

    {
        m_imageList.erase(it);
    }
}

Core::Wrappers::MemoryRequirementInfo GfxVk::Utility::VkImageFactory::GetImageMemoryRequirement(uint32_t id)
{
    std::vector<GfxVk::Utility::ImageWrapper>::iterator it;
    it = std::find_if(m_imageList.begin(), m_imageList.end(), [&](GfxVk::Utility::ImageWrapper e) { return e.m_imageId == id; });

    ASSERT_MSG(it != m_imageList.end(), "Image not found");

    VkMemoryRequirements req = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&(*it).m_image);

    Core::Wrappers::MemoryRequirementInfo info = {};
    info.alignment = req.alignment;
    info.memoryTypeBits = req.memoryTypeBits;
    info.size = req.size;

    return info;
}


void GfxVk::Utility::VkImageFactory::BindImageMemory(uint32_t imageId, uint32_t memId, size_t offset)
{
    std::vector<GfxVk::Utility::ImageWrapper>::iterator it;
    it = std::find_if(m_imageList.begin(), m_imageList.end(), [&](GfxVk::Utility::ImageWrapper e) { return e.m_imageId == imageId; });

    ASSERT_MSG_DEBUG(it != m_imageList.end(), "Image not found");

    (*it).m_memId = memId;
    VkDeviceMemory mem = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);

    GfxVk::Utility::ErrorCheck(vkBindImageMemory(DeviceInfo::GetLogicalDevice(), (*it).m_image, mem, offset));
}

uint32_t GfxVk::Utility::VkImageFactory::GetId()
{
    return s_imageIdCounter++;
}

VkImageView GfxVk::Utility::VkImageFactory::GetImageView(uint32_t id)
{
    std::vector<GfxVk::Utility::ImageWrapper>::iterator it;
    it = std::find_if(m_imageList.begin(), m_imageList.end(), [&](GfxVk::Utility::ImageWrapper e) { return e.m_imageId == id; });
    ASSERT_MSG_DEBUG(it != m_imageList.end(), "Image not found");

    return (*it).m_imageView;
}

VkImage GfxVk::Utility::VkImageFactory::GetImage(uint32_t id)
{
    std::vector<GfxVk::Utility::ImageWrapper>::iterator it;
    it = std::find_if(m_imageList.begin(), m_imageList.end(), [&](GfxVk::Utility::ImageWrapper e) { return e.m_imageId == id; });

    ASSERT_MSG_DEBUG(it != m_imageList.end(), "Image not found");

    return (*it).m_image;
}
#include "utility/VkImageFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanMemoryManager.h"
#include "PresentationEngine.h"
#include <CorePrecompiled.h>
#include <utility/VkDebugMarkerUtil.h>
#include "VkCommandBufferFactory.h"
#include "VkQueueFactory.h"
#include <IO/FileReader.h>

GfxVk::Utility::VkImageFactory* GfxVk::Utility::VkImageFactory::s_instance = nullptr;
uint32_t GfxVk::Utility::VkImageFactory::s_imageIdCounter = 0;

void CreateStagingBuffer(VkBuffer& buffer, VkDeviceMemory& memory, uint32_t& memId, void* imageData, uint32_t dataSize)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    GfxVk::Utility::ErrorCheck(vkCreateBuffer(DeviceInfo::GetLogicalDevice(), &bufferCreateInfo, nullptr, &buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(DeviceInfo::GetLogicalDevice(), buffer, &memReqs);
    memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(
        &memReqs,
        VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memReqs.size);
    memory = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);
    GfxVk::Utility::ErrorCheck(vkBindBufferMemory(DeviceInfo::GetLogicalDevice(), buffer, memory, 0));

    uint8_t* data;
    GfxVk::Utility::ErrorCheck(vkMapMemory(DeviceInfo::GetLogicalDevice(), memory, 0, memReqs.size, 0, (void**)&data));
    memcpy(data, imageData, dataSize);
    vkUnmapMemory(DeviceInfo::GetLogicalDevice(), memory);
}



void GfxVk::Utility::VkImageFactory::Init(uint32_t swapBufferCount, uint32_t transferQueueId)
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

    m_transferQueueId = transferQueueId;
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

uint32_t GfxVk::Utility::VkImageFactory::CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name, bool createView)
{
    VkImageCreateInfo info = Unwrap::UnWrapImageCreateInfo(&imageInfo);
    if (imageInfo.m_arrayLayers.has_value())
    {
        info.arrayLayers = imageInfo.m_arrayLayers.value();
        info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    VkImage image;
    GfxVk::Utility::ErrorCheck(vkCreateImage(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &image));

    ImageWrapper wrapper;
    wrapper.m_image = image;
    wrapper.m_imageId = GetId();
    wrapper.m_name = name;

    if (createView)
    {
        VkMemoryRequirements req = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&image);
        auto memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(&req, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.size);
        VkDeviceMemory mem = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);
        GfxVk::Utility::ErrorCheck(vkBindImageMemory(DeviceInfo::GetLogicalDevice(), image, mem, 0));

        VkImageView imageView;
        VkImageViewCreateInfo vkViewInfo = {};
        vkViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkViewInfo.flags = 0;
        vkViewInfo.format = Unwrap::UnWrapFormat(imageInfo.m_format);
        vkViewInfo.image = image;
        vkViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
        vkViewInfo.subresourceRange.baseArrayLayer = 0;
        vkViewInfo.subresourceRange.baseMipLevel = 0;
        vkViewInfo.subresourceRange.layerCount = imageInfo.m_arrayLayers.has_value() ? 6 : 1;
        vkViewInfo.subresourceRange.levelCount = 1;
        vkViewInfo.viewType = Unwrap::UnwrapImageViewType(imageInfo.m_viewType);

        GfxVk::Utility::ErrorCheck(vkCreateImageView(DeviceInfo::GetLogicalDevice(), &vkViewInfo, DeviceInfo::GetAllocationCallback(), &imageView));

        wrapper.m_imageView = imageView;
        wrapper.m_memId = memId;
    }

    m_imageList.insert({ wrapper.m_imageId, wrapper });

    GfxVk::DebugMarker::SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)image, name.c_str());

    return wrapper.m_imageId;
}

uint32_t GfxVk::Utility::VkImageFactory::CreateImage(void* buffer, size_t bufferSize, const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name)
{
    auto imageId = CreateImage(imageInfo, name, true);
    const VkImage& image = GetImage(imageId);

    auto id = GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->CreateCommandBuffer(
        VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);

    VkCommandBuffer cmdBuffer = GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->GetCommandBuffer(id, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);

    VkBuffer stagingBuffer;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    GfxVk::Utility::ErrorCheck(vkCreateBuffer(DeviceInfo::GetLogicalDevice(), &bufferCreateInfo, nullptr, &stagingBuffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(DeviceInfo::GetLogicalDevice(), stagingBuffer, &memReqs);
    auto memId = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->AllocateMemory(
        &memReqs,
        VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memReqs.size);
    VkDeviceMemory stagingMemory = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);
    GfxVk::Utility::ErrorCheck(vkBindBufferMemory(DeviceInfo::GetLogicalDevice(), stagingBuffer, stagingMemory, 0));

    uint8_t* data;
    GfxVk::Utility::ErrorCheck(vkMapMemory(DeviceInfo::GetLogicalDevice(), stagingMemory, 0, memReqs.size, 0, (void**)&data));
    memcpy(data, buffer, bufferSize);
    vkUnmapMemory(DeviceInfo::GetLogicalDevice(), stagingMemory);

    // copy data 
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.pNext = nullptr;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        // change layout
        std::vector<VkImageMemoryBarrier2> list;
        VkImageMemoryBarrier2 imgBarrier{};
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        imgBarrier.image = image;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
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

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencyInfo.imageMemoryBarrierCount = list.size();
        dependencyInfo.pImageMemoryBarriers = list.data();
        dependencyInfo.pNext = nullptr;
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

        vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);

        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = imageInfo.m_width;
        bufferCopyRegion.imageExtent.height = imageInfo.m_height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = 0;

        vkCmdCopyBufferToImage(
            cmdBuffer,
            stagingBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &bufferCopyRegion
        );

        list.clear();
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        list.push_back(imgBarrier);

        dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencyInfo.imageMemoryBarrierCount = list.size();
        dependencyInfo.pImageMemoryBarriers = list.data();
        dependencyInfo.pNext = nullptr;
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

        vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
    }

    vkEndCommandBuffer(cmdBuffer);

    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.pNext = nullptr;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ErrorCheck(vkCreateFence(DeviceInfo::GetLogicalDevice(), &fenceInfo, DeviceInfo::GetAllocationCallback(), &fence));

    VkQueue* queue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VK_QUEUE_TRANSFER_BIT, m_transferQueueId);

    VkSubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    ErrorCheck(vkQueueSubmit(*queue, 1, &submitInfo, fence));

    ErrorCheck(vkWaitForFences(DeviceInfo::GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(DeviceInfo::GetLogicalDevice(), fence, DeviceInfo::GetAllocationCallback());

    // delete resources
    vkDestroyBuffer(DeviceInfo::GetLogicalDevice(), stagingBuffer, nullptr);
    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory(memId);
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DestroyCommandBuffer(id, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);
    return imageId;
}

uint32_t GfxVk::Utility::VkImageFactory::CreateCubeMap(const std::string& path, const std::string& imageName)
{
    uint32_t width, height;
    uint32_t dataSize;
    auto data = Core::IO::ReadKtxImage(width, height, dataSize, path.c_str());

    Core::Wrappers::ImageCreateInfo imageInfo = {};
    imageInfo.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
    imageInfo.m_depth = 1;
    imageInfo.m_format = Core::Enums::Format::B8G8R8A8_UNORM;
    imageInfo.m_height = height;
    imageInfo.m_imageType = Core::Enums::ImageType::IMAGE_TYPE_2D;
    imageInfo.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
    imageInfo.m_layers = 6;
    imageInfo.m_mips = 1;
    imageInfo.m_sampleCount = Core::Enums::Samples::SAMPLE_COUNT_1_BIT;
    imageInfo.m_usages = { Core::Enums::ImageUsage::USAGE_TRANSFER_DST_BIT, Core::Enums::ImageUsage::USAGE_SAMPLED_BIT };
    imageInfo.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_CUBE;
    imageInfo.m_width = width;
    imageInfo.m_arrayLayers = 6;

    auto imageId = CreateImage(imageInfo, imageName, true);
    const VkImage& image = GetImage(imageId);

    auto id = GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->CreateCommandBuffer(
        VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);

    VkCommandBuffer cmdBuffer = GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->GetCommandBuffer(id, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    uint32_t memId = 0;
    CreateStagingBuffer(buffer, memory, memId, data, dataSize);

    // copy data 
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.pNext = nullptr;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        // change layout
        std::vector<VkImageMemoryBarrier2> list;
        VkImageMemoryBarrier2 imgBarrier{};
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        imgBarrier.image = image;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgBarrier.pNext = nullptr;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange.baseArrayLayer = 0;
        imgBarrier.subresourceRange.baseMipLevel = 0;
        imgBarrier.subresourceRange.layerCount = 6;
        imgBarrier.subresourceRange.levelCount = 1;
        list.push_back(imgBarrier);

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencyInfo.imageMemoryBarrierCount = list.size();
        dependencyInfo.pImageMemoryBarriers = list.data();
        dependencyInfo.pNext = nullptr;
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

        vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);

        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
        bufferCopyRegion.imageSubresource.layerCount = 6;
        bufferCopyRegion.imageExtent.width = imageInfo.m_width;
        bufferCopyRegion.imageExtent.height = imageInfo.m_height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = 0;

        vkCmdCopyBufferToImage(
            cmdBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &bufferCopyRegion
        );

        list.clear();
        imgBarrier.dstAccessMask = 0;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        imgBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imgBarrier.srcAccessMask = 0;
        imgBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        list.push_back(imgBarrier);

        dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencyInfo.imageMemoryBarrierCount = list.size();
        dependencyInfo.pImageMemoryBarriers = list.data();
        dependencyInfo.pNext = nullptr;
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

        vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
    }

    vkEndCommandBuffer(cmdBuffer);

    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.pNext = nullptr;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ErrorCheck(vkCreateFence(DeviceInfo::GetLogicalDevice(), &fenceInfo, DeviceInfo::GetAllocationCallback(), &fence));

    VkQueue* queue = GfxVk::Utility::VkQueueFactory::GetInstance()->GetQueue(VK_QUEUE_TRANSFER_BIT, m_transferQueueId);

    VkSubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    ErrorCheck(vkQueueSubmit(*queue, 1, &submitInfo, fence));

    ErrorCheck(vkWaitForFences(DeviceInfo::GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(DeviceInfo::GetLogicalDevice(), fence, DeviceInfo::GetAllocationCallback());

    // delete resources
    vkDestroyBuffer(DeviceInfo::GetLogicalDevice(), buffer, nullptr);
    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory(memId);
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DestroyCommandBuffer(id, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);
    return imageId;
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
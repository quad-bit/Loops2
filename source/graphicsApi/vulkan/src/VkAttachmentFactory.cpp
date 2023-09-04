#include "VkAttachmentFactory.h"
#include "utility/VulkanUtility.h"
#include "PresentationEngine.h"
#include "utility/VulkanMemoryManager.h"
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <CorePrecompiled.h>
#include <Platform/Assertion.h>
#include "utility/VkRenderingUnwrapper.h"


GfxVk::Framebuffer::VkAttachmentFactory * GfxVk::Framebuffer::VkAttachmentFactory::instance = nullptr;
uint32_t GfxVk::Framebuffer::VkAttachmentFactory::imageId = 0;

GfxVk::Framebuffer::AttachmentInfo * GfxVk::Framebuffer::VkAttachmentFactory::UnwrapImageInfo(Core::Wrappers::ImageInfo * imageInfo)
{
    GfxVk::Framebuffer::AttachmentInfo* vulkanImageInfo = new GfxVk::Framebuffer::AttachmentInfo();

    vulkanImageInfo->colorSpace = GfxVk::Unwrap::UnWrapColorSpace( imageInfo->colorSpace) ;
    vulkanImageInfo->imageType = GfxVk::Unwrap::UnWrapImageType(imageInfo->imageType);
    vulkanImageInfo->format = GfxVk::Unwrap::UnWrapFormat(imageInfo->format);
    vulkanImageInfo->height = imageInfo->height;
    vulkanImageInfo->layers = imageInfo->layers;
    vulkanImageInfo->mips = imageInfo->mips;
    vulkanImageInfo->width = imageInfo->width;
    vulkanImageInfo->sampleCount = GfxVk::Unwrap::UnWrapSampleCount(imageInfo->sampleCount);
    vulkanImageInfo->usage = GfxVk::Unwrap::UnwrapUsage(imageInfo->usage);
    vulkanImageInfo->initialLayout = GfxVk::Unwrap::UnWrapImageLayout(imageInfo->initialLayout);
    return vulkanImageInfo;
}

void GfxVk::Framebuffer::VkAttachmentFactory::Init()
{
    PLOGD << "VkAttachmentFactory init";
}

void GfxVk::Framebuffer::VkAttachmentFactory::DeInit()
{
    PLOGD << "VkAttachmentFactory Deinit";

    for (uint32_t i = 0; i < attachmentList.size(); i++)
    {
        delete attachmentList[i];
    }
    attachmentList.clear();
}

void GfxVk::Framebuffer::VkAttachmentFactory::Update()
{

}

GfxVk::Framebuffer::VkAttachmentFactory *GfxVk::Framebuffer::VkAttachmentFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Framebuffer::VkAttachmentFactory();
    }
    return instance;
}

GfxVk::Framebuffer::VkAttachmentFactory::~VkAttachmentFactory()
{
}


int GfxVk::Framebuffer::VkAttachmentFactory::FindBestDepthFormat(Core::Enums::Format * imageFormat, uint32_t count)
{
    VkFormat * formatList = new VkFormat[count];
    VkFormatProperties props = {};
    
    for (uint32_t i = 0; i < count; i++)
    {
        formatList[i] = GfxVk::Unwrap::UnWrapFormat( imageFormat[i] );
        vkGetPhysicalDeviceFormatProperties(*GfxVk::Utility::CoreObjects::physicalDeviceObj, formatList[i], &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            delete[] formatList;
            return i;
        }
    }

    delete[] formatList;
    return -1;
}

void GfxVk::Framebuffer::VkAttachmentFactory::CreateColorAttachment(Core::Wrappers::ImageInfo * info, uint32_t count, bool defaultTarget, std::vector<uint32_t>* ids)
{
    /*
    AttachmentInfo * attachmentInfo = UnwrapImageInfo(info);
    
    if (defaultTarget == true)
    {
        std::vector<VkImage> * imageVector = (PresentationEngine::GetInstance()->CreateSwapchainImage(attachmentInfo, count));
        std::vector<VkImageView> * imageViewVector = PresentationEngine::GetInstance()->CreateSwapchainImageViews(attachmentInfo, count);
        
        for (uint32_t i = 0; i < count; i++)
        {
            AttachmentWrapper * temp = new AttachmentWrapper;
            temp->id = GetId();
            temp->image = &imageVector->at(i);
            temp->imageView = &imageViewVector->at(i);
            temp->imageMemory = nullptr;
            temp->usage = attachmentInfo->usage;

            ids->push_back(temp->id);
            attachmentList.push_back(temp);
        }
    }

    delete attachmentInfo;
    */
}

// single Create infos, count = swapBufferCount
void GfxVk::Framebuffer::VkAttachmentFactory::CreateSwapChainImage(VkImageCreateInfo imageCreateinfo, VkImageViewCreateInfo imageViewCreateinfo, const uint32_t & count, uint32_t * ids)
{
    std::vector<VkImage> imageVector = GfxVk::Utility::PresentationEngine::GetInstance()->CreateSwapchainImages(imageCreateinfo, count);
    std::vector<VkImageView> imageViewVector = GfxVk::Utility::PresentationEngine::GetInstance()->CreateSwapchainImageViews(imageViewCreateinfo, imageVector.data(), count);

    for (uint32_t i = 0; i < count; i++)
    {
        GfxVk::Framebuffer::AttachmentWrapper * temp = new GfxVk::Framebuffer::AttachmentWrapper;
        temp->id = GetId();
        temp->image = imageVector[i];
        temp->imageView = imageViewVector[i];
        temp->imageMemory = nullptr;
        temp->usage = imageCreateinfo.usage;
        temp->isDefaultTarget = true;

        ids[i] = temp->id;
        attachmentList.push_back(temp);
    }
}

void GfxVk::Framebuffer::VkAttachmentFactory::CreateAttachment(VkImageCreateInfo * info, uint32_t count, uint32_t * ids)
{
    if (info == nullptr)
    {
        ASSERT_MSG_DEBUG(0, " Info null");
        std::exit(-1);
    }

    for (uint32_t i = 0; i < count; i++)
    {
        GfxVk::Framebuffer::AttachmentWrapper * temp = new GfxVk::Framebuffer::AttachmentWrapper;
        temp->id = GetId();
        temp->usage = info[i].usage;
        ids[i] = (temp->id);

        GfxVk::Utility::ErrorCheck(vkCreateImage(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &info[i], GfxVk::Utility::CoreObjects::pAllocator, &temp->image));

        /*VulkanMemoryManager::GetSingleton()->AllocateImageMemory(temp->image, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, temp->imageMemory);
        vkBindImageMemory(*GfxVk::Utility::CoreObjects::logicalDeviceObj, *temp->image, *temp->imageMemory, 0);
        */
        /*
        bool stencilAvailable = false;
        if (stencilRequired)
            if (info[i].format == VK_FORMAT_D32_SFLOAT_S8_UINT || info[i].format == VK_FORMAT_D24_UNORM_S8_UINT
                || info[i].format == VK_FORMAT_D16_UNORM_S8_UINT)
                stencilAvailable = true;
        
        viewInfo[i].image = temp->image;
        viewInfo[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (stencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);

        GfxVk::Utility::ErrorCheck(vkCreateImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &viewInfo[i],GfxVk::Utility::CoreObjects::pAllocator, &temp->imageView));
        */
        attachmentList.push_back(temp);
    }
}

Core::Wrappers::MemoryRequirementInfo GfxVk::Framebuffer::VkAttachmentFactory::GetImageMemoryRequirement(const uint32_t & id)
{
    std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == id; });

    ASSERT_MSG(it != attachmentList.end(), "Image not found");

    VkMemoryRequirements req = GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&(*it)->image);

    (*it)->imageMemoryRequirements = req;

    Core::Wrappers::MemoryRequirementInfo info = {};
    info.alignment = req.alignment;
    info.memoryTypeBits = req.memoryTypeBits;
    info.size = req.size;

    return info;
}


void GfxVk::Framebuffer::VkAttachmentFactory::CreateImageView(Core::Wrappers::ImageViewInfo * info, const uint32_t & count)
{
    std::vector<VkImageViewCreateInfo> viewList;
    std::vector<GfxVk::Framebuffer::AttachmentWrapper *> wrapperList;

    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == info[i].imageId; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        VkImageViewCreateInfo view = GfxVk::Unwrap::UnWrapImageViewCreateInfo(&info[i]);
        view.image = (*it)->image;

        wrapperList.push_back(*it);
        viewList.push_back(view);
    }

    CreateImageView(viewList.data(), count, wrapperList.data());
}

void GfxVk::Framebuffer::VkAttachmentFactory::CreateImageView(VkImageViewCreateInfo * info, const uint32_t & count, GfxVk::Framebuffer::AttachmentWrapper ** wrappers)
{
    for (uint32_t i = 0; i < count; i++)
    {
        GfxVk::Utility::ErrorCheck(vkCreateImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &info[i], GfxVk::Utility::CoreObjects::pAllocator, &(wrappers)[i]->imageView));
    }
}
//deprecated.
void GfxVk::Framebuffer::VkAttachmentFactory::DestroyAttachment(std::vector<uint32_t> ids, bool defaultTarget)
{
    DEPRECATED;
    std::reverse(std::begin(ids), std::end(ids));

    bool swapChainDestroyed = false;

    for (uint32_t i = 0; i < ids.size(); i++)
    {
        std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        if (defaultTarget == true && (*it)->usage == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            if (swapChainDestroyed == false)
            {
                GfxVk::Utility::PresentationEngine::GetInstance()->DestroySwapChainImageView();
                GfxVk::Utility::PresentationEngine::GetInstance()->DestroySwapChain();
                swapChainDestroyed = true;
            }
        }
        else
            (*it)->DeActivateAttachment();
    }
}

void GfxVk::Framebuffer::VkAttachmentFactory::DestroyAttachment(uint32_t * ids, bool * destroyImageView, bool * freeImageMemory, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");
        uint32_t count = 0;
        if ((*it)->image != VK_NULL_HANDLE)
        {
            vkDestroyImage(*GfxVk::Utility::CoreObjects::logicalDeviceObj, (*it)->image, GfxVk::Utility::CoreObjects::pAllocator);
            (*it)->image = VK_NULL_HANDLE;
            count++;
        }

        if (destroyImageView[i] == true)
        {
            if ((*it)->imageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, (*it)->imageView, GfxVk::Utility::CoreObjects::pAllocator);
                (*it)->imageView = VK_NULL_HANDLE;
                count++;
            }
        }

        if (freeImageMemory[i] == true)
        {
            if ((*it)->imageMemory != VK_NULL_HANDLE)
            {
                //vkFreeMemory(*GfxVk::Utility::CoreObjects::logicalDeviceObj, (*it)->imageMemory, GfxVk::Utility::CoreObjects::pAllocator);
                GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory((*it)->memoryId);
                (*it)->imageMemory = VK_NULL_HANDLE;
                count++;
            }
        }

        if (count == 3)
        {
            delete (*it);
            attachmentList.erase(it);
        }
    }
}

void GfxVk::Framebuffer::VkAttachmentFactory::FreeAttachmentMemory(uint32_t * imageIds, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == imageIds[i]; });
        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        GfxVk::Utility::VulkanMemoryManager::GetSingleton()->FreeMemory((*it)->memoryId);
        (*it)->imageMemory = VK_NULL_HANDLE;

        if ((*it)->image == VK_NULL_HANDLE &&
            (*it)->imageView == VK_NULL_HANDLE &&
            (*it)->imageMemory == VK_NULL_HANDLE)
        {
            delete (*it);
            attachmentList.erase(it);
        }
    }
}

void GfxVk::Framebuffer::VkAttachmentFactory::DestroySwapChainImageViews(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == ids[i]; });
        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        vkDestroyImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, (*it)->imageView, GfxVk::Utility::CoreObjects::pAllocator);
        
        delete (*it);
        attachmentList.erase(it);
    }
}


VkImageView GfxVk::Framebuffer::VkAttachmentFactory::GetImageView(uint32_t id)
{
    std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == id; });

    return (*it)->imageView;
}

VkImage * GfxVk::Framebuffer::VkAttachmentFactory::GetImage(const uint32_t & id)
{
    std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image not found");

    return &(*it)->image;
}

void GfxVk::Framebuffer::VkAttachmentFactory::BindImageMemory(const uint32_t & imageId, const uint32_t & memId, const size_t & offset)
{
    std::vector<GfxVk::Framebuffer::AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](GfxVk::Framebuffer::AttachmentWrapper * e) { return e->id == imageId; });

    ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image not found");

    (*it)->memoryId = memId;
    (*it)->imageMemory = *GfxVk::Utility::VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);

    GfxVk::Utility::ErrorCheck( vkBindImageMemory(*GfxVk::Utility::CoreObjects::logicalDeviceObj, (*it)->image, (*it)->imageMemory, offset));
}

uint32_t GfxVk::Framebuffer::VkAttachmentFactory::GetId()
{
    return imageId++;
}

void GfxVk::Framebuffer::AttachmentWrapper::DeActivateAttachment()
{
    if (imageView != VK_NULL_HANDLE)
        vkDestroyImageView(*GfxVk::Utility::CoreObjects::logicalDeviceObj, imageView, GfxVk::Utility::CoreObjects::pAllocator);
    if (imageMemory != VK_NULL_HANDLE)
        vkFreeMemory(*GfxVk::Utility::CoreObjects::logicalDeviceObj, imageMemory, GfxVk::Utility::CoreObjects::pAllocator);
    if (image != VK_NULL_HANDLE)
        vkDestroyImage(*GfxVk::Utility::CoreObjects::logicalDeviceObj, image, GfxVk::Utility::CoreObjects::pAllocator);

    attachmentActive = false;
}

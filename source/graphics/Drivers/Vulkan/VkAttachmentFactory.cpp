#include "VkAttachmentFactory.h"
#include "VulkanUtility.h"
#include "PresentationEngine.h"
#include "VulkanMemoryManager.h"
#include <RenderingWrapper.h>
#include <CorePrecompiled.h>
#include <Assertion.h>

VkAttachmentFactory *VkAttachmentFactory::instance = nullptr;
uint32_t VkAttachmentFactory::imageId = 0;

AttachmentInfo * VkAttachmentFactory::UnwrapImageInfo(ImageInfo * imageInfo)
{
    AttachmentInfo* vulkanImageInfo = new AttachmentInfo();

    vulkanImageInfo->colorSpace = VulkanUnwrap::UnWrapColorSpace( imageInfo->colorSpace) ;
    vulkanImageInfo->imageType = VulkanUnwrap::UnWrapImageType(imageInfo->imageType);
    vulkanImageInfo->format = VulkanUnwrap::UnWrapFormat(imageInfo->format);
    vulkanImageInfo->height = imageInfo->height;
    vulkanImageInfo->layers = imageInfo->layers;
    vulkanImageInfo->mips = imageInfo->mips;
    vulkanImageInfo->width = imageInfo->width;
    vulkanImageInfo->sampleCount = VulkanUnwrap::UnWrapSampleCount(imageInfo->sampleCount);
    vulkanImageInfo->usage = VulkanUnwrap::UnwrapUsage(imageInfo->usage);
    vulkanImageInfo->initialLayout = VulkanUnwrap::UnWrapImageLayout(imageInfo->initialLayout);
    return vulkanImageInfo;
}

void VkAttachmentFactory::Init()
{
    PLOGD << "VkAttachmentFactory init";
}

void VkAttachmentFactory::DeInit()
{
    PLOGD << "VkAttachmentFactory Deinit";

    for (uint32_t i = 0; i < attachmentList.size(); i++)
    {
        delete attachmentList[i];
    }
    attachmentList.clear();
}

void VkAttachmentFactory::Update()
{

}

VkAttachmentFactory *VkAttachmentFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkAttachmentFactory();
    }
    return instance;
}

VkAttachmentFactory::~VkAttachmentFactory()
{
}


int VkAttachmentFactory::FindBestDepthFormat(Format * imageFormat, uint32_t count)
{
    VkFormat * formatList = new VkFormat[count];
    VkFormatProperties props = {};
    
    for (uint32_t i = 0; i < count; i++)
    {
        formatList[i] = VulkanUnwrap::UnWrapFormat( imageFormat[i] );
        vkGetPhysicalDeviceFormatProperties(*CoreObjects::physicalDeviceObj, formatList[i], &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            delete[] formatList;
            return i;
        }
    }

    delete[] formatList;
    return -1;
}

void VkAttachmentFactory::CreateColorAttachment(ImageInfo * info, uint32_t count, bool defaultTarget, std::vector<uint32_t>* ids)
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
void VkAttachmentFactory::CreateSwapChainImage(VkImageCreateInfo imageCreateinfo, VkImageViewCreateInfo imageViewCreateinfo, const uint32_t & count, uint32_t * ids)
{
    std::vector<VkImage> imageVector = PresentationEngine::GetInstance()->CreateSwapchainImages(imageCreateinfo, count);
    std::vector<VkImageView> imageViewVector = PresentationEngine::GetInstance()->CreateSwapchainImageViews(imageViewCreateinfo, imageVector.data(), count);

    for (uint32_t i = 0; i < count; i++)
    {
        AttachmentWrapper * temp = new AttachmentWrapper;
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

void VkAttachmentFactory::CreateAttachment(VkImageCreateInfo * info, uint32_t count, uint32_t * ids)
{
    if (info == nullptr)
    {
        ASSERT_MSG_DEBUG(0, " Info null");
        std::exit(-1);
    }

    for (uint32_t i = 0; i < count; i++)
    {
        AttachmentWrapper * temp = new AttachmentWrapper;
        temp->id = GetId();
        temp->usage = info[i].usage;
        ids[i] = (temp->id);

        ErrorCheck(vkCreateImage(*CoreObjects::logicalDeviceObj, &info[i], CoreObjects::pAllocator, &temp->image));

        /*VulkanMemoryManager::GetSingleton()->AllocateImageMemory(temp->image, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, temp->imageMemory);
        vkBindImageMemory(*CoreObjects::logicalDeviceObj, *temp->image, *temp->imageMemory, 0);
        */
        /*
        bool stencilAvailable = false;
        if (stencilRequired)
            if (info[i].format == VK_FORMAT_D32_SFLOAT_S8_UINT || info[i].format == VK_FORMAT_D24_UNORM_S8_UINT
                || info[i].format == VK_FORMAT_D16_UNORM_S8_UINT)
                stencilAvailable = true;
        
        viewInfo[i].image = temp->image;
        viewInfo[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (stencilAvailable ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);

        ErrorCheck(vkCreateImageView(*CoreObjects::logicalDeviceObj, &viewInfo[i], CoreObjects::pAllocator, &temp->imageView));
        */
        attachmentList.push_back(temp);
    }
}

MemoryRequirementInfo VkAttachmentFactory::GetImageMemoryRequirement(const uint32_t & id)
{
    std::vector<AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == id; });

    ASSERT_MSG(it != attachmentList.end(), "Image not found");

    VkMemoryRequirements req = VulkanMemoryManager::GetSingleton()->GetImageMemoryRequirement(&(*it)->image);

    (*it)->imageMemoryRequirements = req;

    MemoryRequirementInfo info = {};
    info.alignment = req.alignment;
    info.memoryTypeBits = req.memoryTypeBits;
    info.size = req.size;

    return info;
}


void VkAttachmentFactory::CreateImageView(ImageViewInfo * info, const uint32_t & count)
{
    std::vector<VkImageViewCreateInfo> viewList;
    std::vector<AttachmentWrapper *> wrapperList;

    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == info[i].imageId; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        VkImageViewCreateInfo view = VulkanUnwrap::UnWrapImageViewCreateInfo(&info[i]);
        view.image = (*it)->image;

        wrapperList.push_back(*it);
        viewList.push_back(view);
    }

    CreateImageView(viewList.data(), count, wrapperList.data());
}

void VkAttachmentFactory::CreateImageView(VkImageViewCreateInfo * info, const uint32_t & count, AttachmentWrapper ** wrappers)
{
    for (uint32_t i = 0; i < count; i++)
    {
        ErrorCheck(vkCreateImageView(*CoreObjects::logicalDeviceObj, &info[i], CoreObjects::pAllocator, &(wrappers)[i]->imageView));
    }
}
//deprecated.
void VkAttachmentFactory::DestroyAttachment(std::vector<uint32_t> ids, bool defaultTarget)
{
    DEPRECATED;
    std::reverse(std::begin(ids), std::end(ids));

    bool swapChainDestroyed = false;

    for (uint32_t i = 0; i < ids.size(); i++)
    {
        std::vector<AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        if (defaultTarget == true && (*it)->usage == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            if (swapChainDestroyed == false)
            {
                PresentationEngine::GetInstance()->DestroySwapChainImageView();
                PresentationEngine::GetInstance()->DestroySwapChain();
                swapChainDestroyed = true;
            }
        }
        else
            (*it)->DeActivateAttachment();
    }
}

void VkAttachmentFactory::DestroyAttachment(uint32_t * ids, bool * destroyImageView, bool * freeImageMemory, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");
        uint32_t count = 0;
        if ((*it)->image != VK_NULL_HANDLE)
        {
            vkDestroyImage(*CoreObjects::logicalDeviceObj, (*it)->image, CoreObjects::pAllocator);
            (*it)->image = VK_NULL_HANDLE;
            count++;
        }

        if (destroyImageView[i] == true)
        {
            if ((*it)->imageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(*CoreObjects::logicalDeviceObj, (*it)->imageView, CoreObjects::pAllocator);
                (*it)->imageView = VK_NULL_HANDLE;
                count++;
            }
        }

        if (freeImageMemory[i] == true)
        {
            if ((*it)->imageMemory != VK_NULL_HANDLE)
            {
                //vkFreeMemory(*CoreObjects::logicalDeviceObj, (*it)->imageMemory, CoreObjects::pAllocator);
                VulkanMemoryManager::GetSingleton()->FreeMemory((*it)->memoryId);
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

void VkAttachmentFactory::FreeAttachmentMemory(uint32_t * imageIds, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == imageIds[i]; });
        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        VulkanMemoryManager::GetSingleton()->FreeMemory((*it)->memoryId);
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

void VkAttachmentFactory::DestroySwapChainImageViews(uint32_t * ids, const uint32_t & count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<AttachmentWrapper *>::iterator it;
        it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == ids[i]; });
        ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image id not found");

        vkDestroyImageView(*CoreObjects::logicalDeviceObj, (*it)->imageView, CoreObjects::pAllocator);
        
        delete (*it);
        attachmentList.erase(it);
    }
}


VkImageView VkAttachmentFactory::GetImageView(uint32_t id)
{
    std::vector<AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == id; });

    return (*it)->imageView;
}

VkImage * VkAttachmentFactory::GetImage(const uint32_t & id)
{
    std::vector<AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image not found");

    return &(*it)->image;
}

void VkAttachmentFactory::BindImageMemory(const uint32_t & imageId, const uint32_t & memId, const size_t & offset)
{
    std::vector<AttachmentWrapper *>::iterator it;
    it = std::find_if(attachmentList.begin(), attachmentList.end(), [&](AttachmentWrapper * e) { return e->id == imageId; });

    ASSERT_MSG_DEBUG(it != attachmentList.end(), "Image not found");

    (*it)->memoryId = memId;
    (*it)->imageMemory = *VulkanMemoryManager::GetSingleton()->GetDeviceMemory(memId);

    ErrorCheck( vkBindImageMemory(*CoreObjects::logicalDeviceObj, (*it)->image, (*it)->imageMemory, offset));
}

uint32_t VkAttachmentFactory::GetId()
{
    return imageId++;
}

void AttachmentWrapper::DeActivateAttachment()
{
    if (imageView != VK_NULL_HANDLE)
        vkDestroyImageView(*CoreObjects::logicalDeviceObj, imageView, CoreObjects::pAllocator);
    if (imageMemory != VK_NULL_HANDLE)
        vkFreeMemory(*CoreObjects::logicalDeviceObj, imageMemory, CoreObjects::pAllocator);
    if (image != VK_NULL_HANDLE)
        vkDestroyImage(*CoreObjects::logicalDeviceObj, image, CoreObjects::pAllocator);

    attachmentActive = false;
}

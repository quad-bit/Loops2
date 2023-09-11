#include "VkFrameBufferFactory.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"
#include <CorePrecompiled.h>

GfxVk::Framebuffer::VkFrameBufferFactory* GfxVk::Framebuffer::VkFrameBufferFactory::instance = nullptr;
uint32_t GfxVk::Framebuffer::VkFrameBufferFactory::fboId = 0;

uint32_t GfxVk::Framebuffer::VkFrameBufferFactory::GetId()
{
    return fboId++;
}

void GfxVk::Framebuffer::VkFrameBufferFactory::Init()
{
    PLOGD << "VkFrameBufferFactory Init";
}

void GfxVk::Framebuffer::VkFrameBufferFactory::DeInit()
{
    PLOGD << "VkFrameBufferFactory DeInit";

    uint32_t size = (uint32_t)fboList.size();
    for (uint32_t i = 0; i < size; i++)
    {
        delete fboList[i];
    }

    fboList.clear();
}

void GfxVk::Framebuffer::VkFrameBufferFactory::Update()
{

}

GfxVk::Framebuffer::VkFrameBufferFactory * GfxVk::Framebuffer::VkFrameBufferFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Framebuffer::VkFrameBufferFactory();
    }
    return instance;
}

GfxVk::Framebuffer::VkFrameBufferFactory::~VkFrameBufferFactory()
{
}

void GfxVk::Framebuffer::VkFrameBufferFactory::CreateFrameBuffer(uint32_t numFrameBuffers, VkImageView * pImageView, 
    uint32_t viewsPerFB, VkRenderPass * renderPass, uint32_t width, uint32_t height, uint32_t * ids)
{

    for (uint32_t i = 0; i < numFrameBuffers; i++)
    {
        GfxVk::Framebuffer::FrameBufferWrapper * fboWrapper = new GfxVk::Framebuffer::FrameBufferWrapper;
        fboWrapper->id = GetId();
        ids[i] = fboWrapper->id;

        VkFramebufferCreateInfo frameBufferCreateInfo{};
        frameBufferCreateInfo.attachmentCount = viewsPerFB;
        frameBufferCreateInfo.height = height;
        frameBufferCreateInfo.layers = 1;
        frameBufferCreateInfo.pAttachments = &pImageView[i * viewsPerFB];
        frameBufferCreateInfo.renderPass = *renderPass;
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.width = width;

        GfxVk::Utility::ErrorCheck(vkCreateFramebuffer(DeviceInfo::GetLogicalDevice(), &frameBufferCreateInfo,
            DeviceInfo::GetAllocationCallback(), &fboWrapper->fbo));

        fboList.push_back(fboWrapper);
    }
}

void GfxVk::Framebuffer::VkFrameBufferFactory::DestroyFrameBuffer(uint32_t id)
{
    ASSERT_MSG_DEBUG(0, "yet to be done");
}

void GfxVk::Framebuffer::VkFrameBufferFactory::DestroyFrameBuffer(uint32_t * ids, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<GfxVk::Framebuffer::FrameBufferWrapper*>::iterator it;
        it = std::find_if(fboList.begin(), fboList.end(), [&](GfxVk::Framebuffer::FrameBufferWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != fboList.end(), "Image id not found");

        vkDestroyFramebuffer(DeviceInfo::GetLogicalDevice(), (*it)->fbo, DeviceInfo::GetAllocationCallback());
    }
}

VkFramebuffer * GfxVk::Framebuffer::VkFrameBufferFactory::GetFrameBuffer(uint32_t id)
{
    std::vector<GfxVk::Framebuffer::FrameBufferWrapper*>::iterator it;
    it = std::find_if(fboList.begin(), fboList.end(), [&](GfxVk::Framebuffer::FrameBufferWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != fboList.end(), "Image id not found");

    return &(*it)->fbo;
} 


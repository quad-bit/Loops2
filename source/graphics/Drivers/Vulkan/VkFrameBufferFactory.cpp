#include "VkFrameBufferFactory.h"
#include "VulkanUtility.h"
#include "VulkanSettings.h"
#include <CorePrecompiled.h>

VkFrameBufferFactory* VkFrameBufferFactory::instance = nullptr;
uint32_t VkFrameBufferFactory::fboId = 0;

uint32_t VkFrameBufferFactory::GetId()
{
    return fboId++;
}

void VkFrameBufferFactory::Init()
{
    PLOGD << "VkFrameBufferFactory Init";
}

void VkFrameBufferFactory::DeInit()
{
    PLOGD << "VkFrameBufferFactory DeInit";

    uint32_t size = (uint32_t)fboList.size();
    for (uint32_t i = 0; i < size; i++)
    {
        delete fboList[i];
    }

    fboList.clear();
}

void VkFrameBufferFactory::Update()
{

}

VkFrameBufferFactory * VkFrameBufferFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkFrameBufferFactory();
    }
    return instance;
}

VkFrameBufferFactory::~VkFrameBufferFactory()
{
}

void VkFrameBufferFactory::CreateFrameBuffer(uint32_t numFrameBuffers, VkImageView * pImageView, 
    uint32_t viewsPerFB, VkRenderPass * renderPass, uint32_t width, uint32_t height, uint32_t * ids)
{

    for (uint32_t i = 0; i < numFrameBuffers; i++)
    {
        FrameBufferWrapper * fboWrapper = new FrameBufferWrapper;
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

        ErrorCheck(vkCreateFramebuffer(*CoreObjects::logicalDeviceObj, &frameBufferCreateInfo, 
            CoreObjects::pAllocator, &fboWrapper->fbo));

        fboList.push_back(fboWrapper);
    }
}

void VkFrameBufferFactory::DestroyFrameBuffer(uint32_t id)
{
    ASSERT_MSG_DEBUG(0, "yet to be done");
}

void VkFrameBufferFactory::DestroyFrameBuffer(uint32_t * ids, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::vector<FrameBufferWrapper*>::iterator it;
        it = std::find_if(fboList.begin(), fboList.end(), [&](FrameBufferWrapper * e) { return e->id == ids[i]; });

        ASSERT_MSG_DEBUG(it != fboList.end(), "Image id not found");

        vkDestroyFramebuffer(*CoreObjects::logicalDeviceObj, (*it)->fbo, CoreObjects::pAllocator);
    }
}

VkFramebuffer * VkFrameBufferFactory::GetFrameBuffer(uint32_t id)
{
    std::vector<FrameBufferWrapper*>::iterator it;
    it = std::find_if(fboList.begin(), fboList.end(), [&](FrameBufferWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != fboList.end(), "Image id not found");

    return &(*it)->fbo;
} 


#include "VkRenderPassFactory.h"
#include "VkRenderingUnwrapper.h"
#include "VulkanUtility.h"
#include <CorePrecompiled.h>
#include <array>
#include "Settings.h"
#include "RendererSettings.h"

VkRenderPassFactory* VkRenderPassFactory::instance = nullptr;
uint32_t VkRenderPassFactory::renderpassIdCounter = 0;

uint32_t VkRenderPassFactory::GetId()
{
    uint32_t id = renderpassIdCounter;
    renderpassIdCounter += 1;
    return id;
}

void VkRenderPassFactory::Init()
{
    PLOGD << "VkRenderPassFactory init";
    refCounter = 0;
}

void VkRenderPassFactory::DeInit()
{
    PLOGD << "VkRenderPassFactory Deinit";

    for (uint32_t i = 0; i < renderpassList.size(); i++)
    {
        if (renderpassList[i]->clearValue != nullptr)
            delete renderpassList[i]->clearValue;

        delete renderpassList[i];
    }
    renderpassList.clear();
}

void VkRenderPassFactory::Update()
{

}

VkRenderPassFactory * VkRenderPassFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkRenderPassFactory();
    }
    return instance;
}

VkRenderPassFactory::~VkRenderPassFactory()
{

}

void VkRenderPassFactory::CreateRenderPass(const VkAttachmentDescription * renderpassAttachmentList, 
    const uint32_t & attachmentCount, const VkSubpassDescription * subpassList, const uint32_t & subpassCount,
    const VkSubpassDependency * dependencyList, const uint32_t & dependencyCount, uint32_t & renderPassId)
{
    RenderpassWrapper * info = new RenderpassWrapper();
    info->id = GetId();

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.attachmentCount = attachmentCount;
    renderPassCreateInfo.dependencyCount = dependencyCount;
    renderPassCreateInfo.pAttachments = renderpassAttachmentList;
    renderPassCreateInfo.pDependencies = dependencyList;
    renderPassCreateInfo.pSubpasses = subpassList;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.subpassCount = subpassCount;

    ErrorCheck(vkCreateRenderPass(*CoreObjects::logicalDeviceObj, &renderPassCreateInfo, CoreObjects::pAllocator, &info->renderPass));

    renderpassList.push_back(info);
    renderPassId = info->id;
    
    VkClearValue * clearValue;
    if (RendererSettings::MSAA_Enabled && RendererSettings::multiSamplingAvailable)
    {
        clearValue = new VkClearValue[3];
        clearValue[0].color.float32[0] = Settings::clearColorValue[0];
        clearValue[0].color.float32[1] = Settings::clearColorValue[1];
        clearValue[0].color.float32[2] = Settings::clearColorValue[2];
        clearValue[0].color.float32[3] = Settings::clearColorValue[3];

        clearValue[1].color.float32[0] = Settings::clearColorValue[0];
        clearValue[1].color.float32[1] = Settings::clearColorValue[1];
        clearValue[1].color.float32[2] = Settings::clearColorValue[2];
        clearValue[1].color.float32[3] = Settings::clearColorValue[3];

        clearValue[2].depthStencil.depth = Settings::depthClearValue;
        clearValue[2].depthStencil.stencil = (uint32_t)Settings::stencilClearValue;

        // float32 is chosen because VK_FORMAT_B8G8R8A8_UNORM is preferred to be the format UNORM is unsigned normalised which is floating point
        // the type float32 / int / uint should be selected based on the format
        info->clearValue = clearValue;
        info->clearValueCount = 3;
    }
    else
    {
        clearValue = new VkClearValue[2];

        clearValue[0].color.float32[0] = Settings::clearColorValue[0];
        clearValue[0].color.float32[1] = Settings::clearColorValue[1];
        clearValue[0].color.float32[2] = Settings::clearColorValue[2];
        clearValue[0].color.float32[3] = Settings::clearColorValue[3];

        clearValue[1].depthStencil.depth = Settings::depthClearValue;
        clearValue[1].depthStencil.stencil = (uint32_t)Settings::stencilClearValue;

        // float32 is chosen because VK_FORMAT_B8G8R8A8_UNORM is preferred to be the format UNORM is unsigned normalised which is floating point
        // the type float32 / int / uint should be selected based on the format
        info->clearValue = clearValue;
        info->clearValueCount = 2;
    }
    
}

void VkRenderPassFactory::CreateRenderPass(const VkAttachmentDescription * renderpassAttachmentList, 
    const uint32_t & attachmentCount, const VkSubpassDescription * subpassList, const uint32_t &
    subpassCount, const VkSubpassDependency * dependencyList, 
    const uint32_t & dependencyCount, const VkRenderPassBeginInfo beginInfo, uint32_t & renderPassId)
{
    DEPRECATED;
    RenderpassWrapper * info = new RenderpassWrapper();
    info->id = GetId();

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.attachmentCount = attachmentCount;
    renderPassCreateInfo.dependencyCount = dependencyCount;
    renderPassCreateInfo.pAttachments = renderpassAttachmentList;
    renderPassCreateInfo.pDependencies = nullptr;
    renderPassCreateInfo.pSubpasses = subpassList;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.subpassCount = subpassCount;

    ErrorCheck(vkCreateRenderPass(*CoreObjects::logicalDeviceObj, &renderPassCreateInfo, CoreObjects::pAllocator, &info->renderPass));
    info->beginInfo = beginInfo;

    renderpassList.push_back(info);
    renderpassIdCounter = info->id;

    info->clearValue[1].depthStencil.depth = Settings::depthClearValue;
    info->clearValue[1].depthStencil.stencil = (uint32_t)Settings::stencilClearValue;

    // float32 is chosen because VK_FORMAT_B8G8R8A8_UNORM is preferred to be the format UNORM is unsigned normalised which is floating point
    // the type float32 / int / uint should be selected based on the format
    info->clearValue[0].color.float32[0] = Settings::clearColorValue[0];
    info->clearValue[0].color.float32[1] = Settings::clearColorValue[1];
    info->clearValue[0].color.float32[2] = Settings::clearColorValue[2];
    info->clearValue[0].color.float32[3] = Settings::clearColorValue[3];
}

void VkRenderPassFactory::SetRenderPassBeginInfo(const VkRenderPassBeginInfo beginInfo, const uint32_t & renderpassId)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    (*it)->beginInfo = beginInfo;
    (*it)->beginInfo.clearValueCount = (uint32_t)(*it)->clearValueCount;
    (*it)->beginInfo.pClearValues = (*it)->clearValue;
}

void VkRenderPassFactory::DestroyRenderPass(const uint32_t & id)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    vkDestroyRenderPass(*CoreObjects::logicalDeviceObj, (*it)->renderPass, CoreObjects::pAllocator);
}

VkRenderPass * VkRenderPassFactory::GetRenderPass(const uint32_t & id)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return &(*it)->renderPass;
}

VkClearValue * VkRenderPassFactory::GetClearValue(const uint32_t & renderpassId)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return (*it)->clearValue;
}

uint32_t VkRenderPassFactory::GetClearValueCount(const uint32_t & renderpassId)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return (*it)->clearValueCount;
}

void VkRenderPassFactory::SetClearColor(std::vector<VkClearValue> clearValue, const uint32_t & renderPassId)
{
    std::vector<RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](RenderpassWrapper * e) { return e->id == renderPassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    (*it)->clearValue = clearValue.data();
    (*it)->clearValueCount = (uint32_t)clearValue.size();
}

#include "VkRenderPassFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include "utility/VulkanUtility.h"
#include <CorePrecompiled.h>
#include <array>
#include "Settings.h"
#include "RendererSettings.h"

GfxVk::Renderpass::VkRenderPassFactory* GfxVk::Renderpass::VkRenderPassFactory::instance = nullptr;
uint32_t GfxVk::Renderpass::VkRenderPassFactory::renderpassIdCounter = 0;

uint32_t GfxVk::Renderpass::VkRenderPassFactory::GetId()
{
    uint32_t id = renderpassIdCounter;
    renderpassIdCounter += 1;
    return id;
}

void GfxVk::Renderpass::VkRenderPassFactory::Init()
{
    PLOGD << "GfxVk::Renderpass::VkRenderPassFactory init";
    refCounter = 0;
}

void GfxVk::Renderpass::VkRenderPassFactory::DeInit()
{
    PLOGD << "GfxVk::Renderpass::VkRenderPassFactory Deinit";

    for (uint32_t i = 0; i < renderpassList.size(); i++)
    {
        if (renderpassList[i]->clearValue != nullptr)
            delete renderpassList[i]->clearValue;

        delete renderpassList[i];
    }
    renderpassList.clear();
}

void GfxVk::Renderpass::VkRenderPassFactory::Update()
{

}

GfxVk::Renderpass::VkRenderPassFactory * GfxVk::Renderpass::VkRenderPassFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Renderpass::VkRenderPassFactory();
    }
    return instance;
}

GfxVk::Renderpass::VkRenderPassFactory::~VkRenderPassFactory()
{

}

void GfxVk::Renderpass::VkRenderPassFactory::CreateRenderPass(const VkAttachmentDescription * renderpassAttachmentList, 
    const uint32_t & attachmentCount, const VkSubpassDescription * subpassList, const uint32_t & subpassCount,
    const VkSubpassDependency * dependencyList, const uint32_t & dependencyCount, uint32_t & renderPassId)
{
    /*
    GfxVk::Renderpass::RenderpassWrapper * info = new GfxVk::Renderpass::RenderpassWrapper();
    info->id = GetId();

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.attachmentCount = attachmentCount;
    renderPassCreateInfo.dependencyCount = dependencyCount;
    renderPassCreateInfo.pAttachments = renderpassAttachmentList;
    renderPassCreateInfo.pDependencies = dependencyList;
    renderPassCreateInfo.pSubpasses = subpassList;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.subpassCount = subpassCount;

    GfxVk::Utility::ErrorCheck(vkCreateRenderPass(DeviceInfo::GetLogicalDevice(), &renderPassCreateInfo,DeviceInfo::GetAllocationCallback(), &info->renderPass));

    renderpassList.push_back(info);
    renderPassId = info->id;
    
    VkClearValue * clearValue;
    if (Core::RendererSettings::MSAA_Enabled && Core::RendererSettings::multiSamplingAvailable)
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
*/
}

void GfxVk::Renderpass::VkRenderPassFactory::CreateRenderPass(const VkAttachmentDescription * renderpassAttachmentList, 
    const uint32_t & attachmentCount, const VkSubpassDescription * subpassList, const uint32_t &
    subpassCount, const VkSubpassDependency * dependencyList, 
    const uint32_t & dependencyCount, const VkRenderPassBeginInfo beginInfo, uint32_t & renderPassId)
{
    DEPRECATED;
    /*
    GfxVk::Renderpass::RenderpassWrapper * info = new GfxVk::Renderpass::RenderpassWrapper();
    info->id = GetId();

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.attachmentCount = attachmentCount;
    renderPassCreateInfo.dependencyCount = dependencyCount;
    renderPassCreateInfo.pAttachments = renderpassAttachmentList;
    renderPassCreateInfo.pDependencies = nullptr;
    renderPassCreateInfo.pSubpasses = subpassList;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.subpassCount = subpassCount;

    GfxVk::Utility::ErrorCheck(vkCreateRenderPass(DeviceInfo::GetLogicalDevice(), &renderPassCreateInfo,DeviceInfo::GetAllocationCallback(), &info->renderPass));
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
    */
}

void GfxVk::Renderpass::VkRenderPassFactory::SetRenderPassBeginInfo(const VkRenderPassBeginInfo beginInfo, const uint32_t & renderpassId)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    (*it)->beginInfo = beginInfo;
    (*it)->beginInfo.clearValueCount = (uint32_t)(*it)->clearValueCount;
    (*it)->beginInfo.pClearValues = (*it)->clearValue;
}

void GfxVk::Renderpass::VkRenderPassFactory::DestroyRenderPass(const uint32_t & id)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    vkDestroyRenderPass(DeviceInfo::GetLogicalDevice(), (*it)->renderPass,DeviceInfo::GetAllocationCallback());
}

VkRenderPass * GfxVk::Renderpass::VkRenderPassFactory::GetRenderPass(const uint32_t & id)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return &(*it)->renderPass;
}

VkClearValue * GfxVk::Renderpass::VkRenderPassFactory::GetClearValue(const uint32_t & renderpassId)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return (*it)->clearValue;
}

uint32_t GfxVk::Renderpass::VkRenderPassFactory::GetClearValueCount(const uint32_t & renderpassId)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == renderpassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    return (*it)->clearValueCount;
}

void GfxVk::Renderpass::VkRenderPassFactory::SetClearColor(std::vector<VkClearValue> clearValue, const uint32_t & renderPassId)
{
    std::vector<GfxVk::Renderpass::RenderpassWrapper*>::iterator it;
    it = std::find_if(renderpassList.begin(), renderpassList.end(), [&](GfxVk::Renderpass::RenderpassWrapper * e) { return e->id == renderPassId; });

    ASSERT_MSG_DEBUG(it != renderpassList.end(), "Image id not found");

    (*it)->clearValue = clearValue.data();
    (*it)->clearValueCount = (uint32_t)clearValue.size();
}

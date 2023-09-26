#include "RenderingManager.h"
#include "RendererSettings.h"

//#include "ForwardInterface.h"
//#include "CommandBufferManager.h"
//#include "MeshFactory.h"
//#include "GraphicsPipelineManager.h"
//#include "ShaderFactory.h"
//#include "MaterialFactory.h"
//#include "UniformFactory.h"
//#include "DrawGraphManager.h"
//#include "DrawGraphNode.h"
//#include "ForwardDrawGraph.h"

#include "VulkanInterface.h"
#include "VulkanManager.h"
#include "utility/VulkanUtility.h"
#include <PresentationEngine.h>

uint32_t Renderer::RendererSettings::m_renderQueueId;
uint32_t Renderer::RendererSettings::m_presentationQueueId;
uint32_t Renderer::RendererSettings::m_computeQueueId;
uint32_t Renderer::RendererSettings::m_transferQueueId;

std::vector<Core::Wrappers::QueueWrapper> Renderer::RendererSettings::m_queueReq;

bool Renderer::RendererSettings::m_msaaEnabled;
bool Renderer::RendererSettings::m_sampleRateShadingEnabled;
bool Renderer::RendererSettings::m_multiSamplingAvailable;

Core::Enums::Samples Renderer::RendererSettings::m_maxSampleCountAvailable;

std::vector<uint32_t> Renderer::RendererSettings::depthPrepassImageId;
uint32_t Renderer::RendererSettings::m_shadowMapWidth, Renderer::RendererSettings::m_shadowMapHeight;

Core::Enums::Format Renderer::RendererSettings::m_bestDepthFormat;

uint32_t Renderer::RendererSettings::m_swapBufferCount;
uint32_t Renderer::RendererSettings::m_maxFramesInFlight, Renderer::RendererSettings::m_currentFrameInFlight;
uint32_t Renderer::RendererSettings::m_maxFrameRate;

void Renderer::RenderingManager::BeginRenderLoop()
{
    //get swapchain image index to be used for rendering the current frame.
    /*
    m_currentFenceId = m_getSwapChainImageFences[m_currentFrameIndex];
    m_currentRenderSemaphoreId = m_renderSemaphores[m_currentFrameIndex];
    m_currentPresentationSemaphoreId = m_presentationSemaphores[m_currentFrameIndex];

    m_currentSwapchainIndex = apiInterface->GetAvailableSwapchainIndex(m_currentFenceId, m_currentRenderSemaphoreId);
    Settings::currentSwapChainIndex = m_currentSwapchainIndex;
    
    activeDrawCommandBuffer = drawCommandBufferList[m_currentSwapchainIndex];

    DrawGraphNode::dcb = activeDrawCommandBuffer;

    CommandBufferManager<T>::GetInstance()->ResetDrawCommandBuffer(activeDrawCommandBuffer);

    CommandBufferUsage usage{ CommandBufferUsage::USAGE_ONE_TIME_SUBMIT_BIT };
    CommandBufferManager<T>::GetInstance()->BeginDrawCommandBufferRecording(activeDrawCommandBuffer,
        &usage, nullptr);

    forwardRenderer->BeginRender(activeDrawCommandBuffer, m_currentSwapchainIndex);*/
}

void Renderer::RenderingManager::EndRenderLoop()
{
    //forwardRenderer->EndRender(activeDrawCommandBuffer);
    //CommandBufferManager<T>::GetInstance()->EndDrawCommandBufferRecording(activeDrawCommandBuffer);

    //SubmitInfo info = {};
    //PipelineType queueType = PipelineType::GRAPHICS;
    //QueuePurpose purpose = QueuePurpose::RENDER;

    //uint32_t id = activeDrawCommandBuffer->GetId();

    //info.commandBufferCount = 1;
    //info.commandBufferIds = &id;
    //info.pipelineStage = PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT;
    //info.purpose = &purpose;
    //info.signalSemaphoreCount = 1;
    //info.signalSemaphoreIds = &m_presentationSemaphores[m_currentFrameIndex];
    //info.waitSemaphoreCount = 1;
    //info.waitSemaphoreIds = &m_renderSemaphores[m_currentFrameIndex];

    //apiInterface->SubmitJob(&Renderer::RendererSettings::m_queueReq[0], &info, 1, m_getSwapChainImageFences[m_currentFrameIndex]);

    //// submit for presentation
    //PresentInfo presentInfo = {};
    //presentInfo.waitSemaphoreCount = 1;
    //presentInfo.pWaitSemaphoreIds = &m_presentationSemaphores[m_currentFrameIndex];
    //presentInfo.pImageIndices = &m_currentSwapchainIndex;

    ////TODO : send the correct presentation queue id, DONE.
    //apiInterface->PresentSwapchainImage(&Renderer::RendererSettings::m_queueReq[1], &presentInfo, 0);

    //m_currentFrameIndex = (m_currentFrameIndex + 1) % Settings::maxFramesInFlight;
    //Settings::currentFrameInFlight = m_currentFrameIndex;
}

void Renderer::RenderingManager::RenderLoop()
{
    //forwardRenderer->Render(activeDrawCommandBuffer, m_currentSwapchainIndex);
}

void Renderer::RenderingManager::CheckForMSAA()
{
    if (Renderer::RendererSettings::m_msaaEnabled)
    {
        Renderer::RendererSettings::m_maxSampleCountAvailable = VulkanInterfaceAlias::GetMaxUsableSampleCount();

        ASSERT_MSG_DEBUG(Renderer::RendererSettings::m_maxSampleCountAvailable != Core::Enums::Samples::SAMPLE_COUNT_1_BIT, "MSAA not available");


        if (Renderer::RendererSettings::m_maxSampleCountAvailable != Core::Enums::Samples::SAMPLE_COUNT_1_BIT)
        {
            if (VulkanInterfaceAlias::IsSampleRateShadingAvailable())
            {
                Renderer::RendererSettings::m_sampleRateShadingEnabled = true;
            }

            Renderer::RendererSettings::m_multiSamplingAvailable = true;
            if (Renderer::RendererSettings::m_maxSampleCountAvailable < m_desiredSampleCountForMSAA)
            {

            }
            else if (Renderer::RendererSettings::m_maxSampleCountAvailable > m_desiredSampleCountForMSAA)
            {
                Renderer::RendererSettings::m_maxSampleCountAvailable = m_desiredSampleCountForMSAA;
            }
        }
        else
        {
            Renderer::RendererSettings::m_multiSamplingAvailable = false;
        }
    }
    else
    {
        Renderer::RendererSettings::m_multiSamplingAvailable = false;
        Renderer::RendererSettings::m_maxSampleCountAvailable = Core::Enums::Samples::SAMPLE_COUNT_1_BIT;
    }
}

void Renderer::RenderingManager::Init(GLFWwindow* window)
{
    PLOGD << "Rendering interface Init";

    Renderer::RendererSettings::m_shadowMapHeight = 2048;
    Renderer::RendererSettings::m_shadowMapWidth = 2048;
    Renderer::RendererSettings::m_msaaEnabled = true;

    Renderer::RendererSettings::m_queueReq.resize(4);

    Renderer::RendererSettings::m_queueReq[0].purpose = Core::Enums::QueueType::RENDER;
    Renderer::RendererSettings::m_queueReq[0].queueType = Core::Enums::PipelineType::GRAPHICS;

    Renderer::RendererSettings::m_queueReq[1].purpose = Core::Enums::QueueType::PRESENT;
    Renderer::RendererSettings::m_queueReq[1].queueType = Core::Enums::PipelineType::GRAPHICS;

    Renderer::RendererSettings::m_queueReq[2].purpose = Core::Enums::QueueType::COMPUTE;
    Renderer::RendererSettings::m_queueReq[2].queueType = Core::Enums::PipelineType::COMPUTE;

    Renderer::RendererSettings::m_queueReq[3].purpose = Core::Enums::QueueType::TRANSFER;
    Renderer::RendererSettings::m_queueReq[3].queueType = Core::Enums::PipelineType::TRANSFER;

    m_vulkanMngrObj->Init(Renderer::RendererSettings::m_queueReq,
        Renderer::RendererSettings::m_renderQueueId,
        Renderer::RendererSettings::m_presentationQueueId,
        Renderer::RendererSettings::m_computeQueueId,
        Renderer::RendererSettings::m_transferQueueId);
    VkInstance obj = DeviceInfo::GetVkInstance();
    m_vulkanMngrObj->CreateSurface(window);

    GfxVk::Utility::PresentationEngine::GetInstance()->Init(GfxVk::Utility::VulkanDeviceInfo::GetSurface(), GfxVk::Utility::VulkanDeviceInfo::GetSurfaceFormat(), Renderer::RendererSettings::m_swapBufferCount);
    Renderer::RendererSettings::m_maxFramesInFlight = Renderer::RendererSettings::m_swapBufferCount - 1;

    {
        std::array<Core::Enums::Format, 5> depthFormats;
        depthFormats[0] = Core::Enums::Format::D32_SFLOAT_S8_UINT;
        depthFormats[1] = Core::Enums::Format::D24_UNORM_S8_UINT;
        depthFormats[2] = Core::Enums::Format::D16_UNORM_S8_UINT;
        depthFormats[3] = Core::Enums::Format::D32_SFLOAT;
        depthFormats[4] = Core::Enums::Format::D16_UNORM;

        int index = Renderer::Utility::VulkanInterface::FindBestDepthFormat(&depthFormats[0], 5);

        ASSERT_MSG_DEBUG(index != -1, "depth format not available");
        Renderer::RendererSettings::m_bestDepthFormat = depthFormats[index];
    }

    // swapchain / presentation setup
    {
        Core::Wrappers::ImageInfo info = {};
        info.colorSpace = Renderer::Utility::VulkanInterface::GetWindowColorSpace();
        info.format = Renderer::Utility::VulkanInterface::GetWindowSurfaceFormat();
        info.width = m_windowSettings.m_windowWidth;
        info.height = m_windowSettings.m_windowHeight;
        info.imageType = Core::Enums::ImageType::IMAGE_TYPE_2D;
        info.usage.push_back(Core::Enums::AttachmentUsage::USAGE_COLOR_ATTACHMENT_BIT);

        //Renderer::Utility::VulkanInterface::SetupPresentationEngine(info);
        GfxVk::Utility::PresentationEngine::GetInstance()->CreateSwapChain(info);
    }

    //forwardRenderer = new ForwardRendering<T>();
    //forwardRenderer->Init(apiInterface);

    //CheckForMSAA();

    /*CommandBufferManager<T>::GetInstance()->Init(apiInterface);
    MeshFactory::GetInstance()->Init(apiInterface);
    GraphicsPipelineManager<T>::GetInstance()->Init(apiInterface);
    MaterialFactory::GetInstance()->Init();
    ShaderFactory::GetInstance()->Init(apiInterface);
    UniformFactory::GetInstance()->Init(apiInterface);
    DrawGraphManager::GetInstance()->Init(RendererType::Forward);*/
    //this->apiInterface = apiInterface;

    //Settings::clearColorValue[0] = 34.0f / 256.0f; // Red
    //Settings::clearColorValue[1] = 30.0f / 256.0f;  // Green
    //Settings::clearColorValue[2] = 34.0f / 256.0f;  // Blue
    //Settings::clearColorValue[3] = 1.0f;

    //Settings::depthClearValue = 1.0f;
    //Settings::stencilClearValue = 0.0f;

    //Settings::currentFrameInFlight = m_currentFrameIndex;
}

void Renderer::RenderingManager::SetupRenderer()
{
    /*PipelineType pipelineType = PipelineType::GRAPHICS;
    CommandPoolProperty prop = CommandPoolProperty::TRANS_RESET;

    m_graphicCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);

    pipelineType = PipelineType::COMPUTE;
    m_computeCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);

    pipelineType = PipelineType::GRAPHICS;
    m_guiCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);*/

    //=============================================================================================

    //forwardRenderer->SetupRenderer();

    //drawCommandBufferList = new DrawCommandBuffer<T>*[Settings::swapBufferCount];

    /*for (uint32_t i = 0; i < Settings::swapBufferCount; i++)
    {
        CommandBufferLevel* level = new CommandBufferLevel;
        *level = CommandBufferLevel::PRIMARY;
        drawCommandBufferList[i] = CommandBufferManager<T>::GetInstance()->CreateDrawCommandBuffer(level, m_graphicCommandPoolId);
    }
    DrawGraphNode::dcb = drawCommandBufferList[0];*/

    /*Settings::maxFramesInFlight = Settings::swapBufferCount - 1;

    m_renderSemaphores = new uint32_t[Settings::maxFramesInFlight];
    m_presentationSemaphores = new uint32_t[Settings::maxFramesInFlight];
    m_getSwapChainImageFences = new uint32_t[Settings::maxFramesInFlight];

    for (uint32_t i = 0; i < Settings::maxFramesInFlight; i++)
    {
        m_renderSemaphores[i] = apiInterface->Create_Semaphore(false);
        m_presentationSemaphores[i] = apiInterface->Create_Semaphore(false);
        m_getSwapChainImageFences[i] = apiInterface->CreateFence(true);
    }*/
}

void Renderer::RenderingManager::DislogeRenderer()
{
    VulkanInterfaceAlias::IsApplicationSafeForClosure();

    /*for (uint32_t i = 0; i < Settings::maxFramesInFlight; i++)
    {
        apiInterface->DestroyFence(m_getSwapChainImageFences[i]);
        apiInterface->DestroySemaphore(m_renderSemaphores[i]);
        apiInterface->DestroySemaphore(m_presentationSemaphores[i]);
    }

    delete[] m_getSwapChainImageFences;
    delete[] m_presentationSemaphores;
    delete[] m_renderSemaphores;*/

    /*for (uint32_t i = 0; i < Settings::swapBufferCount; i++)
    {
        CommandBufferManager<T>::GetInstance()->DestroyDrawCommandBuffer(drawCommandBufferList[i]);
    }

    delete[] drawCommandBufferList;

    forwardRenderer->DislogeRenderer();*/

    /*CommandBufferManager<T>::GetInstance()->DestroyCommandPool(m_guiCommandPoolId);
    CommandBufferManager<T>::GetInstance()->DestroyCommandPool(m_computeCommandPoolId);
    CommandBufferManager<T>::GetInstance()->DestroyCommandPool(m_graphicCommandPoolId);*/
}

void Renderer::RenderingManager::DeInit()
{
    PLOGD << "Rendering interface DeInit";

    /*DrawGraphManager::GetInstance()->DeInit();
    delete DrawGraphManager::GetInstance();

    UniformFactory::GetInstance()->DeInit();
    delete UniformFactory::GetInstance();

    ShaderFactory::GetInstance()->DeInit();
    delete ShaderFactory::GetInstance();

    MaterialFactory::GetInstance()->DeInit();
    delete MaterialFactory::GetInstance();

    GraphicsPipelineManager<T>::GetInstance()->DeInit();
    delete GraphicsPipelineManager<T>::GetInstance();

    MeshFactory::GetInstance()->DeInit();
    delete MeshFactory::GetInstance();

    CommandBufferManager<T>::GetInstance()->DeInit();
    delete CommandBufferManager<T>::GetInstance();

    forwardRenderer->DeInit();
    delete forwardRenderer;*/

    GfxVk::Utility::PresentationEngine::GetInstance()->DeInit();

    m_vulkanMngrObj->DeInit();
}

void Renderer::RenderingManager::PreRender()
{
    //forwardRenderer->PreRender();
}

void Renderer::RenderingManager::Render()
{
    BeginRenderLoop();
    RenderLoop();
    EndRenderLoop();
}

void Renderer::RenderingManager::PostRenderLoopEnd()
{
    VulkanInterfaceAlias::IsApplicationSafeForClosure();
}

Renderer::RenderingManager::~RenderingManager()
{
    m_vulkanMngrObj.reset();
}

Renderer::RenderingManager::RenderingManager(const Core::WindowSettings& windowSettings/*, Core::Settings* settings*/):
    m_windowSettings(windowSettings)
{
    m_vulkanMngrObj = std::make_unique<GfxVk::Utility::VulkanManager>(windowSettings);
}

uint32_t Renderer::RendererSettings::GetRenderQueueId()
{
    return m_renderQueueId;
}

uint32_t Renderer::RendererSettings::GetPresentationQueueId()
{
    return m_presentationQueueId;
}

uint32_t Renderer::RendererSettings::GetComputeQueueId()
{
    return m_computeQueueId;
}

uint32_t Renderer::RendererSettings::GetTransferQueueId()
{
    return m_transferQueueId;
}

Core::Enums::Samples Renderer::RendererSettings::GetMaxSampleCountAvailable()
{
    return m_maxSampleCountAvailable;
}

bool Renderer::RendererSettings::IsMsaaEnabled()
{
    return m_msaaEnabled;
}

bool Renderer::RendererSettings::IsSampleRateShadingAvailable()
{
    return m_sampleRateShadingEnabled;
}

bool Renderer::RendererSettings::IsMultiSamplingAvailable()
{
    return m_multiSamplingAvailable;
}

const uint32_t& Renderer::RendererSettings::GetMaxFramesInFlightCount()
{
    return m_maxFramesInFlight;
}

const uint32_t& Renderer::RendererSettings::GetCurrentFrameIndex()
{
    return m_currentFrameInFlight;
}

const uint32_t& Renderer::RendererSettings::GetSwapBufferCount()
{
    return m_swapBufferCount;
}

const Core::Enums::Format& Renderer::RendererSettings::GetBestDepthFormat()
{
    return m_bestDepthFormat;
}

#if 0
inline void ForwardRendering<T>::SetupRenderer()
{
    //Samples sampleCount = CheckForMSAA();
    windowSurfaceFormat = apiInterface->GetWindowSurfaceFormat();
    windowColorSpace = apiInterface->GetWindowColorSpace();

    {
        Format* depthFormats = new Format[5];
        depthFormats[0] = Format::D32_SFLOAT_S8_UINT;
        depthFormats[1] = Format::D24_UNORM_S8_UINT;
        depthFormats[2] = Format::D16_UNORM_S8_UINT;
        depthFormats[3] = Format::D32_SFLOAT;
        depthFormats[4] = Format::D16_UNORM;

        int index = apiInterface->FindBestDepthFormat(depthFormats, 5);

        ASSERT_MSG_DEBUG(index != -1, "depth format not available");
        Renderer::RendererSettings::m_bestDepthFormat = depthFormats[index];
        delete[] depthFormats;
    }

    // swapchain / presentation setup
    {
        ImageInfo info = {};
        info.colorSpace = windowColorSpace;
        info.format = windowSurfaceFormat;
        info.width = Settings::windowWidth;
        info.height = Settings::windowHeight;
        info.imageType = ImageType::IMAGE_TYPE_2D;
        info.usage.push_back(AttachmentUsage::USAGE_COLOR_ATTACHMENT_BIT);

        apiInterface->SetupPresentationEngine(info);
    }

    uint32_t depthPrepassBufferingCount = Settings::swapBufferCount;

    //if (RendererSettings::MSAA_Enabled && multiSamplingAvailable)
    //{
    //}
    //else
    {
        // depth pre pass samplCount = 1
        {
            ImageInfo info;
            info.imageType = ImageType::IMAGE_TYPE_2D;
            info.format = bestDepthFormat;
            info.layers = 1;
            info.mips = 1;
            info.sampleCount = Samples::SAMPLE_COUNT_1_BIT;
            info.usage.push_back(AttachmentUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
            info.usage.push_back(AttachmentUsage::USAGE_SAMPLED_BIT);
            info.width = RendererSettings::shadowMapWidth;
            info.height = RendererSettings::shadowMapHeight;
            info.depth = 1;
            info.initialLayout = ImageLayout::LAYOUT_UNDEFINED;

            depthPrepassDefaultImageIdList.resize(depthPrepassBufferingCount);
            apiInterface->CreateAttachment(&info, depthPrepassBufferingCount, depthPrepassDefaultImageIdList.data());

            {
                // Get the image memory req
                MemoryRequirementInfo req = apiInterface->GetImageMemoryRequirement(depthPrepassDefaultImageIdList[0]);

                // Allocate the memory
                size_t allocationSize = req.size * depthPrepassBufferingCount;
                MemoryType userReq[1]{ MemoryType::DEVICE_LOCAL_BIT };
                depthResolveImageMemoryId = apiInterface->AllocateMemory(&req, &userReq[0], allocationSize);

                // Bind the memory to the image
                for (uint32_t i = 0; i < depthPrepassDefaultImageIdList.size(); i++)
                    apiInterface->BindImageMemory(depthPrepassDefaultImageIdList[i], depthResolveImageMemoryId, req.size * i);
            }

            // Create image View
            ImageViewInfo viewInfo = {};
            viewInfo.baseArrayLayer = 0;
            viewInfo.baseMipLevel = 0;
            viewInfo.components[0] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[1] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[2] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[3] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.format = bestDepthFormat;
            // TODO : need to fix this OR thing
            viewInfo.imageAspect = ImageAspectFlag::IMAGE_ASPECT_DEPTH_BIT;// | (stencilAvailable ? IMAGE_ASPECT_STENCIL_BIT : 0);
            viewInfo.layerCount = 1;
            viewInfo.levelCount = 1;
            viewInfo.viewType = ImageViewType::IMAGE_VIEW_TYPE_2D;

            {
                std::vector<ImageViewInfo> viewInfoList(depthPrepassBufferingCount, viewInfo);// , viewInfo, viewInfo };
                for (uint32_t i = 0; i < depthPrepassDefaultImageIdList.size(); i++)
                {
                    viewInfoList[i].imageId = depthPrepassDefaultImageIdList[i];
                }
                uint32_t count = (uint32_t)viewInfoList.size();
                apiInterface->CreateImageView(viewInfoList.data(), count);
            }

            RendererSettings::depthPrepassImageId = depthPrepassDefaultImageIdList;
        }

        // default render target setup
        {
            ImageInfo info;
            info.colorSpace = windowColorSpace;
            info.imageType = ImageType::IMAGE_TYPE_2D;
            info.format = windowSurfaceFormat;
            info.layers = 1;
            info.mips = 1;
            info.sampleCount = Samples::SAMPLE_COUNT_1_BIT;
            info.usage.push_back(AttachmentUsage::USAGE_COLOR_ATTACHMENT_BIT);
            info.height = Settings::windowHeight;
            info.width = Settings::windowWidth;
            info.depth = 1;
            info.initialLayout = ImageLayout::LAYOUT_UNDEFINED;

            ImageViewInfo viewInfo = {};
            viewInfo.baseArrayLayer = 0;
            viewInfo.baseMipLevel = 0;
            viewInfo.components[0] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[1] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[2] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[3] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.format = windowSurfaceFormat;
            viewInfo.imageAspect = ImageAspectFlag::IMAGE_ASPECT_COLOR_BIT;
            viewInfo.layerCount = 1;
            viewInfo.levelCount = 1;
            viewInfo.viewType = ImageViewType::IMAGE_VIEW_TYPE_2D;

            defaultRenderTargetList.resize(Settings::swapBufferCount);
            //apiInterface->CreateRenderTarget(info, Settings::swapBufferCount, true, &defaultRenderTargetList);
            apiInterface->CreateDefaultRenderTarget(info, viewInfo, Settings::swapBufferCount, defaultRenderTargetList.data());
        }

        // depth
        {
            ImageInfo info;
            info.imageType = ImageType::IMAGE_TYPE_2D;
            info.format = bestDepthFormat;
            info.layers = 1;
            info.mips = 1;
            info.sampleCount = Samples::SAMPLE_COUNT_1_BIT;
            info.usage.push_back(AttachmentUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
            info.width = Settings::windowWidth;
            info.height = Settings::windowHeight;
            info.depth = 1;
            info.initialLayout = ImageLayout::LAYOUT_UNDEFINED;
            bool stencilRequired = false;
            bool stencilAvailable = false;

            if (stencilRequired)
                if (info.format == Format::D32_SFLOAT_S8_UINT ||
                    info.format == Format::D24_UNORM_S8_UINT ||
                    info.format == Format::D16_UNORM_S8_UINT)
                    stencilAvailable = true;


            defaultDepthTargetList.resize(Settings::swapBufferCount);
            apiInterface->CreateAttachment(&info, Settings::swapBufferCount, defaultDepthTargetList.data());

            // Get the image memory req
            MemoryRequirementInfo req = apiInterface->GetImageMemoryRequirement(defaultDepthTargetList[0]);

            // Allocate the memory
            size_t allocationSize = req.size * Settings::swapBufferCount;
            MemoryType userReq[1]{ MemoryType::DEVICE_LOCAL_BIT };
            depthImageMemoryId = apiInterface->AllocateMemory(&req, &userReq[0], allocationSize);

            // Bind the memory to the image
            for (uint32_t i = 0; i < defaultDepthTargetList.size(); i++)
                apiInterface->BindImageMemory(defaultDepthTargetList[i], depthImageMemoryId, req.size * i);

            // Create image View
            ImageViewInfo viewInfo = {};
            viewInfo.baseArrayLayer = 0;
            viewInfo.baseMipLevel = 0;
            viewInfo.components[0] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[1] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[2] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components[3] = ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.format = bestDepthFormat;
            // TODO : need to fix this OR thing
            viewInfo.imageAspect = ImageAspectFlag::IMAGE_ASPECT_DEPTH_BIT;// | (stencilAvailable ? IMAGE_ASPECT_STENCIL_BIT : 0);
            viewInfo.layerCount = 1;
            viewInfo.levelCount = 1;
            viewInfo.viewType = ImageViewType::IMAGE_VIEW_TYPE_2D;

            std::vector<ImageViewInfo> viewInfoList{ viewInfo, viewInfo, viewInfo };

            for (uint32_t i = 0; i < defaultDepthTargetList.size(); i++)
            {
                viewInfoList[i].imageId = defaultDepthTargetList[i];
            }
            uint32_t count = (uint32_t)viewInfoList.size();
            apiInterface->CreateImageView(viewInfoList.data(), count);
        }

        if (RendererSettings::MSAA_Enabled && RendererSettings::multiSamplingAvailable)
        {
            SetupAttachmentsForMSAA(*RendererSettings::sampleCount);

            uint32_t imagesPerFbo = 4, numFbos = Settings::swapBufferCount;
            uint32_t* imageIds = new uint32_t[numFbos * imagesPerFbo];

            for (uint32_t i = 0, j = 0; i < numFbos * imagesPerFbo; i++, j++)
            {
                imageIds[i++] = msaaRenderTargetList[j]; // Multi sampled color
                imageIds[i++] = defaultRenderTargetList[j]; // default color
                imageIds[i++] = msaaDepthTargetList[j]; // MSAA depth
                imageIds[i] = defaultDepthTargetList[j]; // default depth
            }

            colorPassFbo.resize(numFbos);
            apiInterface->CreateFrameBuffer(numFbos, imageIds, imagesPerFbo, colorPassId,
                Settings::windowWidth, Settings::windowHeight, &colorPassFbo[0]);

            delete[] imageIds;
        }
        else
        {
            RenderPassAttachmentInfo attchmentDescList[2];

            attchmentDescList[0].finalLayout = ImageLayout::LAYOUT_PRESENT_SRC_KHR;
            attchmentDescList[0].format = Format::B8G8R8A8_UNORM;
            attchmentDescList[0].initialLayout = ImageLayout::LAYOUT_UNDEFINED;
            attchmentDescList[0].loadOp = LoadOperation::LOAD_OP_CLEAR;
            attchmentDescList[0].sampleCount = *RendererSettings::sampleCount;
            attchmentDescList[0].stencilLoadOp = LoadOperation::LOAD_OP_DONT_CARE;
            attchmentDescList[0].stencilLStoreOp = StoreOperation::STORE_OP_DONT_CARE;
            attchmentDescList[0].storeOp = StoreOperation::STORE_OP_STORE;

            attchmentDescList[1].finalLayout = ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attchmentDescList[1].format = bestDepthFormat;
            attchmentDescList[1].initialLayout = ImageLayout::LAYOUT_UNDEFINED;
            attchmentDescList[1].loadOp = LoadOperation::LOAD_OP_CLEAR;
            attchmentDescList[1].sampleCount = *RendererSettings::sampleCount;
            attchmentDescList[1].stencilLoadOp = LoadOperation::LOAD_OP_DONT_CARE;
            attchmentDescList[1].stencilLStoreOp = StoreOperation::STORE_OP_DONT_CARE;
            attchmentDescList[1].storeOp = StoreOperation::STORE_OP_DONT_CARE;

            AttachmentRef colorAttachmentRef;
            colorAttachmentRef.index = 0;
            colorAttachmentRef.layoutInSubPass = ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            AttachmentRef depthAttachmentRef;
            depthAttachmentRef.index = 1;
            depthAttachmentRef.layoutInSubPass = ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            SubpassInfo subpassInfo;
            subpassInfo.colorAttachmentCount = 1;
            subpassInfo.inputAttachmentCount = 0;
            subpassInfo.pColorAttachments = &colorAttachmentRef;
            subpassInfo.pDepthStencilAttachment = &depthAttachmentRef;
            subpassInfo.pInputAttachments = nullptr;
            subpassInfo.pResolveAttachments = nullptr;

            apiInterface->CreateRenderPass(
                attchmentDescList, 2,
                &subpassInfo, 1,
                nullptr, 0,
                colorPassId
            );

            uint32_t imagesPerFbo = 2, numFbos = Settings::swapBufferCount;
            uint32_t* imageIds = new uint32_t[numFbos * imagesPerFbo];

            for (uint32_t i = 0, j = 0; i < numFbos * imagesPerFbo; i++, j++)
            {
                imageIds[i++] = defaultRenderTargetList[j];
                imageIds[i] = defaultDepthTargetList[j];
            }

            colorPassFbo.resize(numFbos);
            apiInterface->CreateFrameBuffer(numFbos, imageIds, imagesPerFbo, colorPassId,
                Settings::windowWidth, Settings::windowHeight, &colorPassFbo[0]);

            delete[] imageIds;
        }

        // depth pre pass
        {
            RenderPassAttachmentInfo depthPrepassAttchmentDescList[1];
            depthPrepassAttchmentDescList[0].finalLayout = ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            depthPrepassAttchmentDescList[0].format = bestDepthFormat;
            depthPrepassAttchmentDescList[0].initialLayout = ImageLayout::LAYOUT_UNDEFINED;
            depthPrepassAttchmentDescList[0].loadOp = LoadOperation::LOAD_OP_CLEAR;
            depthPrepassAttchmentDescList[0].sampleCount = Samples::SAMPLE_COUNT_1_BIT;
            depthPrepassAttchmentDescList[0].stencilLoadOp = LoadOperation::LOAD_OP_DONT_CARE;
            depthPrepassAttchmentDescList[0].stencilLStoreOp = StoreOperation::STORE_OP_DONT_CARE;
            depthPrepassAttchmentDescList[0].storeOp = StoreOperation::STORE_OP_STORE;

            AttachmentRef depthPrepassAttachmentRef;
            depthPrepassAttachmentRef.index = 0;
            depthPrepassAttachmentRef.layoutInSubPass = ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            SubpassInfo subpassInfo;
            subpassInfo.colorAttachmentCount = 0;
            subpassInfo.inputAttachmentCount = 0;
            subpassInfo.pColorAttachments = nullptr;
            subpassInfo.pDepthStencilAttachment = &depthPrepassAttachmentRef;
            subpassInfo.pInputAttachments = nullptr;
            subpassInfo.pResolveAttachments = nullptr;

            SubpassDependency dependency[2];
            dependency[0].srcSubpass = -1;
            dependency[0].dstSubpass = 0;
            dependency[0].srcStageMask.push_back(PipelineStage::FRAGMENT_SHADER_BIT);
            dependency[0].dstStageMask.push_back(PipelineStage::EARLY_FRAGMENT_TESTS_BIT);
            dependency[0].srcAccessMask.push_back(AccessFlagBits::ACCESS_SHADER_READ_BIT);
            dependency[0].dstAccessMask.push_back(AccessFlagBits::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
            dependency[0].dependencyFlags.push_back(DependencyFlagBits::DEPENDENCY_BY_REGION_BIT);

            dependency[1].srcSubpass = 0;
            dependency[1].dstSubpass = -1;
            dependency[1].srcStageMask.push_back(PipelineStage::LATE_FRAGMENT_TESTS_BIT);
            dependency[1].dstStageMask.push_back(PipelineStage::FRAGMENT_SHADER_BIT);
            dependency[1].srcAccessMask.push_back(AccessFlagBits::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
            dependency[1].dstAccessMask.push_back(AccessFlagBits::ACCESS_SHADER_READ_BIT);
            dependency[1].dependencyFlags.push_back(DependencyFlagBits::DEPENDENCY_BY_REGION_BIT);

            apiInterface->CreateRenderPass(
                &depthPrepassAttchmentDescList[0], 1,
                &subpassInfo, 1,
                dependency, 2,
                depthPrePassId
            );

            uint32_t imagesPerFbo = 1, numFbos = Settings::swapBufferCount;
            uint32_t* imageIds = new uint32_t[numFbos * imagesPerFbo];

            for (uint32_t i = 0, j = 0; i < numFbos * imagesPerFbo; i++, j++)
            {
                imageIds[i] = depthPrepassDefaultImageIdList[j];
            }

            depthPassFbo.resize(numFbos);
            apiInterface->CreateFrameBuffer(numFbos, imageIds, imagesPerFbo, depthPrePassId,
                RendererSettings::shadowMapWidth, RendererSettings::shadowMapHeight, &depthPassFbo[0]);

            delete[] imageIds;
        }
    }
}
#endif

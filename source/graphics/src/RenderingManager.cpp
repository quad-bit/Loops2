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

void Renderer::RenderingManager::Init()
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

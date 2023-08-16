#pragma once
#include <Settings.h>
#include "RenderingWrapper.h"
#include <CorePrecompiled.h>

template <typename T>
class ForwardRendering;

template <typename T>
class DeferredRendering;

class VulkanInterface;
class DxInterface;

template <typename T>
class CommandBufferManager;

template <typename T>
class DrawCommandBuffer;

template <typename T>
class GraphicsPipelineManager;

template <typename T>
class RenderingInterface
{
private:
    ForwardRendering<T> * forwardRenderer;
    DrawCommandBuffer<T> ** drawCommandBufferList;
    uint32_t graphicCommandPoolId, computeCommandPoolId, guiCommandPoolId;

    //uint32_t maxFramesInFlight;

    uint32_t * renderSemaphores, *presentationSemaphores;
    uint32_t * getSwapChainImageFences;

    uint32_t currentFrameIndex{ 0 }, currentSwapchainIndex{ 0 };
    uint32_t currentRenderSemaphoreId, currentPresentationSemaphoreId, currentFenceId;

    void BeginRenderLoop();
    void EndRenderLoop();
    void RenderLoop();

    T * apiInterface;

    DrawCommandBuffer<T> * activeDrawCommandBuffer;
    void CheckForMSAA();
    Samples desiredSampleCountForMSAA = Samples::SAMPLE_COUNT_8_BIT;

public:
    void Init(T * apiInterface);
    void SetupRenderer();
    void DislogeRenderer();
    void DeInit();
    void PreRender();
    void Render();
    void PostRenderLoopEnd();
};

#include "ForwardInterface.h"
#include "CommandBufferManager.h"
#include "MeshFactory.h"
#include "GraphicsPipelineManager.h"
#include "ShaderFactory.h"
#include "MaterialFactory.h"
#include "UniformFactory.h"
#include "DrawGraphManager.h"
//#include "DrawGraphNode.h"
//#include "ForwardDrawGraph.h"

template<typename T>
inline void RenderingInterface<T>::BeginRenderLoop()
{
    //get swapchain image index to be used for rendering the current frame.

    currentFenceId = getSwapChainImageFences[currentFrameIndex];
    currentRenderSemaphoreId = renderSemaphores[currentFrameIndex];
    currentPresentationSemaphoreId = presentationSemaphores[currentFrameIndex];

    currentSwapchainIndex = apiInterface->GetAvailableSwapchainIndex(currentFenceId, currentRenderSemaphoreId);
    Settings::currentSwapChainIndex = currentSwapchainIndex;
    activeDrawCommandBuffer = drawCommandBufferList[currentSwapchainIndex];

    DrawGraphNode::dcb = activeDrawCommandBuffer;

    CommandBufferManager<T>::GetInstance()->ResetDrawCommandBuffer(activeDrawCommandBuffer);

    CommandBufferUsage usage { CommandBufferUsage::USAGE_ONE_TIME_SUBMIT_BIT };
    CommandBufferManager<T>::GetInstance()->BeginDrawCommandBufferRecording(activeDrawCommandBuffer,
     &usage, nullptr);

    forwardRenderer->BeginRender(activeDrawCommandBuffer, currentSwapchainIndex);
}

template<typename T>
inline void RenderingInterface<T>::EndRenderLoop()
{
    forwardRenderer->EndRender(activeDrawCommandBuffer);
    CommandBufferManager<T>::GetInstance()->EndDrawCommandBufferRecording(activeDrawCommandBuffer);

    SubmitInfo info = {};
    PipelineType queueType = PipelineType::GRAPHICS;
    QueuePurpose purpose = QueuePurpose::RENDER;

    uint32_t id = activeDrawCommandBuffer->GetId();

    info.commandBufferCount = 1;
    info.commandBufferIds = &id;
    info.pipelineStage = PipelineStage::COLOR_ATTACHMENT_OUTPUT_BIT;
    info.purpose = &purpose;
    info.signalSemaphoreCount = 1;
    info.signalSemaphoreIds = &presentationSemaphores[currentFrameIndex];
    info.waitSemaphoreCount = 1;
    info.waitSemaphoreIds = &renderSemaphores[currentFrameIndex];

    apiInterface->SubmitJob(&RendererSettings::queueReq[0], &info, 1, getSwapChainImageFences[currentFrameIndex]);

    // submit for presentation
    PresentInfo presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphoreIds = &presentationSemaphores[currentFrameIndex];
    presentInfo.pImageIndices = &currentSwapchainIndex;

    //TODO : send the correct presentation queue id, DONE.
    apiInterface->PresentSwapchainImage(&RendererSettings::queueReq[1], &presentInfo, 0);

    currentFrameIndex = (currentFrameIndex + 1) % Settings::maxFramesInFlight;
    Settings::currentFrameInFlight = currentFrameIndex;
}

template<typename T>
inline void RenderingInterface<T>::RenderLoop()
{
    forwardRenderer->Render(activeDrawCommandBuffer, currentSwapchainIndex);
}

template<typename T>
inline void RenderingInterface<T>::CheckForMSAA()
{
    //Samples sampleCount;

    RendererSettings::sampleCount = new Samples;
    if (RendererSettings::MSAA_Enabled)
    {
        *RendererSettings::sampleCount = apiInterface->GetMaxUsableSampleCount();

        ASSERT_MSG_DEBUG(*RendererSettings::sampleCount != Samples::SAMPLE_COUNT_1_BIT, "MSAA not available");


        if (*RendererSettings::sampleCount != Samples::SAMPLE_COUNT_1_BIT)
        {
            if (apiInterface->IsSampleRateShadingAvailable())
            {
                RendererSettings::sampleRateShadingEnabled = true;
            }

            RendererSettings::multiSamplingAvailable= true;
            if (*RendererSettings::sampleCount < desiredSampleCountForMSAA)
            {

            }
            else if (*RendererSettings::sampleCount > desiredSampleCountForMSAA)
            {
                *RendererSettings::sampleCount = desiredSampleCountForMSAA;
            }
        }
        else
        {
            RendererSettings::multiSamplingAvailable = false;
        }
    }
    else
    {
        RendererSettings::multiSamplingAvailable = false;
        *RendererSettings::sampleCount = Samples::SAMPLE_COUNT_1_BIT;
    }
}

template<typename T>
inline void RenderingInterface<T>::Init(T * apiInterface)
{
    PLOGD << "Rendering interface Init";


    RendererSettings::shadowMapHeight = 2048;
    RendererSettings::shadowMapWidth = 2048;
    RendererSettings::MSAA_Enabled = true;

    forwardRenderer = new ForwardRendering<T>();
    forwardRenderer->Init(apiInterface);
    
    CheckForMSAA();

    CommandBufferManager<T>::GetInstance()->Init(apiInterface);
    MeshFactory::GetInstance()->Init(apiInterface);
    GraphicsPipelineManager<T>::GetInstance()->Init(apiInterface);
    MaterialFactory::GetInstance()->Init();
    ShaderFactory::GetInstance()->Init(apiInterface);
    UniformFactory::GetInstance()->Init(apiInterface);
    DrawGraphManager::GetInstance()->Init(RendererType::Forward);
    this->apiInterface = apiInterface;

    Settings::clearColorValue[0] = 34.0f / 256.0f; // Red
    Settings::clearColorValue[1] = 30.0f / 256.0f;  // Green
    Settings::clearColorValue[2] = 34.0f / 256.0f;  // Blue
    Settings::clearColorValue[3] = 1.0f;

    Settings::depthClearValue = 1.0f;
    Settings::stencilClearValue = 0.0f;

    Settings::currentFrameInFlight = currentFrameIndex;
    
}

template<typename T>
inline void RenderingInterface<T>::SetupRenderer()
{
    PipelineType pipelineType = PipelineType::GRAPHICS;
    CommandPoolProperty prop = CommandPoolProperty::TRANS_RESET;

    graphicCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);
    
    pipelineType = PipelineType::COMPUTE;
    computeCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);

    pipelineType = PipelineType::GRAPHICS;
    guiCommandPoolId = CommandBufferManager<T>::GetInstance()->CreateCommandPool(&pipelineType, &prop);
    
    forwardRenderer->SetupRenderer();

    drawCommandBufferList = new DrawCommandBuffer<T>*[Settings::swapBufferCount];

    for (uint32_t i = 0; i < Settings::swapBufferCount; i++)
    {
        CommandBufferLevel * level = new CommandBufferLevel;
        *level = CommandBufferLevel::PRIMARY;
        drawCommandBufferList[i] = CommandBufferManager<T>::GetInstance()->CreateDrawCommandBuffer(level, graphicCommandPoolId);
    }
    DrawGraphNode::dcb = drawCommandBufferList[0];

    Settings::maxFramesInFlight = Settings::swapBufferCount - 1;

    renderSemaphores = new uint32_t[Settings::maxFramesInFlight];
    presentationSemaphores = new uint32_t[Settings::maxFramesInFlight];
    getSwapChainImageFences = new uint32_t[Settings::maxFramesInFlight];

    for (uint32_t i = 0; i < Settings::maxFramesInFlight; i++)
    {
        renderSemaphores[i] = apiInterface->Create_Semaphore(false);
        presentationSemaphores[i] = apiInterface->Create_Semaphore(false);
        getSwapChainImageFences[i] = apiInterface->CreateFence(true);
    }
}

template<typename T>
inline void RenderingInterface<T>::DislogeRenderer()
{
    apiInterface->IsApplicationSafeForClosure();

    for (uint32_t i = 0; i < Settings::maxFramesInFlight; i++)
    {
        apiInterface->DestroyFence(getSwapChainImageFences[i]);
        apiInterface->DestroySemaphore(renderSemaphores[i]);
        apiInterface->DestroySemaphore(presentationSemaphores[i]);
    }

    delete[] getSwapChainImageFences;
    delete[] presentationSemaphores;
    delete[] renderSemaphores;

    for (uint32_t i = 0; i < Settings::swapBufferCount; i++)
    {
        CommandBufferManager<T>::GetInstance()->DestroyDrawCommandBuffer(drawCommandBufferList[i]);
    }

    delete[] drawCommandBufferList;

    forwardRenderer->DislogeRenderer();

    CommandBufferManager<T>::GetInstance()->DestroyCommandPool(guiCommandPoolId);
    CommandBufferManager<T>::GetInstance()->DestroyCommandPool(computeCommandPoolId);
    CommandBufferManager<T>::GetInstance()->DestroyCommandPool(graphicCommandPoolId);
}

template<typename T>
inline void RenderingInterface<T>::DeInit()
{
    PLOGD << "Rendering interface DeInit";
    
    DrawGraphManager::GetInstance()->DeInit();
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
    delete forwardRenderer;
}

template<typename T>
inline void RenderingInterface<T>::PreRender()
{
    forwardRenderer->PreRender();
}

template<typename T>
inline void RenderingInterface<T>::Render()
{
    BeginRenderLoop();
    RenderLoop();
    EndRenderLoop();
}

template<typename T>
inline void RenderingInterface<T>::PostRenderLoopEnd()
{
    apiInterface->IsApplicationSafeForClosure();
}

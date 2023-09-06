#pragma once
#include <Settings.h>
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include <CorePrecompiled.h>


namespace Renderer
{
    class VulkanInterface;

    class RenderingManager
    {
    private:
        uint32_t graphicCommandPoolId, computeCommandPoolId, guiCommandPoolId;

        //uint32_t maxFramesInFlight;

        uint32_t* renderSemaphores, * presentationSemaphores;
        uint32_t* getSwapChainImageFences;

        uint32_t currentFrameIndex{ 0 }, currentSwapchainIndex{ 0 };
        uint32_t currentRenderSemaphoreId, currentPresentationSemaphoreId, currentFenceId;

        void BeginRenderLoop();
        void EndRenderLoop();
        void RenderLoop();

        VulkanInterface* apiInterface;

        void CheckForMSAA();
        Core::Enums::Samples desiredSampleCountForMSAA = Core::Enums::Samples::SAMPLE_COUNT_8_BIT;

    public:
        void Init(VulkanInterface* apiInterface);
        void SetupRenderer();
        void DislogeRenderer();
        void DeInit();
        void PreRender();
        void Render();
        void PostRenderLoopEnd();
        ~RenderingManager();
        RenderingManager();
    };
}

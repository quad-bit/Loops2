#pragma once
#include <Settings.h>
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include <CorePrecompiled.h>

namespace GfxVk
{
    namespace Utility
    {
        class VulkanManager;
    }
}

namespace Core
{
    class RendererSettings;
    struct WindowSettings;
}

namespace Renderer
{
    class RenderingManager;
    class RendererSettings
    {
    private:
        static uint32_t m_renderQueueId;
        static uint32_t m_presentationQueueId;
        static uint32_t m_computeQueueId;
        static uint32_t m_transferQueueId;

        static std::vector<Core::Wrappers::QueueWrapper> m_queueReq;

        static bool m_msaaEnabled;
        static bool m_sampleRateShadingEnabled;
        static bool m_multiSamplingAvailable;
        static Core::Enums::Samples m_maxSampleCountAvailable;

        static std::vector<uint32_t> depthPrepassImageId;
        static uint32_t m_shadowMapWidth, m_shadowMapHeight;
        
        static uint32_t m_swapBufferCount;
        static uint32_t m_maxFramesInFlight, m_currentFrameInFlight;
        static uint32_t m_maxFrameRate;
        static Core::Enums::Format m_bestDepthFormat;

    public:
        friend class RenderingManager;

        static uint32_t GetRenderQueueId();
        static uint32_t GetPresentationQueueId();
        static uint32_t GetComputeQueueId();
        static uint32_t GetTransferQueueId();

        static Core::Enums::Samples GetMaxSampleCountAvailable();

        static bool IsMsaaEnabled();
        static bool IsSampleRateShadingAvailable();
        static bool IsMultiSamplingAvailable();
        static const uint32_t& GetMaxFramesInFlightCount();
        static const uint32_t& GetCurrentFrameIndex();
        static const uint32_t& GetSwapBufferCount();
        static const Core::Enums::Format& GetBestDepthFormat();
    };


    /*
    * RenderingManager handles the VulkanManager (basic vulkan related implementation)
    */
    class RenderingManager
    {
    private:
        uint32_t m_graphicCommandPoolId, m_computeCommandPoolId, m_guiCommandPoolId;

        //uint32_t maxFramesInFlight;

        uint32_t* m_renderSemaphores, * m_presentationSemaphores;
        uint32_t* m_getSwapChainImageFences;

        uint32_t m_currentFrameIndex{ 0 }, m_currentSwapchainIndex{ 0 };
        uint32_t m_currentRenderSemaphoreId, m_currentPresentationSemaphoreId, m_currentFenceId;

        void BeginRenderLoop();
        void EndRenderLoop();
        void RenderLoop();

        void CheckForMSAA();
        Core::Enums::Samples m_desiredSampleCountForMSAA = Core::Enums::Samples::SAMPLE_COUNT_8_BIT;

        std::unique_ptr<GfxVk::Utility::VulkanManager> m_vulkanMngrObj;

        Core::Settings* m_settings;
        const Core::WindowSettings& m_windowSettings;

    public:
        void Init(GLFWwindow* window);
        void SetupRenderer();
        void DislogeRenderer();
        void DeInit();
        void PreRender();
        void Render();
        void PostRenderLoopEnd();
        ~RenderingManager();
        RenderingManager(const Core::WindowSettings& windowSettings/*, Core::Settings* settings*/);
    };
}


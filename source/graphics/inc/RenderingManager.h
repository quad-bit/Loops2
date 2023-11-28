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
    struct WindowSettings;
}

namespace Renderer
{
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


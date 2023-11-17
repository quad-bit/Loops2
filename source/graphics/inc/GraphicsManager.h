#pragma once
#include <stdint.h>
#include <string>
#include <memory>

#include <PlatformSettings.h>
#include <Settings.h>
#include <RendererSettings.h>
#include <RenderData.h>

#if defined(GLFW_ENABLED)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace Core
{
    class Settings;
    namespace Wrappers
    {
        struct QueueWrapper;
    }
    namespace Enums
    {
        enum class QueueType;
        enum class PipelineType;
    }
}

namespace Renderer
{
    class RenderingManager;

    namespace RenderGraph
    {
        class RenderGraphManager;
    }

    namespace Windowing
    {
        class WindowManager;
        struct KeyInputEvent;
    }

    class GraphicsManager
    {
    private:
        GraphicsManager(GraphicsManager const&) = delete;
        GraphicsManager const& operator= (GraphicsManager const&) = delete;

        const Core::WindowSettings& m_windowSettings;
        Core::Utility::RenderData& m_renderData;

        std::unique_ptr <Renderer::RenderingManager> m_renderingMngrObj;
        std::unique_ptr <Windowing::WindowManager> m_windowMngrObj;
        std::unique_ptr<Renderer::RenderGraph::RenderGraphManager> m_renderGraphManager;

        void KeyBoardEventHandler(Renderer::Windowing::KeyInputEvent * evt);

    public:
        ~GraphicsManager();
        GraphicsManager() = delete;
        GraphicsManager(const Core::WindowSettings& windowSettings, Core::Utility::RenderData& renderData);
        void Init();
        void DeInit();
        void Update();
        static GraphicsManager* GetInstance();

        void SetupRenderer();
        void DislogeRenderer();

        void PreUpdate();
        void PostUpdate();

        bool IsWindowActive();

        GLFWwindow* GetGlfwWindow();
    };
}
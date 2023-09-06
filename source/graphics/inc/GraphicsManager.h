#pragma once
#include <stdint.h>
#include <string>
#include <memory>

#include <PlatformSettings.h>

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
    class VulkanInterface;
    class RenderingManager;

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

        std::unique_ptr<Core::Settings> m_settings;

        VulkanInterface* apiInterface;
        std::unique_ptr <Renderer::RenderingManager> m_renderingMngrObj;
        std::unique_ptr <Windowing::WindowManager> m_windowMngrObj;

        void KeyBoardEventHandler(Renderer::Windowing::KeyInputEvent * evt);

    public:
        ~GraphicsManager();
        GraphicsManager();
        GraphicsManager(uint32_t winWidth, uint32_t winHeight, uint32_t renderWidth, uint32_t renderHeight, std::string winName);
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
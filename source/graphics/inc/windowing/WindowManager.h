#pragma once

#include <Settings.h>
#include <stdint.h>
#include <string>
#include <PlatformSettings.h>

#if defined(GLFW_ENABLED)
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
#endif

namespace Renderer
{
    namespace Windowing
    {
        class WindowManager
        {
        private:
            void                                InitOSWindow();
            void                                DeInitOSWindow();
            void                                UpdateOSWindow();

            WindowManager() = delete;
            WindowManager(WindowManager const&) = delete;
            WindowManager const& operator= (WindowManager const&) = delete;

            const Core::WindowSettings& m_windowSettings;

        public:
            ~WindowManager(){}
            WindowManager(const Core::WindowSettings& windowSettings);
            void                                Init();
            void                                DeInit();
            void                                Close();
            bool                                Update();

            bool                                windowShouldRun = true;

#if defined(GLFW_ENABLED) 
            GLFWwindow* glfwWindow = nullptr;
#elif VK_USE_PLATFORM_WIN32_KHR
            HINSTANCE                            win32Instance = NULL;
            HWND                                win32Window = NULL;
            std::string                            win32ClassName;
            static uint64_t                        win32ClassIdCounter;
#elif VK_USE_PLATFORM_XCB_KHR
            xcb_connection_t* xcbConnection = nullptr;
            xcb_screen_t* xcbScreen = nullptr;
            xcb_window_t                        xcbWindow = 0;
            xcb_intern_atom_reply_t* xcbAtomWindowReply = nullptr;
#endif

        };
    }
}
#pragma once

#include <Settings.h>
#include <vector>
#include "windowing/InputEvents.h"
#include <PlatformSettings.h>

#if defined(GLFW_ENABLED)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace Renderer
{
    namespace Windowing
    {
        class InputManager
        {

        private:
            InputManager();
            InputManager(InputManager const&) {}
            InputManager const& operator= (InputManager const&) {}

            static InputManager* instance;

            std::vector<KeyInputEvent*> keyEventPool;
            uint16_t poolSize = 10;
            uint16_t eventCounter = 0;
            GLFWwindow* m_windowObj;

        public:
            void Init(GLFWwindow* windowObj);
            void DeInit();
            void Update();
            static InputManager* GetInstance();
            ~InputManager();

            void EventNotification(KeyInputEvent* event);

            KeyInputEvent* FetchKeyInputEvent();
        };
    }
}
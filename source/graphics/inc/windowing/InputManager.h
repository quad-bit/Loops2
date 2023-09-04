#pragma once

#include <Settings.h>
#include <vector>
#include "windowing/InputEvents.h"

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

        public:
            void Init();
            void DeInit();
            void Update();
            static InputManager* GetInstance();
            ~InputManager();

            void EventNotification(KeyInputEvent* event);

            KeyInputEvent* FetchKeyInputEvent();
        };
    }
}
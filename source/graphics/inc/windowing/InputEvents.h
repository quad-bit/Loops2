#pragma once

#include <ECS/Events/Event.h>
#include <glm/glm.hpp>

namespace Renderer
{
    namespace Windowing
    {
        enum class KeyState
        {
            PRESSED,
            DOWN,
            RELEASED,
            NONE
        };

        enum class MouseButtons
        {
            Left,
            Right,
            None
        };

        struct KeyInputEvent : public Core::ECS::Events::Event
        {
            KeyState keyState = KeyState::NONE;
            const char* keyname;
            const char* keyStateName;
        };

        struct MouseButtonEvent : public Core::ECS::Events::Event
        {
            KeyState keyState = KeyState::NONE;
            MouseButtons button;
            const char* keyname;
            const char* keyStateName;
            float mouseX, mouseY;
        };

        struct MousePointerEvent : public Core::ECS::Events::Event
        {
            double x, y;
        };

        struct MouseDragEvent : public Core::ECS::Events::Event
        {
            KeyState keyState = KeyState::NONE;
            double x, y;
        };
    }
}

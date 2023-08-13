#pragma once

#include <Event.h>
#include <glm\glm.hpp>

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

struct KeyInputEvent : public Event
{
    KeyState keyState = KeyState::NONE;
    const char* keyname;
    const char* keyStateName;
};

struct MouseButtonEvent : public Event
{
    KeyState keyState = KeyState::NONE;
    MouseButtons button;
    const char* keyname;
    const char* keyStateName;
    float mouseX, mouseY;
};

struct MousePointerEvent : public Event
{
    double x, y;
};

struct MouseDragEvent : public Event
{
    KeyState keyState = KeyState::NONE;
    double x, y;
};
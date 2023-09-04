#include "windowing/MouseInputManager.h"
#include <Platform/Assertion.h>
#include <ECS/Events/EventBus.h>
#include "CorePrecompiled.h"

Renderer::Windowing::MouseInputManager* Renderer::Windowing::MouseInputManager::instance = nullptr;

void Renderer::Windowing::MouseInputManager::CreateMouseDragEvent(Renderer::Windowing::KeyState state)
{
    if (currentMouseButtonDown == Renderer::Windowing::MouseButtons::Left || currentMouseButtonDown == Renderer::Windowing::MouseButtons::Right)
    {
        MouseDragEvent * evt = FetchMouseDragEvent();
        evt->x = mouseX;
        evt->y = mouseY;
        evt->keyState = currentMouseButtonState;

        Core::ECS::Events::EventBus::GetInstance()->Publish(evt);
    }
}

void Renderer::Windowing::MouseInputManager::Init()
{
    mouseButtonEventPool.resize(buttonEventPoolSize);
    for (uint16_t i = 0; i < buttonEventPoolSize; i++)
    {
        mouseButtonEventPool[i] = new Renderer::Windowing::MouseButtonEvent;
    }

    mousePointerEventPool.resize(pointerEventPoolSize);
    for (uint16_t i = 0; i < pointerEventPoolSize; i++)
    {
        mousePointerEventPool[i] = new Renderer::Windowing::MousePointerEvent;
    }

    mouseDragEventPool.resize(dragEventPoolSize);
    for (uint16_t i = 0; i < dragEventPoolSize; i++)
    {
        mouseDragEventPool[i] = new Renderer::Windowing::MouseDragEvent;
    }
}

void Renderer::Windowing::MouseInputManager::DeInit()
{
    for (uint16_t i = 0; i < pointerEventPoolSize; i++)
    {
        delete mousePointerEventPool[i];
    }

    for (uint16_t i = 0; i < buttonEventPoolSize; i++)
    {
        delete mouseButtonEventPool[i];
    }

    for (uint16_t i = 0; i < dragEventPoolSize; i++)
    {
        delete mouseDragEventPool[i];
    }
}

void Renderer::Windowing::MouseInputManager::Update()
{
    if (currentMouseButtonDown != Renderer::Windowing::MouseButtons::None && currentMouseButtonState == Renderer::Windowing::KeyState::PRESSED )
    {
        CreateMouseDragEvent(Renderer::Windowing::KeyState::PRESSED);
    }
}

Renderer::Windowing::MouseInputManager * Renderer::Windowing::MouseInputManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::Windowing::MouseInputManager();
    }
    return instance;
}

Renderer::Windowing::MouseInputManager::~MouseInputManager()
{
}

Renderer::Windowing::MouseButtonEvent * Renderer::Windowing::MouseInputManager::FetchMouseButtonEvent()
{
    Renderer::Windowing::MouseButtonEvent * evt = mouseButtonEventPool[buttonEventCounter];

    if (buttonEventCounter >= buttonEventPoolSize)
    {
        buttonEventCounter = 0;
    }
    return evt;
}

Renderer::Windowing::MousePointerEvent * Renderer::Windowing::MouseInputManager::FetchMousePointerEvent()
{
    MousePointerEvent * evt = mousePointerEventPool[pointerEventCounter];

    if (pointerEventCounter >= pointerEventPoolSize)
    {
        pointerEventCounter = 0;
    }
    return evt;
}

Renderer::Windowing::MouseDragEvent * Renderer::Windowing::MouseInputManager::FetchMouseDragEvent()
{
    MouseDragEvent * evt = mouseDragEventPool[dragEventCounter];

    if (dragEventCounter >= dragEventPoolSize)
    {
        dragEventCounter = 0;
    }
    return evt;
}

void Renderer::Windowing::MouseInputManager::MousePointerEventHandler(double x, double y)
{
    //printf("Cursor position: %f %f\n", x, y);
    // publish the event when needed

    mouseX = x;
    mouseY = y;
}

void Renderer::Windowing::MouseInputManager::MouseButtonEventHandler(const char * buttonName, const char * buttonEvent)
{
    // publish the event when needed
    const char* upAction = "released";
    const char* pressedAction = "pressed";

    //PLOGD << buttonEvent;

    Renderer::Windowing::MouseButtonEvent * keyEvent = FetchMouseButtonEvent();
    keyEvent->keyname = buttonName;
    
    if (strcmp(buttonEvent, upAction) == 0)
    {
        keyEvent->keyState = Renderer::Windowing::KeyState::RELEASED;
        currentMouseButtonDown = MouseButtons::None;
    }
    else if (strcmp(buttonEvent, pressedAction) == 0)
    {
        keyEvent->keyState = Renderer::Windowing::KeyState::PRESSED;
    
        const char* right = "right";
        const char* left = "left";

        currentMouseButtonDown = MouseButtons::None;
        if (strcmp(buttonName, right) == 0)
        {
            currentMouseButtonDown = MouseButtons::Right;
        }
        else if (strcmp(buttonName, left) == 0)
        {
            currentMouseButtonDown = MouseButtons::Left;
        }
    }

    currentMouseButtonState = keyEvent->keyState;
    keyEvent->button = currentMouseButtonDown;
    keyEvent->mouseX = (float)mouseX;
    keyEvent->mouseY = (float)mouseY;

    Core::ECS::Events::EventBus::GetInstance()->Publish(keyEvent);
}

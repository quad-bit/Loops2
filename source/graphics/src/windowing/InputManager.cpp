#include "windowing/InputManager.h"
#include <ECS/Events/EventBus.h>

Renderer::Windowing::InputManager* Renderer::Windowing::InputManager::instance = nullptr;

Renderer::Windowing::InputManager * Renderer::Windowing::InputManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::Windowing::InputManager();
    }
    return instance;
}

Renderer::Windowing::InputManager::InputManager()
{
    keyEventPool.resize(poolSize);

    for (uint16_t i = 0; i < poolSize; i++)
    {
        keyEventPool[i] = new KeyInputEvent;
    }
}

Renderer::Windowing::InputManager::~InputManager()
{
    for (uint16_t i = 0; i < poolSize; i++)
    {
        delete keyEventPool[i];
    }

    keyEventPool.clear();
}

void Renderer::Windowing::InputManager::EventNotification(KeyInputEvent * event)
{
    //switch (test.keyState)
    //{
    //case KeyInputEvent::KEY_STATE::DOWN :
    //    //printf("down ");
    //    break;

    //case KeyInputEvent::KEY_STATE::RELEASED:
    //    //printf("up ");
    //    break;

    //case KeyInputEvent::KEY_STATE::PRESSED:
    //    printf("pressed ");
    //    break;
    //default:
    //    break;
    //}

    //printf(event.keyname);
    //printf("\n");

    //publish the event.

    Core::ECS::Events::EventBus::GetInstance()->Publish(event);
}

Renderer::Windowing::KeyInputEvent * Renderer::Windowing::InputManager::FetchKeyInputEvent()
{
    Renderer::Windowing::KeyInputEvent * event = keyEventPool[eventCounter];

    if (eventCounter >= poolSize)
    {
        eventCounter = 0;
    }
    return event;
}

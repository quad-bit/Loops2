#include "InputManager.h"
#include <EventBus.h>

InputManager* InputManager::instance = nullptr;

InputManager * InputManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new InputManager();
    }
    return instance;
}

InputManager::InputManager()
{
    keyEventPool.resize(poolSize);

    for (uint16_t i = 0; i < poolSize; i++)
    {
        keyEventPool[i] = new KeyInputEvent;
    }
}

InputManager::~InputManager()
{
    for (uint16_t i = 0; i < poolSize; i++)
    {
        delete keyEventPool[i];
    }

    keyEventPool.clear();
}

void InputManager::EventNotification(KeyInputEvent * event)
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

    EventBus::GetInstance()->Publish(event);
}

KeyInputEvent * InputManager::FetchKeyInputEvent()
{
    KeyInputEvent * event = keyEventPool[eventCounter];

    if (eventCounter >= poolSize)
    {
        eventCounter = 0;
    }
    return event;
}

#include "ECS/Events/EventBus.h"

EventBus* EventBus::instance = nullptr;
EventBus * EventBus::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new EventBus();
    }
    return instance;
}

void EventBus::DeInit()
{
    std::map<std::type_index, HandlerList*>::iterator it;

    for (it = subscribers.begin(); it != subscribers.end(); it++)
    {
        if (it->second == nullptr)
            continue;

        HandlerList list = *it->second;
        auto listBegin = list.begin();
        uint32_t numHandlers = (uint32_t)list.size();
        for (uint32_t i = 0; i < numHandlers; i++)
        {
            std::advance(listBegin, i);
            HandlerFunctionBase * obj = *listBegin;
            delete obj;
        }

        delete it->second;
    }
}

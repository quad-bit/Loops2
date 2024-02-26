#include "ECS/Events/EventBus.h"

Core::ECS::Events::EventBus* Core::ECS::Events::EventBus::instance = nullptr;
Core::ECS::Events::EventBus * Core::ECS::Events::EventBus::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Core::ECS::Events::EventBus();
    }
    return instance;
}

void Core::ECS::Events::EventBus::DeInit()
{
    std::map<std::type_index, Core::ECS::Events::HandlerList*>::iterator it;

    for (it = subscribers.begin(); it != subscribers.end(); it++)
    {
        if (it->second == nullptr)
            continue;

        Core::ECS::Events::HandlerList list = *it->second;
        auto listBegin = list.begin();
        uint32_t numHandlers = (uint32_t)list.size();
        for (uint32_t i = 0; i < numHandlers; i++)
        {
            Core::ECS::Events::HandlerFunctionBase * obj = *listBegin;
            delete obj;
            std::advance(listBegin, 1);
        }

        delete it->second;
    }
}

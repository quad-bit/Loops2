#pragma once
#include <map>
#include <list>
#include <typeindex>
#include "ECS/Events/Event.h"

namespace Core
{
    namespace ECS
    {
        namespace Events
        {
            class HandlerFunctionBase
            {
            public:
                // Call the member function
                void Exec(Event* evnt)
                {
                    Call(evnt);
                }
            private:
                // Implemented by MemberFunctionHandler
                virtual void Call(Event* evnt) = 0;
            };

            template<class T, class EventType>
            class MemberFunctionHandler : public HandlerFunctionBase
            {
            public:
                typedef void (T::* MemberFunction)(EventType*);

                MemberFunctionHandler(T* instance, MemberFunction memberFunction) : instance{ instance }, memberFunction{ memberFunction } {};

                void Call(Event* evnt)
                {
                    // Cast event to the correct type and call member function
                    (instance->*memberFunction)(static_cast<EventType*>(evnt));
                }
            private:
                // Pointer to class instance
                T* instance;

                // Pointer to member function
                MemberFunction memberFunction;
            };


            typedef std::list<HandlerFunctionBase*> HandlerList;

            class EventBus
            {
            private:
                std::map<std::type_index, HandlerList*> subscribers;

                EventBus() {};
                EventBus(EventBus const &) {}
                EventBus const & operator= (EventBus const &) {}

                static EventBus* instance;

            public:

                static EventBus* GetInstance();
                void DeInit();

                template <typename EventType>
                void Publish(EventType * event)
                {
                    std::type_index typeIndex = typeid(EventType);

                    HandlerList * handlers = subscribers[typeIndex];

                    if (handlers == nullptr) {
                        return;
                    }

                    for (auto & handler : *handlers) {
                        if (handler != nullptr) {
                            handler->Exec(event);
                        }
                    }
                }

                template<typename T, typename EventType>
                void Subscribe(T * instance, void (T::*memberFunction)(EventType * type))
                {
                    HandlerList * handlers = subscribers[typeid(EventType)];

                    if (handlers == nullptr)
                    {
                        handlers = new HandlerList;
                        //subscribers.insert(std::pair<std::type_index, HandlerList*> ( typeid(EventType), handlers ));
                        subscribers[typeid(EventType)] = handlers;
                    }

                    handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
                }
            };
        }
    }
}

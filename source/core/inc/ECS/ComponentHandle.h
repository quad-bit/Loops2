#pragma once
#include <stdint.h>

class Entity;
namespace Core
{
    namespace ECS
    {
        template <typename ComponentType>
        class ComponentManager;
    }
}
namespace Core
{
    namespace ECS
    {
        template <typename ComponentType>
        class ComponentHandle
        {
        private:
            Entity* owner;
            ComponentManager<ComponentType>* componentManagerObj;
            ComponentType* componentType;

        public:
            ComponentHandle() {}
            ComponentHandle(Core::ECS::ComponentManager<ComponentType>* componentManagerObj, Entity* owner, uint32_t componentIndex);
            void DestroyComponent();
            ComponentType* operator->() const { return componentType; }
            ComponentType* GetComponent();
        };
    }
}

#include "ComponentManager.h"

template<typename ComponentType>
inline Core::ECS::ComponentHandle<ComponentType>::ComponentHandle(Core::ECS::ComponentManager<ComponentType>* componentManagerObj, Entity * owner, uint32_t componentIndex)
{
    this->owner = owner;
    this->componentManagerObj = componentManagerObj;
    this->componentType = componentManagerObj->GetComponent(owner);
}

template<typename ComponentType>
void Core::ECS::ComponentHandle<ComponentType>::DestroyComponent()
{
    componentManagerObj->RemoveComponent(owner);
}

template<typename ComponentType>
inline ComponentType * Core::ECS::ComponentHandle<ComponentType>::GetComponent()
{
    return componentType;
}

#pragma once

#include "Component.h"

namespace Core
{
    namespace ECS
    {
        class ComponentMask
        {
        private:
            unsigned int mask = 0;

        public:

            template<typename ComponentType>
            void AddComponent()
            {
                mask |= (1 << GetComponentFamily<ComponentType>());// Component<ComponentType>::get());
            }

            template<typename ComponentType>
            void RemoveComponent()
            {
                mask &= ~(1 << Core::ECS::Component<ComponentType>::Family());
            }

            bool isNewMatch(ComponentMask oldMask, ComponentMask systemMask);

            bool isNoLongerMatched(ComponentMask oldMask, ComponentMask systemMask);

            bool matches(ComponentMask systemMask);

            ComponentMask();
            ~ComponentMask();
        };
    }
}
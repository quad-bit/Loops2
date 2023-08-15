#pragma once

#include <stdint.h>
#include <vector>
#include "ECS_Helper.h"

namespace ECS
{
    struct ComponentCounter 
    {
        static int counter;
    };

    template <typename ComponentType>
    class Component
    {
    private:
        static int family;
        bool isComponentActive = true;

    public:
        uint32_t componentId = UINT32_MAX;
        COMPONENT_TYPE componentType;

        static inline int Family()
        {
            if (family == -1)
                family = ComponentCounter::counter++;

            return family;
        }

        bool GetActiveStatus()
        {
            return isComponentActive;
        }

        void SetActiveStatus(const bool& status)
        {
            isComponentActive = status;
        }
    };
}



template <typename ComponentType>
int ECS::Component<ComponentType>::family = -1;

//TODO: Need more understanding on the below function

template<typename ComponentType>
int GetComponentFamily()
{
    int family = ECS::Component<typename std::remove_const<ComponentType>::type>::Family();
    return family;
}

template<typename ComponentType>
void AssignFamily()
{
    ECS::Component<typename std::remove_const<ComponentType>::type>::Family();
}


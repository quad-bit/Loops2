#pragma once

#include "ECS/Events/Event.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"

namespace Core
{
    namespace ECS
    {
        namespace Events
        {
            class MeshToMatAdditionEvent : public Event
            {
            public:
                std::vector<Core::Wrappers::SetWrapper*> setWrapperList;
                uint32_t meshId;
            };
        }
    }
}

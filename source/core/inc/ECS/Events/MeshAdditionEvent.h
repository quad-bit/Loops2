#pragma once

#include "ECS/Events/Event.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"

class MeshToMatAdditionEvent : public Event
{
public:
    std::vector<SetWrapper *> setWrapperList;
    uint32_t meshId; 
};

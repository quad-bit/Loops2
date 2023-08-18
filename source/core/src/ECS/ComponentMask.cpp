#include "ECS/ComponentMask.h"

bool Core::ECS::ComponentMask::isNewMatch(Core::ECS::ComponentMask oldMask, Core::ECS::ComponentMask systemMask)
{
    return (matches(systemMask) && !oldMask.matches(systemMask));
}

bool Core::ECS::ComponentMask::isNoLongerMatched(Core::ECS::ComponentMask oldMask, Core::ECS::ComponentMask systemMask)
{
    return (oldMask.matches(systemMask) && !matches(systemMask));
}

bool Core::ECS::ComponentMask::matches(Core::ECS::ComponentMask systemMask)
{
    return ((mask & systemMask.mask) == systemMask.mask);
}

Core::ECS::ComponentMask::ComponentMask()
{
}


Core::ECS::ComponentMask::~ComponentMask()
{
}

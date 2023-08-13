#include "ECS/ComponentMask.h"

bool ComponentMask::isNewMatch(ComponentMask oldMask, ComponentMask systemMask)
{
    return (matches(systemMask) && !oldMask.matches(systemMask));
}

bool ComponentMask::isNoLongerMatched(ComponentMask oldMask, ComponentMask systemMask)
{
    return (oldMask.matches(systemMask) && !matches(systemMask));
}

bool ComponentMask::matches(ComponentMask systemMask)
{
    return ((mask & systemMask.mask) == systemMask.mask);
}

ComponentMask::ComponentMask()
{
}


ComponentMask::~ComponentMask()
{
}

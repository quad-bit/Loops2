#include "ECS/System.h"
#include "ECS/World.h"
#include "ECS/Entity.h"

void System::RegisterWorld(World * world)
{
    worldObj = world;
}

void System::RegisterEntity(Entity * entity) 
{
    registeredEntities.push_back(entity);
}

// This entity has stopped fitting our current requirements
void System::UnRegisterEntity(Entity* entity) 
{
    for (auto it = registeredEntities.begin(); it != registeredEntities.end(); ++it) 
    {
        Entity* e = *it;
        if (e->id == entity->id) 
        {
            registeredEntities.erase(it);
            return;
        }
    }
}

//ComponentMask System::GetSignature()
//{
//	return signature;
//}

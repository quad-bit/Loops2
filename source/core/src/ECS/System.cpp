#include "ECS/System.h"
#include "ECS/World.h"
#include "ECS/Entity.h"

void Core::ECS::System::RegisterWorld(Core::ECS::World * world)
{
    worldObj = world;
}

void Core::ECS::System::RegisterEntity(Core::ECS::Entity * entity)
{
    registeredEntities.push_back(entity);
}

// This entity has stopped fitting our current requirements
void Core::ECS::System::UnRegisterEntity(Core::ECS::Entity* entity)
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

#include "ECS/EntityManager.h"
#include "ECS/Entity.h"
#include "ECS/EntityHandle.h"
#include "ECS/Components/Transform.h"
#include <string>

EntityManager* EntityManager::entityManagerInstance = nullptr;

EntityManager::EntityManager()
{
}

EntityManager * EntityManager::GetSingleton()
{
    if (entityManagerInstance == nullptr)
    {
        entityManagerInstance = new EntityManager();
    }
    return entityManagerInstance;
}

void EntityManager::Init()
{
    PLOGD << "EntityManager init";
}

void EntityManager::DeInit()
{
    PLOGD << "EntityManager Deinit";

	entityList.clear();
	entityHandleList.clear();
}


Entity * EntityManager::CreateEntity()
{
    lastEntity++;
    Entity * obj = new Entity();
    obj->id = lastEntity;
    std::string name = "Entity" + std::to_string(obj->id);
    
    obj->entityName = name;
	entityList.push_back(obj);
    return obj;
}

EntityHandle * EntityManager::CreateEntityHandle(Entity * obj, World * worldObj)
{
	EntityHandle * handle = new EntityHandle(obj, worldObj);
	entityHandleList.push_back(handle);

	return handle;
}

EntityHandle * const EntityManager::FindEntity(const std::string & name)
{
    std::vector<EntityHandle*>::iterator it;
    it = std::find_if(entityHandleList.begin(), entityHandleList.end(), [=](EntityHandle * e) {
        return e->GetEntity()->entityName == name ; });

    if (it != entityHandleList.end())
    {
        return *it;
    }
    return nullptr;
}

void EntityManager::DestroyEntity(Entity * entity)
{
	uint32_t pos = 0;
	for (uint32_t i = 0; i < entityList.size(); i++) 
	{
		if (entityList[i]->id == entity->id)
		{
			pos = i;
			break;
		}
    } 
    
    entityHandleList[pos]->RemoveComponent<ECS::Components::Transform>(entity->transform);

    if (entity != nullptr)
    {
        if (entity->transform != nullptr)
        {
            delete entity->transform;
            entity->transform = NULL;
        }

        delete entity;
        entity = nullptr;
    }

	entityList.erase(entityList.begin() + pos);

    if (entityHandleList[pos])
    {
        delete entityHandleList[pos];
    }

	entityHandleList.erase(entityHandleList.begin() + pos);
}

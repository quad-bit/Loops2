#include "ECS/EntityManager.h"
#include "ECS/Entity.h"
#include "ECS/EntityHandle.h"
#include "ECS/Components/Transform.h"
#include <string>

Core::ECS::EntityManager* Core::ECS::EntityManager::entityManagerInstance = nullptr;

Core::ECS::EntityManager::EntityManager()
{
}

Core::ECS::EntityManager * Core::ECS::EntityManager::GetSingleton()
{
    if (entityManagerInstance == nullptr)
    {
        entityManagerInstance = new EntityManager();
    }
    return entityManagerInstance;
}

void Core::ECS::EntityManager::Init()
{
    PLOGD << "EntityManager init";
}

void Core::ECS::EntityManager::DeInit()
{
    PLOGD << "EntityManager Deinit";

	entityList.clear();
	entityHandleList.clear();
}


Core::ECS::Entity * Core::ECS::EntityManager::CreateEntity()
{
    lastEntity++;
    Core::ECS::Entity * obj = new Core::ECS::Entity();
    obj->id = lastEntity;
    std::string name = "Entity" + std::to_string(obj->id);
    
    obj->entityName = name;
	entityList.push_back(obj);
    return obj;
}

Core::ECS::EntityHandle * Core::ECS::EntityManager::CreateEntityHandle(Core::ECS::Entity * obj, Core::ECS::World * worldObj)
{
    Core::ECS::EntityHandle * handle = new Core::ECS::EntityHandle(obj, worldObj);
	entityHandleList.push_back(handle);

	return handle;
}

Core::ECS::EntityHandle * const Core::ECS::EntityManager::FindEntity(const std::string & name)
{
    std::vector<Core::ECS::EntityHandle*>::iterator it;
    it = std::find_if(entityHandleList.begin(), entityHandleList.end(), [=](Core::ECS::EntityHandle * e) {
        return e->GetEntity()->entityName == name ; });

    if (it != entityHandleList.end())
    {
        return *it;
    }
    return nullptr;
}

void Core::ECS::EntityManager::DestroyEntity(Core::ECS::Entity * entity)
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

    entityHandleList[pos]->RemoveComponent<Core::ECS::Components::Transform>(entity->transform);

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

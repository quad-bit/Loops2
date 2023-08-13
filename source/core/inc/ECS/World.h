#pragma once
#include <vector>
#include <map>

#include "ComponentMask.h"


template<typename ComponentType>
class ComponentManager;

class BaseComponentManager;
class Entity;
class EntityHandle;
class System;

//#include "ComponentHandle.h"

template<typename T>
class ComponentHandle;


class World
{
private:
    template<typename ComponentType>
    ComponentManager<ComponentType>* GetComponentManager();

    std::vector<BaseComponentManager*> managerList;
    std::vector<System*> systemList;

    std::map<Entity * , ComponentMask> entityMasks;

public:

    //explicit World(std::unique_ptr<EntityManager> entityManager);

    void Init();

    void DestroyEntity(Entity* entityObj);
    void DestroyEntity(EntityHandle* entityHandleObj);
    EntityHandle* CreateEntity();
    EntityHandle* CreateEntity(const std::string & name);

    EntityHandle * const FindEntity(const std::string & name);

    template<typename ComponentType>
    void AddComponent(ComponentType* componentType, Entity * entityObj);

    template<typename ComponentType>
    void RemoveComponent(ComponentType* componentType, Entity * entityObj);

    void UpdateEntityMask(Entity *entity, ComponentMask oldMask);

    template<typename ComponentType>
    ComponentManager<ComponentType>* CreateManager();

    void AddSystem(System* system);

    template<typename ComponentType, typename... Args>
    void Unpack(Entity* e, ComponentHandle<ComponentType>& handle, ComponentHandle<Args>&... args)
    {
        typedef ComponentManager<ComponentType> componentManagerType;

        componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
        handle = *(manager->GetComponentHandle(e));

        Unpack<Args...>(e, args...);
    }

    template<typename ComponentType>
    void Unpack(Entity* e, ComponentHandle<ComponentType>& handle)
    {
        typedef ComponentManager<ComponentType> componentManagerType;

        componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
        handle = *(manager->GetComponentHandle(e));
    }
    
    template<typename ComponentType, typename... Args>
    void Unpack(Entity* e, ComponentHandle<ComponentType>** handle, ComponentHandle<Args>**... args)
    {
        typedef ComponentManager<ComponentType> componentManagerType;

        componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
        *handle = (manager->GetComponentHandle(e));

        Unpack<Args...>(e, args...);
    }

    template<typename ComponentType>
    void Unpack(Entity* e, ComponentHandle<ComponentType>** handle)
    {
        typedef ComponentManager<ComponentType> componentManagerType;

        componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
        *handle = (manager->GetComponentHandle(e));
    }
    


    void Update(float dt);

    void Render(float dt);

    void DeInit();

};


// =========================================     ==================================================

#include "ComponentManager.h"
#include "EntityHandle.h"
#include "Entity.h"
#include "Component.h"

template<typename ComponentType>
inline ComponentManager<ComponentType>* World::GetComponentManager() 
{
	int family = GetComponentFamily<ComponentType>();
    return (ComponentManager<ComponentType>*)managerList[family];
}

template<typename ComponentType>
inline void World::AddComponent(ComponentType * componentType, Entity * entityObj)
{
    ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
    manager->AddComponent(componentType, entityObj);

    ComponentMask oldMask = entityMasks[entityObj];
    entityMasks[entityObj].AddComponent<ComponentType>();

    UpdateEntityMask(entityObj, oldMask);
}

template<typename ComponentType>
inline void World::RemoveComponent(ComponentType * componentType, Entity * entityObj)
{
    ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
    manager->RemoveComponent(entityObj);

    ComponentMask oldMask = entityMasks[entityObj];
    entityMasks[entityObj].RemoveComponent<ComponentType>();

    UpdateEntityMask(entityObj, oldMask);
}

template<typename ComponentType>
inline ComponentManager<ComponentType>* World::CreateManager() // TODO ..Create the managers properly, hacked for testing..!!!
{
    ComponentManager<ComponentType>* manager = new ComponentManager<ComponentType>();
    managerList.push_back(manager);

	AssignFamily<ComponentType>();
    return manager;
}








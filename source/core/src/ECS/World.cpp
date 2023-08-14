#include "ECS/World.h"
#include "ECS/System.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Transform.h"

EntityHandle* World::CreateEntity()
{
    Entity* obj = EntityManager::GetSingleton()->CreateEntity();
    EntityHandle * handle = EntityManager::GetSingleton()->CreateEntityHandle(obj, this);

    ComponentMask mask;
    entityMasks.insert({ obj, mask });

    ECS::Components::Transform * transform = new ECS::Components::Transform(obj);
    handle->AddComponent<ECS::Components::Transform>(transform);
    obj->transform = transform;

    return handle;
}

EntityHandle * World::CreateEntity(const std::string & name)
{
    EntityHandle * handle = CreateEntity();
    handle->GetEntity()->entityName = name;
    return handle;
}

EntityHandle * const World::FindEntity(const std::string & name)
{
    return EntityManager::GetSingleton()->FindEntity(name);
}

void World::UpdateEntityMask(Entity * entity, ComponentMask oldMask)
{
    ComponentMask newMask = entityMasks[entity];
    // TODO : Complete the system integration

    for (auto &system : systemList) {
        ComponentMask systemSignature = system->GetSignature();
        if (newMask.isNewMatch(oldMask, systemSignature)) {
            // We match but didn't match before
            system->RegisterEntity(entity);
        }
        else if (newMask.isNoLongerMatched(oldMask, systemSignature)) {
            system->UnRegisterEntity(entity);
        }
    }
}

void World::AddSystem(System * system)
{
    systemList.push_back(system);
    system->RegisterWorld(this);
}

void World::Update(float dt)
{
    for (auto system : systemList)
    {
        system->Update(dt);
    }
}
//not getting used
void World::Render(float dt)
{
    return;
    for (auto system : systemList)
    {
        system->Render(dt);
    }
}

void World::DeInit()
{
    PLOGD << "World Deinit";
    

    for (auto system : systemList)
    {
        system->DeInit();
    }

    EntityManager::GetSingleton()->DeInit();

    for (uint32_t i = 0 ; i < managerList.size(); i++)
    {
        delete managerList[i];
    }
    managerList.clear();

    delete EntityManager::GetSingleton();
}

void World::Init()
{
    PLOGD << "World init";

    EntityManager::GetSingleton()->Init();

    for (auto system : systemList)
    {
        system->Init();
    }
}

void World::DestroyEntity(Entity* entityObj)
{
    for (auto system : systemList)
    {
        system->UnRegisterEntity(entityObj);
    }

    /*if (entityObj != nullptr)
    {
        if (entityObj->transform != nullptr)
        {
            delete entityObj->transform;
        }

        delete entityObj;
    }*/

    EntityManager::GetSingleton()->DestroyEntity(entityObj);
}

void World::DestroyEntity(EntityHandle * entityHandleObj)
{
    DestroyEntity(entityHandleObj->GetEntity());
}

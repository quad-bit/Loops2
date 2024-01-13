#include "ECS/World.h"
#include "ECS/System.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"

Core::ECS::EntityHandle* Core::ECS::World::CreateEntity()
{
    Core::ECS::Entity* obj = Core::ECS::EntityManager::GetSingleton()->CreateEntity();
    Core::ECS::EntityHandle * handle = Core::ECS::EntityManager::GetSingleton()->CreateEntityHandle(obj, this);

    Core::ECS::ComponentMask mask;
    entityMasks.insert({ obj, mask });

    Core::ECS::Components::Transform * transform = new Core::ECS::Components::Transform(obj);
    handle->AddComponent<Core::ECS::Components::Transform>(transform);
    obj->transform = transform;

    return handle;
}

Core::ECS::EntityHandle * Core::ECS::World::CreateEntity(const std::string & name)
{
    Core::ECS::EntityHandle * handle = CreateEntity();
    handle->GetEntity()->entityName = name;
    return handle;
}

Core::ECS::EntityHandle * const Core::ECS::World::FindEntity(const std::string & name)
{
    return Core::ECS::EntityManager::GetSingleton()->FindEntity(name);
}

const std::vector<Core::ECS::Entity*>& Core::ECS::World::GetEntityList()
{
    return Core::ECS::EntityManager::GetSingleton()->GetEntityList();
}

void Core::ECS::World::UpdateEntityMask(Core::ECS::Entity * entity, Core::ECS::ComponentMask oldMask)
{
    Core::ECS::ComponentMask newMask = entityMasks[entity];
    // TODO : Complete the system integration

    for (auto &system : systemList) {
        Core::ECS::ComponentMask systemSignature = system->GetSignature();
        if (newMask.isNewMatch(oldMask, systemSignature)) {
            // We match but didn't match before
            system->RegisterEntity(entity);
        }
        else if (newMask.isNoLongerMatched(oldMask, systemSignature)) {
            system->UnRegisterEntity(entity);
        }
    }
}

void Core::ECS::World::AddSystem(Core::ECS::System * system, COMPONENT_TYPE componentType)
{
    systemList.push_back(system);
    system->RegisterWorld(this);

    if (componentType == COMPONENT_TYPE::CAMERA)
        camSystem = system;
}

void Core::ECS::World::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    for (auto system : systemList)
    {
        system->Update(dt, frameInfo);
    }
}
//not getting used
void Core::ECS::World::Render(float dt)
{
    return;
    for (auto system : systemList)
    {
        system->Render(dt);
    }
}

void Core::ECS::World::DeInit()
{
    PLOGD << "World Deinit";
    

    for (auto system : systemList)
    {
        system->DeInit();
    }

    Core::ECS::EntityManager::GetSingleton()->DeInit();

    for (uint32_t i = 0 ; i < managerList.size(); i++)
    {
        delete managerList[i];
    }
    managerList.clear();

    delete Core::ECS::EntityManager::GetSingleton();
}

void Core::ECS::World::Init()
{
    PLOGD << "World init";

    Core::ECS::EntityManager::GetSingleton()->Init();

    for (auto system : systemList)
    {
        system->Init();
    }
}

void Core::ECS::World::DestroyEntity(Core::ECS::Entity* entityObj)
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

    Core::ECS::EntityManager::GetSingleton()->DestroyEntity(entityObj);
}

void Core::ECS::World::DestroyEntity(Core::ECS::EntityHandle * entityHandleObj)
{
    DestroyEntity(entityHandleObj->GetEntity());
}

void Core::ECS::World::SetMainCamera(uint32_t camComponentId)
{

}

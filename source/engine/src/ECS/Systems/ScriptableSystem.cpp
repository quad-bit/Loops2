#include "ECS/Systems/ScriptableSystem.h"
#include <ECS/Components/Scriptable.h>
#include <ECS/World.h>
#include <ECS/ECS_Setting.h>
#include <ECS/ComponentHandle.h>

void ScriptableSystem::Init()
{
    PLOGD << "ScriptableSystem init";

}

void ScriptableSystem::DeInit()
{
    PLOGD << "ScriptableSystem Deinit";

}

void ScriptableSystem::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    switch (appState)
    {
    case Core::ECS::APP_STATE::STARTED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Init();
        }
        appState = Core::ECS::APP_STATE::RUNNING;
        break;

    case Core::ECS::APP_STATE::RUNNING:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Update(dt);
        }
        break;

    case Core::ECS::APP_STATE::PAUSED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if(scriptableHandle->runInEditMode)
                scriptableHandle->Update(dt);
        }
        break;

    case Core::ECS::APP_STATE::STOPPED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->DeInit();
        }
        appState = Core::ECS::APP_STATE::NONE;
        break;

    case Core::ECS::APP_STATE::NONE:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if (scriptableHandle->runInEditMode)
                scriptableHandle->Update(dt);
        }
        break;

    default:
        break;
    }
}

void ScriptableSystem::Render(float dt)
{
    switch (appState)
    {
    case Core::ECS::APP_STATE::STARTED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Init();
        }
        appState = Core::ECS::APP_STATE::RUNNING;
        break;

    case Core::ECS::APP_STATE::RUNNING:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Render(dt);
        }
        break;

    case Core::ECS::APP_STATE::PAUSED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if (scriptableHandle->runInEditMode)
                scriptableHandle->Render(dt);
        }
        break;

    case Core::ECS::APP_STATE::STOPPED:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->DeInit();
        }
        appState = Core::ECS::APP_STATE::NONE;
        break;

    case Core::ECS::APP_STATE::NONE:
        for (auto & entity : registeredEntities)
        {
            Core::ECS::ComponentHandle<Core::ECS::Components::Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if (scriptableHandle->runInEditMode)
                scriptableHandle->Render(dt);
        }
        break;

    default:
        break;
    }
}

ScriptableSystem::ScriptableSystem()
{
    signature.AddComponent<Core::ECS::Components::Scriptable>();
}

ScriptableSystem::~ScriptableSystem()
{
}

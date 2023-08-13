#include "ScriptableSystem.h"
#include "Scriptable.h"
#include "World.h"
#include "ECS_Setting.h"
#include "ComponentHandle.h"

void ScriptableSystem::Init()
{
    PLOGD << "ScriptableSystem init";

}

void ScriptableSystem::DeInit()
{
    PLOGD << "ScriptableSystem Deinit";

}

void ScriptableSystem::Update(float dt)
{
    switch (appState)
    {
    case APP_STATE::STARTED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Init();
        }
        appState = APP_STATE::RUNNING;
        break;

    case APP_STATE::RUNNING:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Update(dt);
        }
        break;

    case APP_STATE::PAUSED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if(scriptableHandle->runInEditMode)
                scriptableHandle->Update(dt);
        }
        break;

    case APP_STATE::STOPPED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->DeInit();
        }
        appState = APP_STATE::NONE;
        break;

    case APP_STATE::NONE:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
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
    case APP_STATE::STARTED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Init();
        }
        appState = APP_STATE::RUNNING;
        break;

    case APP_STATE::RUNNING:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->Render(dt);
        }
        break;

    case APP_STATE::PAUSED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);
            if (scriptableHandle->runInEditMode)
                scriptableHandle->Render(dt);
        }
        break;

    case APP_STATE::STOPPED:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
            worldObj->Unpack(entity, scriptableHandle);

            scriptableHandle->DeInit();
        }
        appState = APP_STATE::NONE;
        break;

    case APP_STATE::NONE:
        for (auto & entity : registeredEntities)
        {
            ComponentHandle<Scriptable> scriptableHandle;
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
    signature.AddComponent<Scriptable>();
}

ScriptableSystem::~ScriptableSystem()
{
}

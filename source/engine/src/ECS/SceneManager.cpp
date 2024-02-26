#include "ECS/SceneManager.h"
#include <ECS/EntityHandle.h>
#include <ECS/Components/Scriptable.h>
#include <ECS/ECS_Setting.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Components/Transform.h>
#include <SceneGraphManager.h>

Engine::SceneManager::SceneManager()
{
    sceneRootEntityHandle = worldObj->CreateEntity();
    sceneRootTransform = sceneRootEntityHandle->GetEntity()->transform;
    sceneRootEntityHandle->GetEntity()->entityName = "sceneRoot";

    SceneGraphManager::GetInstance()->Init(sceneRootTransform);

    Core::ECS::Events::EventBus::GetInstance()->Subscribe<Engine::SceneManager, Renderer::Windowing::KeyInputEvent>(this, &SceneManager::HandleSceneControls);
}

Engine::SceneManager::~SceneManager()
{
    /*scriptableParent->RemoveComponent<Scriptable>(playerHandlerScript);
    delete playerHandlerScript;
    playerHandlerScript = NULL;
    worldObj->DestroyEntity(scriptableParent);
    */
    SceneGraphManager::GetInstance()->DeInit();
    delete SceneGraphManager::GetInstance();

    worldObj->DestroyEntity(sceneRootEntityHandle);
}


Core::ECS::Components::Transform * const Engine::SceneManager::GetSceneRootTransform()
{
    ASSERT_MSG_DEBUG(sceneRootTransform != nullptr, "scene root null");
    return sceneRootTransform;
}

void Engine::SceneManager::HandleSceneControls(Renderer::Windowing::KeyInputEvent * inputEvent)
{
    if (strcmp(inputEvent->keyname, "P") == 0)
    {
        if (inputEvent->keyState == Renderer::Windowing::KeyState::RELEASED)
        {
            if (appState == Core::ECS::APP_STATE::NONE || appState == Core::ECS::APP_STATE::STOPPED)
                appState = Core::ECS::APP_STATE::STARTED;
            else if (appState == Core::ECS::APP_STATE::RUNNING)
                appState = Core::ECS::APP_STATE::STOPPED;
        }
    }

    // Just for testing, TODO : need to be removed
    /*if (strcmp(inputEvent->keyname, "T") == 0)
    {
        if (inputEvent->keyState == Core::ECS::KeyState::RELEASED)
        {
            
        }
    }*/
}

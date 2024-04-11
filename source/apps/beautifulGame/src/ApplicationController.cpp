#include "ApplicationController.h"
#include <ECS/Components/Scriptable.h>
#include <ECS/ECS_Setting.h>
#include "SceneManagerScript.h"
#include <ECS/EntityHandle.h>
#include <SceneLoader.h>

void ApplicationController::Init()
{
    mainObject = worldObj->CreateEntity();
    mainObject->GetEntity()->entityName = "mainObject";
    sceneManagerScript = new SceneManagerScript();
    mainObject->AddComponent<Core::ECS::Components::Scriptable>(sceneManagerScript);
}

void ApplicationController::Update()
{

}

void ApplicationController::DeInit()
{
    mainObject->RemoveComponent<Core::ECS::Components::Scriptable>(sceneManagerScript);
    delete sceneManagerScript;
    sceneManagerScript = NULL;
    worldObj->DestroyEntity(mainObject);
}

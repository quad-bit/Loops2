#include <CorePrecompiled.h>
#include "ECS/ECS_Manager.h"
#include <ECS/World.h>
#include <ECS/Components/Transform.h>
#include "ECS/Systems/TransformSystem.h"
#include <ECS/Components/Scriptable.h>
#include "ECS/Systems/ScriptableSystem.h"
#include <ECS/ECS_Setting.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Components/Mesh.h>
#include <ECS/Components/Material.h>
#include <ECS/Components/Camera.h>
#include "ECS/Systems/CameraSystem.h"
#include <ECS/Components/MeshRenderer.h>
#include "ECS/Systems/MeshRendererSystem.h"
#include "ECS/Systems/LightSystem.h"
#include <ECS/Components/Light.h>

Engine::ECS_Manager* Engine::ECS_Manager::instance = nullptr;
Core::ECS::World * worldObj;

void Engine::ECS_Manager::Init()
{
    PLOGD << "ECS Manager Init";

    worldObj = new Core::ECS::World();

    scriptableManager = worldObj->CreateManager<Core::ECS::Components::Scriptable>();
    transformManager = worldObj->CreateManager<Core::ECS::Components::Transform>();
    meshManager = worldObj->CreateManager<Core::ECS::Components::Mesh>();
    materialManager = worldObj->CreateManager<Core::ECS::Components::Material>();
    cameraManager = worldObj->CreateManager<Core::ECS::Components::Camera>();
    meshRendererManager = worldObj->CreateManager<Core::ECS::Components::MeshRenderer>();
    lightManager = worldObj->CreateManager<Core::ECS::Components::Light>();

    scriptableSystemObj = new ScriptableSystem();
    worldObj->AddSystem(scriptableSystemObj, Core::ECS::COMPONENT_TYPE::SCRIPTABLE);

    transformSystemObj = new TransformSystem();
    worldObj->AddSystem(transformSystemObj, Core::ECS::COMPONENT_TYPE::TRANSFORM);

    cameraSystemObj = new Engine::ECS::Systems::CameraSystem();
    worldObj->AddSystem(cameraSystemObj, Core::ECS::COMPONENT_TYPE::CAMERA);

    meshRendererSystem = new MeshRendererSystem();
    worldObj->AddSystem(meshRendererSystem, Core::ECS::COMPONENT_TYPE::MESH_RENDERER);

    lightSystem = new LightSystem();
    worldObj->AddSystem(lightSystem, Core::ECS::COMPONENT_TYPE::LIGHT);
    //((LightSystem*)lightSystem)->AssignCameraSystem(cameraSystemObj);

    worldObj->Init();
}

void Engine::ECS_Manager::DeInit()
{
    PLOGD << "ECS Manager DeInit";

    Core::ECS::Events::EventBus::GetInstance()->DeInit();
    delete Core::ECS::Events::EventBus::GetInstance();

    worldObj->DeInit();

    delete lightSystem;
    delete meshRendererSystem;
    delete cameraSystemObj;
    delete transformSystemObj;
    delete scriptableSystemObj;

    delete worldObj;
}

void Engine::ECS_Manager::Update(float dt)
{
    worldObj->Update(dt);
}

void Engine::ECS_Manager::Render(float dt)
{
    worldObj->Render(dt);
}

void Engine::ECS_Manager::PrepareRenderData(Core::Utility::RenderData& renderData)
{

}

void Engine::ECS_Manager::CreateSceneResources()
{
    // Create uniform data
    // CameraSystem


}

Engine::ECS_Manager * Engine::ECS_Manager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Engine::ECS_Manager();
    }
    return instance;
}

Engine::ECS_Manager::~ECS_Manager()
{
}

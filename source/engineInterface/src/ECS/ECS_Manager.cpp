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

ECS_Manager* ECS_Manager::instance = nullptr;
World * worldObj;

void ECS_Manager::Init()
{
    PLOGD << "ECS Manager Init";

    worldObj = new World();
    
    scriptableManager = worldObj->CreateManager<ECS::Components::Scriptable>();
    transformManager = worldObj->CreateManager<ECS::Components::Transform>();
    meshManager = worldObj->CreateManager<ECS::Components::Mesh>();
    materialManager = worldObj->CreateManager<ECS::Components::Material>();
    cameraManager = worldObj->CreateManager<ECS::Components::Camera>();
    meshRendererManager = worldObj->CreateManager<ECS::Components::MeshRenderer>();
    lightManager = worldObj->CreateManager<ECS::Components::Light>();

    scriptableSystemObj = new ScriptableSystem();
    worldObj->AddSystem(scriptableSystemObj);

    transformSystemObj = new TransformSystem();
    worldObj->AddSystem(transformSystemObj);

    cameraSystemObj = new CameraSystem();
    worldObj->AddSystem(cameraSystemObj);

    meshRendererSystem = new MeshRendererSystem();
    worldObj->AddSystem(meshRendererSystem);

    lightSystem = new LightSystem();
    worldObj->AddSystem(lightSystem);
    ((LightSystem*)lightSystem)->AssignCameraSystem(cameraSystemObj);

    worldObj->Init();
}

void ECS_Manager::DeInit()
{
    PLOGD << "ECS Manager DeInit";

    EventBus::GetInstance()->DeInit();
    delete EventBus::GetInstance();

    worldObj->DeInit();

    delete lightSystem;
    delete meshRendererSystem;
    delete cameraSystemObj;
    delete transformSystemObj;
    delete scriptableSystemObj;

    delete worldObj;
}

void ECS_Manager::Update(float dt)
{
    worldObj->Update(dt);
}

void ECS_Manager::Render(float dt)
{
    worldObj->Render(dt);
}

ECS_Manager * ECS_Manager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new ECS_Manager();
    }
    return instance;
}

ECS_Manager::~ECS_Manager()
{
}

#include <CorePrecompiled.h>
#include "ECS_EngineManager.h"
#include "World.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "Scriptable.h"
#include "ScriptableSystem.h"
#include "ECS_Setting.h"
#include "EventBus.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "CameraSystem.h"
#include "MeshRenderer.h"
#include "MeshRendererSystem.h"
#include "LightSystem.h"
#include "Light.h"

ECS_Manager* ECS_Manager::instance = nullptr;
World * worldObj;

void ECS_Manager::Init()
{
    PLOGD << "ECS Manager Init";

    worldObj = new World();
    
    scriptableManager = worldObj->CreateManager<Scriptable>();
    transformManager = worldObj->CreateManager<Transform>();
    meshManager = worldObj->CreateManager<Mesh>();
    materialManager = worldObj->CreateManager<Material>();
    cameraManager = worldObj->CreateManager<Camera>();
    meshRendererManager = worldObj->CreateManager<MeshRenderer>();
    lightManager = worldObj->CreateManager<Light>();

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

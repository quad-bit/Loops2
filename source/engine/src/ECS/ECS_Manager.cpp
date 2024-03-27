#include <CorePrecompiled.h>
#include "ECS/ECS_Manager.h"
#include <ECS/World.h>
#include <ECS/Components/Transform.h>
#include "ECS/Systems/TransformSystem.h"
#include <ECS/Components/Scriptable.h>
#include "ECS/Systems/ScriptableSystem.h"
#include "ECS/Systems/MaterialSystem.h"
#include "ECS/Systems/BoundSystem.h"

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
#include <ECS/Components/Bounds.h>

Engine::ECS_Manager* Engine::ECS_Manager::instance = nullptr;
Core::ECS::World * worldObj;

void Engine::ECS_Manager::Init(
    Core::Utility::RenderData& renderData,
    std::unique_ptr<Engine::Utility::GltfLoader>& sceneLoader,
    const Core::WindowSettings& windowSettings)
{
    PLOGD << "ECS Manager Init";

    m_windowSettings = windowSettings;

    worldObj = new Core::ECS::World();

    scriptableManager = worldObj->CreateManager<Core::ECS::Components::Scriptable>();
    transformManager = worldObj->CreateManager<Core::ECS::Components::Transform>();
    meshManager = worldObj->CreateManager<Core::ECS::Components::Mesh>();
    materialManager = worldObj->CreateManager<Core::ECS::Components::Material>();
    cameraManager = worldObj->CreateManager<Core::ECS::Components::Camera>();
    meshRendererManager = worldObj->CreateManager<Core::ECS::Components::MeshRenderer>();
    lightManager = worldObj->CreateManager<Core::ECS::Components::Light>();
    boundManager = worldObj->CreateManager<Core::ECS::Components::Bound>();

    scriptableSystemObj = new ScriptableSystem();
    worldObj->AddSystem(scriptableSystemObj, Core::ECS::COMPONENT_TYPE::SCRIPTABLE);

    transformSystemObj = new TransformSystem();
    worldObj->AddSystem(transformSystemObj, Core::ECS::COMPONENT_TYPE::TRANSFORM);

    cameraSystemObj = new Engine::ECS::Systems::CameraSystem(
        renderData.m_cameraData, m_windowSettings);
    worldObj->AddSystem(cameraSystemObj, Core::ECS::COMPONENT_TYPE::CAMERA);

    lightSystem = new LightSystem(renderData.m_lightData);
    worldObj->AddSystem(lightSystem, Core::ECS::COMPONENT_TYPE::LIGHT);

    materialSystem = new MaterialSystem(renderData.m_materialData, renderData.m_skyboxData);
    worldObj->AddSystem(materialSystem, Core::ECS::COMPONENT_TYPE::MATERIAL);

    meshRendererSystem = new MeshRendererSystem(renderData.m_transformData, renderData.m_materialData);
    worldObj->AddSystem(meshRendererSystem, Core::ECS::COMPONENT_TYPE::MESH_RENDERER);

    boundSystem = new BoundSystem(renderData.m_boundData);
    worldObj->AddSystem(boundSystem, Core::ECS::COMPONENT_TYPE::BOUNDS);

    worldObj->Init();

    m_sceneLoader = std::make_unique<Engine::Utility::GltfLoader>(worldObj);

    std::vector<Core::ECS::EntityHandle*> entityList;
    //sceneLoader->LoadGltf("Sponza\\gltf\\Sponza.gltf", entityList);
    m_sceneLoader->LoadGltf("ABeautifulGame\\gltf\\ABeautifulGame.gltf", entityList);
}

void Engine::ECS_Manager::DeInit()
{
    PLOGD << "ECS Manager DeInit";

    m_sceneLoader.reset();

    Core::ECS::Events::EventBus::GetInstance()->DeInit();
    delete Core::ECS::Events::EventBus::GetInstance();

    worldObj->DeInit();

    delete boundSystem;
    delete materialSystem;
    delete lightSystem;
    delete meshRendererSystem;
    delete cameraSystemObj;
    delete transformSystemObj;
    delete scriptableSystemObj;

    delete worldObj;
}

void Engine::ECS_Manager::Update(float dt, const Core::Wrappers::FrameInfo& frameInfo)
{
    worldObj->Update(dt, frameInfo);
}

void Engine::ECS_Manager::Render(float dt)
{
    worldObj->Render(dt);
}

void Engine::ECS_Manager::PrepareRenderData()
{

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

//const Core::Utility::RenderData& Engine::ECS_Manager::GetRenderData()
//{
//    return m_renderData;
//}

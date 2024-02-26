#pragma once

#include <RenderData.h>
#include <SceneLoader.h>

namespace Core
{
    namespace ECS
    {
        class System;
        class World;
        namespace Components
        {
            class Transform;
            class Scriptable;
            class Mesh;
            class Material;
            class Camera;
            class MeshRenderer;
            class Light;
            class Bound;
        }

        template<typename T>
        class ComponentManager;
    }
}

namespace Engine
{
    namespace Utility
    {
        class GltfLoader;
    }

    class ECS_Manager
    {
    private:
        ECS_Manager() {}
        ECS_Manager(ECS_Manager const&) {}
        ECS_Manager const& operator= (ECS_Manager const&) {}

        static ECS_Manager* instance;
        //Core::Utility::RenderData m_renderData;
        std::unique_ptr<Engine::Utility::GltfLoader> m_sceneLoader;

    public:
        Core::ECS::ComponentManager<Core::ECS::Components::Transform>* transformManager;
        Core::ECS::System* transformSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Scriptable>* scriptableManager;
        Core::ECS::System* scriptableSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Camera>* cameraManager;
        Core::ECS::System* cameraSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Mesh>* meshManager;
        Core::ECS::ComponentManager<Core::ECS::Components::Material>* materialManager;
        Core::ECS::System* materialSystem;

        Core::ECS::ComponentManager<Core::ECS::Components::MeshRenderer>* meshRendererManager;
        Core::ECS::System* meshRendererSystem;

        Core::ECS::ComponentManager<Core::ECS::Components::Light>* lightManager;
        Core::ECS::System* lightSystem;

        Core::ECS::ComponentManager<Core::ECS::Components::Bound>* boundManager;
        Core::ECS::System* boundSystem;

        void Init(Core::Utility::RenderData& renderData, std::unique_ptr<Engine::Utility::GltfLoader>& sceneLoader);
        void DeInit();
        void Update(float dt, const Core::Wrappers::FrameInfo& frameInfo);
        void Render(float dt);
        void PrepareRenderData();
        static ECS_Manager* GetInstance();
        ~ECS_Manager();

        const Core::Utility::RenderData& GetRenderData();
    };
}
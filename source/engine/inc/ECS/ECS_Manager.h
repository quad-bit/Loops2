#pragma once

#include <RenderData.h>

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
        }

        template<typename T>
        class ComponentManager;
    }
}

namespace Engine
{
    class ECS_Manager
    {
    private:
        ECS_Manager() {}
        ECS_Manager(ECS_Manager const&) {}
        ECS_Manager const& operator= (ECS_Manager const&) {}

        static ECS_Manager* instance;

    public:
        Core::ECS::ComponentManager<Core::ECS::Components::Transform>* transformManager;
        Core::ECS::System* transformSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Scriptable>* scriptableManager;
        Core::ECS::System* scriptableSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Camera>* cameraManager;
        Core::ECS::System* cameraSystemObj;

        Core::ECS::ComponentManager<Core::ECS::Components::Mesh>* meshManager;
        Core::ECS::ComponentManager<Core::ECS::Components::Material>* materialManager;

        Core::ECS::ComponentManager<Core::ECS::Components::MeshRenderer>* meshRendererManager;
        Core::ECS::System* meshRendererSystem;

        Core::ECS::ComponentManager<Core::ECS::Components::Light>* lightManager;
        Core::ECS::System* lightSystem;

        void Init();
        void DeInit();
        void Update(float dt);
        void Render(float dt);
        void PrepareRenderData(Core::Utility::RenderData& renderData);
        /// <summary>
        /// Create the descriptor sets, uniform buffers
        /// Transform : Create Uniform buffer for TransformData if meshrenderer attached
        /// MeshRenderer : Create Vertex, index buffer
        /// Camera : Create uniform buffer for CameraData
        /// Light : Create uniform buffer for LightData
        /// Clubbing strategy : Group using MaterialType -> LOD
        /// </summary>
        void CreateSceneResources();

        static ECS_Manager* GetInstance();
        ~ECS_Manager();
    };
}
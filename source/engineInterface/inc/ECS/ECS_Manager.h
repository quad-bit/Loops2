#pragma once

class World;
class System;

namespace Core
{
    namespace ECS
    {
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

class ECS_Manager
{
private:
    ECS_Manager(){}
    ECS_Manager(ECS_Manager const &) {}
    ECS_Manager const & operator= (ECS_Manager const &) {}

    static ECS_Manager* instance;

public:
    Core::ECS::ComponentManager<Core::ECS::Components::Transform>* transformManager;
    System* transformSystemObj;

    Core::ECS::ComponentManager<Core::ECS::Components::Scriptable>* scriptableManager;
    System* scriptableSystemObj;

    Core::ECS::ComponentManager<Core::ECS::Components::Camera>* cameraManager;
    System* cameraSystemObj;

    Core::ECS::ComponentManager<Core::ECS::Components::Mesh>* meshManager;
    Core::ECS::ComponentManager<Core::ECS::Components::Material>* materialManager;

    Core::ECS::ComponentManager<Core::ECS::Components::MeshRenderer>* meshRendererManager;
    System* meshRendererSystem;

    Core::ECS::ComponentManager<Core::ECS::Components::Light>* lightManager;
    System* lightSystem;

    void Init();
    void DeInit();
    void Update(float dt);
    void Render(float dt);
    static ECS_Manager* GetInstance();
    ~ECS_Manager();
};

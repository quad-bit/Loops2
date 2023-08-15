#pragma once

class World;
class System;

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

class ECS_Manager
{
private:
    ECS_Manager(){}
    ECS_Manager(ECS_Manager const &) {}
    ECS_Manager const & operator= (ECS_Manager const &) {}

    static ECS_Manager* instance;

public:
    ECS::ComponentManager<ECS::Components::Transform>* transformManager;
    System* transformSystemObj;

    ECS::ComponentManager<ECS::Components::Scriptable>* scriptableManager;
    System* scriptableSystemObj;

    ECS::ComponentManager<ECS::Components::Camera>* cameraManager;
    System* cameraSystemObj;

    ECS::ComponentManager<ECS::Components::Mesh>* meshManager;
    ECS::ComponentManager<ECS::Components::Material>* materialManager;

    ECS::ComponentManager<ECS::Components::MeshRenderer>* meshRendererManager;
    System* meshRendererSystem;

    ECS::ComponentManager<ECS::Components::Light>* lightManager;
    System* lightSystem;

    void Init();
    void DeInit();
    void Update(float dt);
    void Render(float dt);
    static ECS_Manager* GetInstance();
    ~ECS_Manager();
};

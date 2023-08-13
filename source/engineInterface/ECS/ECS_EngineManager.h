#pragma once

class World;
class System;
class Transform;
class Scriptable;
class Mesh;
class Material;
class Camera;
class MeshRenderer;
class Light;

template<typename T>
class ComponentManager;

class ECS_Manager
{

private:
    ECS_Manager(){}
    ECS_Manager(ECS_Manager const &) {}
    ECS_Manager const & operator= (ECS_Manager const &) {}

    static ECS_Manager* instance;

public:
    
    ComponentManager<Transform>* transformManager;
    System* transformSystemObj;
    
    ComponentManager<Scriptable>* scriptableManager;
    System* scriptableSystemObj;

    ComponentManager<Camera>* cameraManager;
    System* cameraSystemObj;

    ComponentManager<Mesh>* meshManager;
    ComponentManager<Material>* materialManager;

    ComponentManager<MeshRenderer>* meshRendererManager;
    System* meshRendererSystem;

    ComponentManager<Light>* lightManager;
    System* lightSystem;

    void Init();
    void DeInit();
    void Update(float dt);
    void Render(float dt);
    static ECS_Manager* GetInstance();
    ~ECS_Manager();
};

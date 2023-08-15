#pragma once
#include "World.h"

#include <iostream>

namespace ECS
{
    namespace Components
    {
        class Transform;
    }
}

class Entity;
class World;

namespace ECS
{
    template< typename ComponentType>
    class ComponentHandle;
}

class EntityHandle
{
private:
    Entity * entityObj;
    World * worldObj;
    //Transform * transform;

public:
    EntityHandle(Entity * entityObj, World * worldObj) {
        this->entityObj = entityObj;
        //this->transform = entityObj->transform;
        this->worldObj = worldObj;
    }

    void Destroy()
    {
        worldObj->DestroyEntity(entityObj);
    }

    Entity * GetEntity() { return entityObj; }
    ECS::Components::Transform * GetTransform(){ return entityObj->transform; }

    template<typename ComponentType>
    void AddComponent(ComponentType* componentType);

    template<typename ComponentType>
    void RemoveComponent(ComponentType* componentType);

    template<typename ComponentType>
    ECS::ComponentHandle<ComponentType> GetComponent();

    ~EntityHandle()
    {
        //std::cout << "entity handle deleted \n";
    }

};

#include "ECS/Entity.h"
#include "ECS/ComponentHandle.h"
#include "ECS/Components/Camera.h"
#include "ECS/Events/ComponentAdditionEvent.h"
#include "ECS/Events/EventBus.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Scriptable.h"
#include "ECS/Components/Light.h"

template<typename ComponentType>
inline void EntityHandle::AddComponent(ComponentType * componentType)
{
    worldObj->AddComponent<ComponentType>(componentType, entityObj);
}

template<>
inline void EntityHandle::AddComponent(ECS::Components::Camera * componentType)
{
    worldObj->AddComponent<ECS::Components::Camera>(componentType, entityObj);
    
    CameraAdditionEvent evt;
    evt.cam = componentType;
    EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void EntityHandle::AddComponent(ECS::Components::MeshRenderer * componentType)
{
    worldObj->AddComponent<ECS::Components::MeshRenderer>(componentType, entityObj);

    MeshRendererAdditionEvent evt;
    evt.renderer = componentType;
    EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void EntityHandle::AddComponent(ECS::Components::Scriptable * componentType)
{
    worldObj->AddComponent<ECS::Components::Scriptable>(componentType, entityObj);
    componentType->entityHandle = this;
    componentType->Activated();
}

template<>
inline void EntityHandle::AddComponent(ECS::Components::Light * componentType)
{
    worldObj->AddComponent<ECS::Components::Light>(componentType, entityObj);

    LightAdditionEvent evt;
    evt.light = componentType;
    EventBus::GetInstance()->Publish(&evt);
}

template<typename ComponentType>
inline void EntityHandle::RemoveComponent(ComponentType * componentType)
{
    worldObj->RemoveComponent<ComponentType>(componentType, entityObj);
}

template<typename ComponentType>
inline ECS::ComponentHandle<ComponentType> EntityHandle::GetComponent()
{
    ECS::ComponentHandle<ComponentType> componentHandle;  
    worldObj->Unpack(entityObj, componentHandle);
    return componentHandle;
}


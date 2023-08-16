#pragma once
#include "World.h"

#include <iostream>

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Transform;
        }
        template< typename ComponentType>
        class ComponentHandle;
    }
}
class Entity;
class World;

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
    Core::ECS::Components::Transform * GetTransform(){ return entityObj->transform; }

    template<typename ComponentType>
    void AddComponent(ComponentType* componentType);

    template<typename ComponentType>
    void RemoveComponent(ComponentType* componentType);

    template<typename ComponentType>
    Core::ECS::ComponentHandle<ComponentType> GetComponent();

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
inline void EntityHandle::AddComponent(Core::ECS::Components::Camera * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Camera>(componentType, entityObj);
    
    CameraAdditionEvent evt;
    evt.cam = componentType;
    EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void EntityHandle::AddComponent(Core::ECS::Components::MeshRenderer * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::MeshRenderer>(componentType, entityObj);

    MeshRendererAdditionEvent evt;
    evt.renderer = componentType;
    EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void EntityHandle::AddComponent(Core::ECS::Components::Scriptable * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Scriptable>(componentType, entityObj);
    componentType->entityHandle = this;
    componentType->Activated();
}

template<>
inline void EntityHandle::AddComponent(Core::ECS::Components::Light * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Light>(componentType, entityObj);

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
inline Core::ECS::ComponentHandle<ComponentType> EntityHandle::GetComponent()
{
    Core::ECS::ComponentHandle<ComponentType> componentHandle;  
    worldObj->Unpack(entityObj, componentHandle);
    return componentHandle;
}


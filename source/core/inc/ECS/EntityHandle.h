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

        class Entity;
        class World;

        class EntityHandle
        {
        private:
            Entity* entityObj;
            World* worldObj;
            //Transform * transform;

        public:
            EntityHandle(Entity* entityObj, World* worldObj) {
                this->entityObj = entityObj;
                //this->transform = entityObj->transform;
                this->worldObj = worldObj;
            }

            void Destroy()
            {
                this->worldObj->DestroyEntity(entityObj);
            }

            Entity* GetEntity() { return entityObj; }
            Core::ECS::Components::Transform* GetTransform() { return entityObj->transform; }

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
    }
}

#include "ECS/Entity.h"
#include "ECS/ComponentHandle.h"
#include "ECS/Components/Camera.h"
#include "ECS/Events/ComponentAdditionEvent.h"
#include "ECS/Events/EventBus.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/Scriptable.h"
#include "ECS/Components/Light.h"

template<typename ComponentType>
inline void Core::ECS::EntityHandle::AddComponent(ComponentType * componentType)
{
    worldObj->AddComponent<ComponentType>(componentType, entityObj);
}

template<>
inline void Core::ECS::EntityHandle::AddComponent(Core::ECS::Components::Camera * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Camera>(componentType, entityObj);

    Core::ECS::Events::CameraAdditionEvent evt;
    evt.cam = componentType;
    Core::ECS::Events::EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void Core::ECS::EntityHandle::AddComponent(Core::ECS::Components::MeshRenderer * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::MeshRenderer>(componentType, entityObj);

    Core::ECS::Events::MeshRendererAdditionEvent evt;
    evt.renderer = componentType;
    Core::ECS::Events::EventBus::GetInstance()->Publish(&evt);
}

template<>
inline void Core::ECS::EntityHandle::AddComponent(Core::ECS::Components::Scriptable * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Scriptable>(componentType, entityObj);
    componentType->entityHandle = this;
    componentType->Activated();
}

template<>
inline void Core::ECS::EntityHandle::AddComponent(Core::ECS::Components::Light * componentType)
{
    worldObj->AddComponent<Core::ECS::Components::Light>(componentType, entityObj);

    Core::ECS::Events::LightAdditionEvent evt;
    evt.light = componentType;
    Core::ECS::Events::EventBus::GetInstance()->Publish(&evt);
}

template<typename ComponentType>
inline void Core::ECS::EntityHandle::RemoveComponent(ComponentType * componentType)
{
    worldObj->RemoveComponent<ComponentType>(componentType, entityObj);
}

template<typename ComponentType>
inline Core::ECS::ComponentHandle<ComponentType> Core::ECS::EntityHandle::GetComponent()
{
    Core::ECS::ComponentHandle<ComponentType> componentHandle;  
    worldObj->Unpack(entityObj, componentHandle);
    return componentHandle;
}


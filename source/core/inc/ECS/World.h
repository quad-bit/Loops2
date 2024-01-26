#pragma once
#include <vector>
#include <map>
#include <string>
#include "ComponentMask.h"
#include <functional>
#include "Utility/RenderingWrappers/RenderingWrapper.h"

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Camera;
        }

        template<typename ComponentType>
        class ComponentManager;

        class BaseComponentManager;

        template<typename T>
        class ComponentHandle;

        class Entity;
        class EntityHandle;
        class System;

        class World
        {
        private:
            template<typename ComponentType>
            Core::ECS::ComponentManager<ComponentType>* GetComponentManager();

            std::vector<Core::ECS::BaseComponentManager*> managerList;
            std::vector<System*> systemList;
            System* camSystem;

            std::map<Entity*, ComponentMask> entityMasks;

        public:

            //explicit World(std::unique_ptr<EntityManager> entityManager);

            void Init();

            void DestroyEntity(Entity* entityObj);
            void DestroyEntity(EntityHandle* entityHandleObj);
            EntityHandle* CreateEntity();
            EntityHandle* CreateEntity(const std::string& name);

            EntityHandle* const FindEntity(const std::string& name);
            const std::vector<Entity*>& GetEntityList();

            template<typename ComponentType>
            void AddComponent(ComponentType* componentType, Entity* entityObj);

            template<typename ComponentType>
            void RemoveComponent(ComponentType* componentType, Entity* entityObj);

            template<typename ComponentType>
            std::vector<EntityHandle*> GetEntities(ComponentType* componentType);

            void UpdateEntityMask(Entity* entity, ComponentMask oldMask);

            void AddSystem(System* system, COMPONENT_TYPE componentType);

            template<typename ComponentType>
            Core::ECS::ComponentManager<ComponentType>* CreateManager();

            /// Run a task for all the components of ComponentType
            template<typename ComponentType>
            void RunTask(std::function<void(ComponentType* componentType)> lambda)
            {
                Core::ECS::ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
                manager->iterateAll(lambda);
            }

            
            template<typename ComponentType, typename... Args>
            void Unpack(Entity* e, Core::ECS::ComponentHandle<ComponentType>& handle, Core::ECS::ComponentHandle<Args>&... args)
            {
                typedef Core::ECS::ComponentManager<ComponentType> componentManagerType;

                componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
                handle = *(manager->GetComponentHandle(e));

                Unpack<Args...>(e, args...);
            }

            template<typename ComponentType>
            void Unpack(Entity* e, Core::ECS::ComponentHandle<ComponentType>& handle)
            {
                typedef Core::ECS::ComponentManager<ComponentType> componentManagerType;

                componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
                handle = *(manager->GetComponentHandle(e));
            }

            template<typename ComponentType, typename... Args>
            void Unpack(Entity* e, Core::ECS::ComponentHandle<ComponentType>** handle, Core::ECS::ComponentHandle<Args>**... args)
            {
                typedef Core::ECS::ComponentManager<ComponentType> componentManagerType;

                componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
                *handle = (manager->GetComponentHandle(e));

                Unpack<Args...>(e, args...);
            }

            template<typename ComponentType>
            void Unpack(Entity* e, Core::ECS::ComponentHandle<ComponentType>** handle)
            {
                typedef Core::ECS::ComponentManager<ComponentType> componentManagerType;

                componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
                *handle = (manager->GetComponentHandle(e));
            }

            template<typename ComponentType>
            bool CheckForComponentType(Entity* e)
            {
                typedef Core::ECS::ComponentManager<ComponentType> componentManagerType;

                componentManagerType* manager = (componentManagerType*)GetComponentManager<ComponentType>();
                return manager->CheckForComponent(e);
            }

            void Update(float dt, const Core::Wrappers::FrameInfo& m_frameInfo);

            void Render(float dt);

            void DeInit();

            void SetMainCamera(uint32_t camComponentId);
        };
    }
}

// =========================================     ==================================================

#include "ComponentManager.h"
#include "EntityHandle.h"
#include "Entity.h"
#include "Component.h"

template<typename ComponentType>
inline Core::ECS::ComponentManager<ComponentType>* Core::ECS::World::GetComponentManager()
{
	int family = GetComponentFamily<ComponentType>();
    return (Core::ECS::ComponentManager<ComponentType>*)managerList[family];
}

template<typename ComponentType>
inline void Core::ECS::World::AddComponent(ComponentType * componentType, Entity * entityObj)
{
    Core::ECS::ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
    manager->AddComponent(componentType, entityObj);

    ComponentMask oldMask = entityMasks[entityObj];
    entityMasks[entityObj].AddComponent<ComponentType>();

    UpdateEntityMask(entityObj, oldMask);
}

template<typename ComponentType>
inline void Core::ECS::World::RemoveComponent(ComponentType * componentType, Entity * entityObj)
{
    Core::ECS::ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
    manager->RemoveComponent(entityObj);

    ComponentMask oldMask = entityMasks[entityObj];
    entityMasks[entityObj].RemoveComponent<ComponentType>();

    UpdateEntityMask(entityObj, oldMask);
}

template<typename ComponentType>
std::vector<Core::ECS::EntityHandle*> Core::ECS::World::GetEntities(ComponentType* componentType)
{
    Core::ECS::ComponentManager<ComponentType>* manager = GetComponentManager<ComponentType>();
    manager-> (entityObj);
}

template<typename ComponentType>
inline Core::ECS::ComponentManager<ComponentType>* Core::ECS::World::CreateManager() // TODO ..Create the managers properly, hacked for testing..!!!
{
    Core::ECS::ComponentManager<ComponentType>* manager = new Core::ECS::ComponentManager<ComponentType>();
    managerList.push_back(manager);

	AssignFamily<ComponentType>();
    return manager;
}








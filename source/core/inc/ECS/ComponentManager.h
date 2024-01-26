#pragma once

#include <map>
#include <array>
#include <functional>
#include "ECS_Setting.h"
#include "CorePrecompiled.h"
//using namespace std;

namespace Core
{
    namespace ECS
    {
        class EntityMap;
        class Entity;

        template <typename ComponentType>
        class ComponentHandle;

        using ComponentIndex = uint32_t;

        template <typename ComponentType>
        class ComponentData
        {
        public:
            uint32_t size = 1;
            std::array<ComponentType*, MAX_NUM_OF_UNIQUE_COMPONENT>* data;
        };

        class BaseComponentManager {
        public:
            BaseComponentManager() = default;
            virtual ~BaseComponentManager() = default;
            BaseComponentManager(const BaseComponentManager&) = default;
            BaseComponentManager& operator=(const BaseComponentManager&) = default;
            BaseComponentManager(BaseComponentManager&&) = default;
            BaseComponentManager& operator=(BaseComponentManager&&) = default;
        };

        template <typename ComponentType>
        class ComponentManager : public BaseComponentManager
        {
        private:
            ComponentData<ComponentType> componentDataObj;
            EntityMap* entityMapObj;

        public:

            using LookupType = ComponentType;

            ComponentManager();
            ComponentIndex AddComponent(ComponentType* componentType, Entity* entity);
            void RemoveComponent(Entity* entity);
            bool CheckForComponent(Entity* entity);
            ComponentType* GetComponent(Entity* entity);
            ComponentHandle<ComponentType>* GetComponentHandle(Entity* entity);
            void iterateAll(std::function<void(ComponentType* componentType)> lambda);
            std::vector<Entity*> GetEntities(ComponentType* componentType);
            ~ComponentManager();
        };
    }
}

#include "ECS/Entity.h"
#include "ECS/EntityMap.h"
#include "ECS/ComponentHandle.h"
#include "ECS/Components/Material.h"

template<typename ComponentType>
inline Core::ECS::ComponentManager<ComponentType>::ComponentManager()
{
    PLOGD << "ComponentManager contructor";
    componentDataObj.data = static_cast<std::array<ComponentType*, MAX_NUM_OF_UNIQUE_COMPONENT>* >(malloc(sizeof(ComponentType) * MAX_NUM_OF_UNIQUE_COMPONENT));
    entityMapObj = new EntityMap();
}

template<typename ComponentType>
inline Core::ECS::ComponentManager<ComponentType>::~ComponentManager()
{
    PLOGD << "ComponentManager destructor";

    for (uint32_t i = 0; i < MAX_NUM_OF_UNIQUE_COMPONENT; i++)
    {
        if(componentDataObj.data->at(i) == NULL)
            delete componentDataObj.data->at(i);
    }

    free(componentDataObj.data);
    //TODO : Delete contents of the array componentDataObj.data, allocation is deleted but the pointer 
    //variable stays
    delete entityMapObj;
}

template<typename ComponentType>
inline Core::ECS::ComponentIndex Core::ECS::ComponentManager<ComponentType>::AddComponent(ComponentType * componentType, Core::ECS::Entity * entity)
{
    Core::ECS::ComponentIndex ind = componentDataObj.size;
    componentDataObj.data->at(ind) = componentType;

    entityMapObj->AddToMap(entity, &ind);
    componentDataObj.size++;

    return ind;
}

template<>
inline Core::ECS::ComponentIndex Core::ECS::ComponentManager<Core::ECS::Components::Material>::AddComponent(Core::ECS::Components::Material * componentType, Entity * entity)
{
    // find if material exists

    auto it = componentDataObj.data->end();

    //if (componentDataObj.size > 1)
    {
        /*it = std::find_if(componentDataObj.data->begin() + 1, componentDataObj.data->begin() + componentDataObj.size,
            [=](Core::ECS::Components::Material* e)
        {
            if (e == nullptr)
                return false;
            return e->componentId == componentType->componentId; 
        });*/
    }

    bool found = false;
    Core::ECS::ComponentIndex ind;

    for (uint32_t i = 1; i < componentDataObj.size; i++)
    {
        if (componentDataObj.data->at(i)->componentId == componentType->componentId)
        {
            found = true;
            ind = i;
            break;
        }
    }

    if (found)
    {
        entityMapObj->AddToMap(entity, &ind);
    }
    else
    {
        ind = componentDataObj.size;
        componentDataObj.data->at(ind) = componentType;

        entityMapObj->AddToMap(entity, &ind);
        componentDataObj.size++;
    }

    return ind;
}

template<typename ComponentType>
std::vector<Core::ECS::Entity*> Core::ECS::ComponentManager<ComponentType>::GetEntities(ComponentType* componentType)
{
    Core::ECS::ComponentIndex ind = entityMapObj->GetComponentIndex(e);
    return entityMapObj->GetEntities(ind);
}

template<typename ComponentType>
inline void Core::ECS::ComponentManager<ComponentType>::RemoveComponent(Core::ECS::Entity * e)
{
    if (componentDataObj.size > 1)
    {
        Core::ECS::ComponentIndex ind = entityMapObj->GetComponentIndex(e);
        Core::ECS::ComponentIndex lastIndex = componentDataObj.size;

        componentDataObj.data->at(ind) = componentDataObj.data->at(lastIndex);
        componentDataObj.size--;
    }
    //auto lastEntities = entityMapObj->GetEntities(lastIndex);

    entityMapObj->RemoveFromMap(e);
    //entityMapObj->UpdateMap(lastEntities, &ind);

}

template<typename ComponentType>
inline bool Core::ECS::ComponentManager<ComponentType>::CheckForComponent(Entity* entity)
{
    return entityMapObj->DoesComponentExist(entity);
}

template<typename ComponentType>
inline ComponentType* Core::ECS::ComponentManager<ComponentType>::GetComponent(Core::ECS::Entity * entity)
{
    Core::ECS::ComponentIndex ind = entityMapObj->GetComponentIndex(entity);
    return componentDataObj.data->at(ind);
}

template<typename ComponentType>
inline Core::ECS::ComponentHandle<ComponentType>* Core::ECS::ComponentManager<ComponentType>::GetComponentHandle(Core::ECS::Entity * entity)
{
    Core::ECS::ComponentIndex ind = entityMapObj->GetComponentIndex(entity);
    Core::ECS::ComponentHandle<ComponentType>* handle = new Core::ECS::ComponentHandle<ComponentType>(this, entity, ind);

    return handle;
}

template<typename ComponentType>
inline void Core::ECS::ComponentManager<ComponentType>::iterateAll(std::function<void(ComponentType* componentType)> lambda)
{
    for (int i = 1; i<componentDataObj.size; i++)
    {
        lambda(componentDataObj.data[i]);
    }
}

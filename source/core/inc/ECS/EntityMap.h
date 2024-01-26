#pragma once
#include <map>
#include <array>
#include "ECS_Setting.h"
#include "Entity.h"
#include <assert.h>
#include <optional>

namespace Core
{
    namespace ECS
    {
        class Entity;

        using ComponentIndex = uint32_t;

        class EntityMap
        {

        private:
            //std::array<Entity*, MAX_NUM_OF_UNIQUE_COMPONENT> indexToEntity;
            //std::map<Entity*, ComponentIndex> entityToIndex;

            //as multiple enities can have the same material
            std::map<ComponentIndex, std::vector<Entity*>> m_indexToEntitiesMap;
            std::map<Entity*, ComponentIndex> m_entityToIndexMap;

        public:

            bool DoesComponentExist(Entity* e)
            {
                /*std::map<Entity*, ComponentIndex>::iterator it = entityToIndex.find(e);

                if (it == entityToIndex.end())
                {
                    return false;
                }
                return true;*/

                if (m_entityToIndexMap.find(e) == m_entityToIndexMap.end())
                {
                    return false;
                }
                return true;
            }

            ComponentIndex& GetComponentIndex(Entity* e)
            {
                /*std::map<Entity*, ComponentIndex>::iterator it = entityToIndex.find(e);

                if (it == entityToIndex.end())
                {
                    assert(0 && "Entity not found in the map ");
                    std::exit(-1);
                }

                return entityToIndex[e];*/

                auto it = m_entityToIndexMap.find(e);

                if (m_entityToIndexMap.find(e) == m_entityToIndexMap.end())
                {
                    ASSERT_MSG_DEBUG(0, "Component not found");
                }

                return it->second;
            }

            //Entity* GetEntity(ComponentIndex* c)
            //{
            //    /* std::array<Entity, MAX_NUM_OF_COMPONENTS>::iterator it = std::find_if(std::begin(indexToEntity), std::end(indexToEntity), [=](const EntityMap& e) {return e.indexToEntity. == valueToCompare; });

            //        if (it == std::end(indexToEntity))
            //        {
            //            assert(0 && "Entity not found in the map ");
            //            std::exit(-1);
            //        }*/

            //    return indexToEntity[*c];
            //}

            std::vector<Entity*> GetEntities(const ComponentIndex& ind)
            {
                ASSERT_MSG_DEBUG(m_indexToEntitiesMap.find(ind) != m_indexToEntitiesMap.end(), "Component not found");
                return m_indexToEntitiesMap[ind];
            }

            void AddToMap(Entity* e, ComponentIndex* c)
            {
                /*entityToIndex.insert({ e, *c });
                indexToEntity[*c] = e;*/

                if (m_indexToEntitiesMap.find(*c) == m_indexToEntitiesMap.end())
                {
                    m_indexToEntitiesMap.insert({ *c, {e} });
                }
                else
                {
                    m_indexToEntitiesMap[*c].push_back(e);
                }

                m_entityToIndexMap.insert({ e, *c });
            }

            void UpdateMap(std::vector<Entity*> entities, ComponentIndex* c)
            {
                for(auto e : entities)
                    m_entityToIndexMap[e] = *c;

                m_indexToEntitiesMap[*c] = entities;

                //entityToIndex[e] = *c;
                //indexToEntity[*c] = e;
            }

            void RemoveFromMap(Entity* e)
            {
                //entityToIndex.erase(e);
                
                auto ind = m_entityToIndexMap[e];
                auto& list = m_indexToEntitiesMap[ind];
                auto it = std::find(list.begin(), list.end(), e);
                ASSERT_MSG_DEBUG(it != list.end(), "entity not found");
                list.erase(it);
                m_entityToIndexMap.erase(e);
            }
        };
    }
}

#pragma once
#include <map>
#include <array>
#include "ECS_Setting.h"
#include "Entity.h"
#include <assert.h>

class Entity;

using ComponentIndex = uint32_t;

class EntityMap
{

private:
    std::array<Entity * , MAX_NUM_OF_UNIQUE_COMPONENT> indexToEntity;
    std::map<Entity *, ComponentIndex> entityToIndex;

public:
    ComponentIndex& GetComponentIndex(Entity* e)
    {
        std::map<Entity* , ComponentIndex>::iterator it = entityToIndex.find(e);

        if (it == entityToIndex.end())
        {
            assert(0 && "Entity not found in the map ");
            std::exit(-1);
        }

        return entityToIndex[e];
    }

    Entity* GetEntity(ComponentIndex* c)
    {
        /* std::array<Entity, MAX_NUM_OF_COMPONENTS>::iterator it = std::find_if(std::begin(indexToEntity), std::end(indexToEntity), [=](const EntityMap& e) {return e.indexToEntity. == valueToCompare; });

            if (it == std::end(indexToEntity))
            {
                assert(0 && "Entity not found in the map ");
                std::exit(-1);
            }*/

        return indexToEntity[*c];
    }

    void AddToMap(Entity* e, ComponentIndex* c)
    {
        entityToIndex.insert({ e, *c });
        indexToEntity[*c] = e;
    }

    void UpdateMap(Entity *e, ComponentIndex *c)
    {
        entityToIndex[e] = *c;
        indexToEntity[*c] = e;
    }

    void RemoveFromMap(Entity *e)
    {
        entityToIndex.erase(e);
        //indexToEntity    THINK..!!!!
    }
};

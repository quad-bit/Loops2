#pragma once
#include <vector>

class Entity;
class EntityHandle;
class World;

class EntityManager
{
public:
    Entity* CreateEntity();
    EntityHandle * CreateEntityHandle(Entity* obj, World * worldObj);
    EntityHandle * const FindEntity(const std::string & name);
    void DestroyEntity(Entity* entity);

    static EntityManager* GetSingleton();
    void Init();
    void DeInit();

private:
    int lastEntity = 0;

    std::vector <Entity*> entityList;
    std::vector <EntityHandle*> entityHandleList;

    EntityManager();
    EntityManager(EntityManager const &) {}
    EntityManager const & operator= (EntityManager const &) {}

    static EntityManager* entityManagerInstance;


};


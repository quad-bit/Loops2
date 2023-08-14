#pragma once
#include <iostream>
#include <string>

//using namespace std;
namespace ECS
{
    namespace Components
    {
        class Transform;
    }
}

class Entity
{
private:

public:
    uint32_t id;

    ECS::Components::Transform * transform;
    std::string entityName;
	std::string entityTag ;
	std::string entityLayer ;
    bool isStatic;

    // To put it in maps
    friend bool operator<(const Entity &l, const Entity &r) { return l.id < r.id; }

    ~Entity()
    {
    }

};

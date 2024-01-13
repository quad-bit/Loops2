#pragma once

#include <vector>

namespace Core
{
    namespace ECS
    {
        class EntityHandle;
        namespace Components
        {
            class Scriptable;
        }
    }
}

class ApplicationController
{
private:
    Core::ECS::EntityHandle* mainObject;
    Core::ECS::Components::Scriptable* sceneManagerScript;

    std::vector< Core::ECS::EntityHandle*> m_entityList;

public:
    void Init();
    void Update();
    void DeInit();
};

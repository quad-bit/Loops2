#pragma once

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

public:
    void Init();
    void Update();
    void DeInit();
};

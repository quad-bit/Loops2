#pragma once

class EntityHandle;
namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Scriptable;
        }
    }
}

class ApplicationController
{
private:
    EntityHandle* mainObject;
    Core::ECS::Components::Scriptable* sceneManagerScript;

public:
    void Init();
    void Update();
    void DeInit();
};

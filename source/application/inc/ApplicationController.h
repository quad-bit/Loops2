#pragma once

class EntityHandle;
namespace ECS
{
    namespace Components
    {
        class Scriptable;
    }
}

class ApplicationController
{
private:
    EntityHandle* mainObject;
    ECS::Components::Scriptable* sceneManagerScript;

public:
    void Init();
    void Update();
    void DeInit();
};

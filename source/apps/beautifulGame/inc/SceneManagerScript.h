#pragma once

#include <ECS/Components/Scriptable.h>
#include <array>

class CameraController;

namespace Core
{
    namespace ECS
    {
        class EntityHandle;
        namespace Components
        {
            class MeshRenderer;
            class Light;
        }
    }
}

#define NUM_LIGHTS 5
class SceneManagerScript : public Core::ECS::Components::Scriptable
{
private:
    Core::ECS::EntityHandle* playerObject;

    Core::ECS::EntityHandle* camHandle0;
    Core::ECS::EntityHandle* camHandle1;
    Core::ECS::Components::Scriptable * cameraController;

    //EntityHandle* playerHandle;
    //Core::ECS::Components::Scriptable* playerHandlerScript;

    std::array< Core::ECS::EntityHandle*, NUM_LIGHTS> lightHandles;
    std::array< Core::ECS::Components::Light*, NUM_LIGHTS> lightComponents;

    float prevAngle, currentAngle;
    void CreateLights();
public:
    SceneManagerScript();
    virtual ~SceneManagerScript();

    void Init() override;
    void Update(float dt) override;
    void Render(float dt) override;
    void DeInit() override;
};

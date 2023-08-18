#pragma once

#include <ECS/Components/Scriptable.h>

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

class SceneManagerScript : public Core::ECS::Components::Scriptable
{
private:
    Core::ECS::EntityHandle* playerObject;

    Core::ECS::EntityHandle* camHandle0;
    Core::ECS::EntityHandle* camHandle1;
    Core::ECS::Components::Scriptable * cameraController;
    
    //EntityHandle* playerHandle;
    //Core::ECS::Components::Scriptable* playerHandlerScript;
    
    Core::ECS::EntityHandle* lightHandle;
    Core::ECS::Components::Light * lightComponent;
    Core::ECS::Components::MeshRenderer * lightDebugRenderer;
    
    Core::ECS::EntityHandle* floorHandle, *wallHandle;
    Core::ECS::Components::MeshRenderer * floorRenderer, *wallRenderer;

    std::vector<Core::ECS::EntityHandle*> boxHandles;
    std::vector<Core::ECS::Components::MeshRenderer*> boxRenderer;
    const uint32_t numBoxes = 10;

    float prevAngle, currentAngle;

public:
    SceneManagerScript();
    virtual ~SceneManagerScript();

    void Init() override;
    void Update(float dt) override;
    void Render(float dt) override;
    void DeInit() override;
};

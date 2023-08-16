#pragma once

#include <ECS/Components/Scriptable.h>

class EntityHandle;
class CameraController;

namespace Core
{
    namespace ECS
    {
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
    EntityHandle* playerObject;

    EntityHandle* camHandle0;
    EntityHandle* camHandle1;
    Core::ECS::Components::Scriptable * cameraController;
    
    //EntityHandle* playerHandle;
    //Core::ECS::Components::Scriptable* playerHandlerScript;
    
    EntityHandle* lightHandle;
    Core::ECS::Components::Light * lightComponent;
    Core::ECS::Components::MeshRenderer * lightDebugRenderer;
    
    EntityHandle* floorHandle, *wallHandle;
    Core::ECS::Components::MeshRenderer * floorRenderer, *wallRenderer;

    std::vector<EntityHandle*> boxHandles;
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

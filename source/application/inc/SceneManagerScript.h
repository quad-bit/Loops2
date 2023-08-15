#pragma once

#include <ECS/Components/Scriptable.h>

class EntityHandle;
class CameraController;

namespace ECS
{
    namespace Components
    {
        class MeshRenderer;
        class Light;
    }
}

class SceneManagerScript : public ECS::Components::Scriptable
{
private:
    EntityHandle* playerObject;

    EntityHandle* camHandle0;
    EntityHandle* camHandle1;
    ECS::Components::Scriptable * cameraController;
    
    //EntityHandle* playerHandle;
    //ECS::Components::Scriptable* playerHandlerScript;
    
    EntityHandle* lightHandle;
    ECS::Components::Light * lightComponent;
    ECS::Components::MeshRenderer * lightDebugRenderer;
    
    EntityHandle* floorHandle, *wallHandle;
    ECS::Components::MeshRenderer * floorRenderer, *wallRenderer;

    std::vector<EntityHandle*> boxHandles;
    std::vector<ECS::Components::MeshRenderer*> boxRenderer;
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

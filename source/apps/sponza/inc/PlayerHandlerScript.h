#pragma once
#if 0
#include <ECS/Components/Scriptable.h>

class EntityHandle;
class CameraController;
namescpace Core
{
namespace ECS
    {
        namespace Components
        {
            class MeshRenderer;
        }
    }
}

class PlayerHandlerScript : public Core::ECS::Components::Scriptable
{
private:
    EntityHandle* playerHandle;
    EntityHandle* head;
    EntityHandle* torso;
    EntityHandle* leftArm;
    EntityHandle* rightArm;
    EntityHandle* rightLeg;
    EntityHandle* leftLeg;

    Core::ECS::Components::MeshRenderer * headMeshRenderer;
    Core::ECS::Components::MeshRenderer * torsoMeshRenderer;
    Core::ECS::Components::MeshRenderer * leftArmMeshRenderer;
    Core::ECS::Components::MeshRenderer * rightArmMeshRenderer;
    Core::ECS::Components::MeshRenderer * rightLegMeshRenderer;
    Core::ECS::Components::MeshRenderer * leftLegMeshRenderer;

    Core::ECS::Components::Scriptable * cameraController;

    float prevAngle, currentAngle;

public:
    PlayerHandlerScript();
    void Init() override;
    void Update(float dt) override;
    void Render(float dt) override;
    void DeInit() override;
    virtual ~PlayerHandlerScript();
};

#endif
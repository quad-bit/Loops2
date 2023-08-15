#pragma once
#if 0
#include <ECS/Components/Scriptable.h>

class EntityHandle;
class CameraController;

namespace ECS
{
    namespace Components
    {
        class MeshRenderer;
    }
}

class PlayerHandlerScript : public ECS::Components::Scriptable
{
private:
    EntityHandle* playerHandle;
    EntityHandle* head;
    EntityHandle* torso;
    EntityHandle* leftArm;
    EntityHandle* rightArm;
    EntityHandle* rightLeg;
    EntityHandle* leftLeg;

    ECS::Components::MeshRenderer * headMeshRenderer;
    ECS::Components::MeshRenderer * torsoMeshRenderer;
    ECS::Components::MeshRenderer * leftArmMeshRenderer;
    ECS::Components::MeshRenderer * rightArmMeshRenderer;
    ECS::Components::MeshRenderer * rightLegMeshRenderer;
    ECS::Components::MeshRenderer * leftLegMeshRenderer;

    ECS::Components::Scriptable * cameraController;

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
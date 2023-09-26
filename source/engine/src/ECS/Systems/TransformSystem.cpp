#include "ECS/Systems/TransformSystem.h"
#include <ECS/Components/Transform.h>
#include <ECS/World.h>
#include "SceneGraphManager.h"

// deprecated
void TransformSystem::UpdateTransform(Core::ECS::Components::Transform * transform)
{
    DEPRECATED;
    /*
    if (transform->GetParent() != nullptr)
    {
        transform->translationMat = glm::translate(transform->localPosition);
        transform->scaleMat = glm::scale(transform->localScale);

        glm::mat4 rotXMat = glm::rotate(transform->localEulerAngle.x, glm::vec3(1, 0, 0));
        glm::mat4 rotYMat = glm::rotate(transform->localEulerAngle.y, glm::vec3(0, 1, 0));
        glm::mat4 rotZMat = glm::rotate(transform->localEulerAngle.z, glm::vec3(0, 0, 1));

        transform->rotationMat = rotZMat * rotYMat * rotXMat;
        transform->globalModelMatrix = transform->translationMat * transform->rotationMat * transform->scaleMat;
        transform->localModelMatrix = transform->globalModelMatrix;
    }
    else
    {
        transform->translationMat = glm::translate(transform->localPosition);
        transform->scaleMat = glm::scale(transform->localScale);

        glm::mat4 rotXMat = glm::rotate(transform->localEulerAngle.x, glm::vec3(1, 0, 0));
        glm::mat4 rotYMat = glm::rotate(transform->localEulerAngle.y, glm::vec3(0, 1, 0));
        glm::mat4 rotZMat = glm::rotate(transform->localEulerAngle.z, glm::vec3(0, 0, 1));

        transform->rotationMat = rotZMat * rotYMat * rotXMat;
        transform->globalModelMatrix = transform->translationMat * transform->rotationMat * transform->scaleMat;
        transform->localModelMatrix = transform->GetParent()->localModelMatrix * transform->globalModelMatrix;
    }
    */
}

void TransformSystem::Init()
{
}

void TransformSystem::DeInit()
{
}

void TransformSystem::Update(float dt)
{
    // Do the scene tree traversal
    // Update the tranform wrt to the parent
    Engine::SceneGraphManager::GetInstance()->Update();
}

TransformSystem::TransformSystem()
{
    signature.AddComponent<Core::ECS::Components::Transform>();
}


TransformSystem::~TransformSystem()
{
}

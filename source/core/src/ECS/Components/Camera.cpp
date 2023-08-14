#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"

ECS::Components::Camera::Camera(ECS::Components::Transform * transform, glm::vec3 up, float yaw, float pitch, CameraType projectionType) : movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), fov(FOV)
{
    this->transform = transform;
    //this->position = position;
    //.this->eulerAngles = eulerAngles;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->aspect = (float)(Settings::windowWidth / Settings::windowHeight);
    this->projectionType = projectionType;
    //this->updateCameraVectors();

    componentType = COMPONENT_TYPE::CAMERA;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 ECS::Components::Camera::GetViewMatrix()
{
    glm::vec3 localForward = transform->GetForward();
    glm::vec3 localUp = transform->GetUp();
    viewMat = glm::lookAt(transform->GetLocalPosition(), 
        transform->GetLocalPosition() + localForward, 
        localUp );
    return viewMat;
}

glm::mat4 ECS::Components::Camera::GetProjectionMat()
{
    glm::mat4 clip = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f
    );
    switch (projectionType)
    {
    case CameraType::ORTHOGONAL:
        //ASSERT_MSG_DEBUG(0, "Need the correct design");
        //projectionMat = glm::ortho( (glm::radians(this->fov), this->aspect, this->zNear, this->zFar);

        break;

    case CameraType::PERSPECTIVE:
        projectionMat = glm::perspective(glm::radians(this->fov), this->aspect, this->zNear, this->zFar);
        break;
    }
    return projectionMat;
}

const glm::vec3* ECS::Components::Camera::GetPosition()
{
    return transform->GetLocalPositionRef();
}

glm::vec3 ECS::Components::Camera::GetFront()
{
    return transform->GetForward();
}

glm::vec3 ECS::Components::Camera::GetUp()
{
    return transform->GetUp();
}

glm::vec3 ECS::Components::Camera::GetRight()
{
    return transform->GetRight();
}

glm::vec3& ECS::Components::Camera::GetWorlUp()
{
    return this->worldUp;
}

float& ECS::Components::Camera::GetYaw()
{
    return this->yaw;
}

float& ECS::Components::Camera::GetPitch()
{
    return this->pitch;
}

float& ECS::Components::Camera::GetMovementSpeed()
{
    return this->movementSpeed;
}

float& ECS::Components::Camera::GetMouseSensitivity()
{
    return this->mouseSensitivity;
}

float& ECS::Components::Camera::GetFOV()
{
    return this->fov;
}

void ECS::Components::Camera::SetFOV(const float & fov)
{
    this->fov = fov;
}

void ECS::Components::Camera::SetNearPlane(const float & near)
{
    this->zNear = near;
}

void ECS::Components::Camera::SetFarPlane(const float & far)
{
    this->zFar = far;
}

void ECS::Components::Camera::SetProjectionType(const CameraType & type)
{
    this->projectionType = type;
}

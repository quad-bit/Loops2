#pragma once

#include <vector>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include "Settings.h"
#include "Platform/Assertion.h"
#include "ECS/Component.h"
#include "Math/MathUtil.h"

class Transform;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 6.0f;
const float SENSITIVTY = 0.25f;
const float FOV = 60.0f;

enum class CameraType
{
    PERSPECTIVE,
    ORTHOGONAL
};

struct CameraUniform
{
    glm::mat4 viewMat;
    glm::mat4 projectionMat;
    glm::vec3 cameraPos;
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera : public Component<Camera>
{
private:
    // Camera Attributes
    //glm::vec3* position;
    //glm::vec3* eulerAngles;


    // Directions
    //glm::vec3 front;
    //glm::vec3 up;
    //glm::vec3 right;
    glm::vec3 worldUp;
    glm::mat4 viewMat;
    glm::mat4 projectionMat;

    // Eular Angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float fov;
    float aspect;
    float zNear = 1.0f, zFar = 1000.0f;
    CameraType projectionType;

    void * cameraDataRaw;

    Camera() = delete;

public:

    Transform * transform;
    // Constructor with vectors
    Camera(Transform * transform, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, CameraType projectionType = CameraType::PERSPECTIVE);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();
    
    glm::mat4 GetProjectionMat();
    
    const glm::vec3* GetPosition();
    
    glm::vec3 GetFront();
    
    glm::vec3 GetUp();
    
    glm::vec3 GetRight();
    
    glm::vec3& GetWorlUp();
    
    float& GetYaw();
    
    float& GetPitch();
    
    float& GetMovementSpeed();
    
    float& GetMouseSensitivity();
    
    float& GetFOV();

    void SetFOV( const float & FOV);

    void SetNearPlane(const float & near);

    void SetFarPlane(const float & far);

    void SetProjectionType(const CameraType & type);
};
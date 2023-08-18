#include "CameraController.h"
#include "ECS/Events/EventBus.h"
#include "ECS/Entity.h"
#include "ECS/Components/Transform.h"
#include "glm/glm.hpp"
#include "Math/MathUtil.h"
#include "ECS/EntityHandle.h"
#include "ECS/Components/Camera.h"
#include <CorePrecompiled.h>

glm::vec2 CameraController::TransformMouse(const glm::vec2 & mouse)
{
    glm::vec2 mouseScreenCoord = mouse;
    float x, y;
    if (mouseScreenCoord.x > 0 && mouseScreenCoord.x < Settings::windowWidth &&
        mouseScreenCoord.y > 0 && mouseScreenCoord.y < Settings::windowHeight)
    {
        x = Core::Math::RangeConversion<float>(mouseScreenCoord.x / Settings::windowWidth, -1, 1);
        y = -Core::Math::RangeConversion<float>(mouseScreenCoord.y / Settings::windowHeight, -1, 1);

        currentMousePos = glm::vec2(x, y);
    }
    return glm::vec2(x, y);
}

void CameraController::Rotate()
{
    float sensitivity = 1.102f;
    Core::ECS::Components::Transform* transform = cam->transform;
    glm::vec3 angles = transform->GetLocalEulerAngles();

    float deltaX = currentMousePos.x - prevMousePos.x;
    float deltaY = currentMousePos.y - prevMousePos.y;

    absDeltaX += glm::abs(deltaX);
    absDeltaY += glm::abs(deltaY);

    if (absDeltaX > 0.0001f)
    {
        transform->SetLocalEulerAngles(
            glm::vec3(angles.x, angles.y + deltaX, angles.z));

        mouseDelta = glm::vec2(0, 0);
        deltaX = 0.0f;
        deltaY = 0.0f;
        absDeltaX = 0.0;
        absDeltaY = 0.0;
    }

}

void CameraController::Revolve()
{
    glm::vec3 currentPosition = cam->transform->GetLocalPosition();
    glm::vec3 currentEulerAngle = cam->transform->GetLocalEulerAngles();
    glm::vec3 center = glm::vec3(0, 0, 0) + glm::vec3(0, currentPosition.y, 0);
    // x = rcost
    // z = rsint
    float radius = glm::distance(center, currentPosition);

    static float angle = 0.0f;
    float offset = 0.086f;

    if(currentMousePos.x > 0)
    {
        angle += offset;
        cam->transform->SetLocalEulerAngles(glm::vec3(currentEulerAngle.x, currentEulerAngle.y + glm::radians(offset), currentEulerAngle.z));
    }
    else
    {
        //PLOGD << "-";

        angle -= offset;
        cam->transform->SetLocalEulerAngles(glm::vec3(currentEulerAngle.x, currentEulerAngle.y - glm::radians(offset), currentEulerAngle.z));
    }

    float z = radius * glm::cos(glm::radians(angle));
    float x = radius * glm::sin(glm::radians(angle));

    cam->transform->SetLocalPosition(glm::vec3(x, currentPosition.y, z));
}

void CameraController::Activated()
{
    cam = entityHandle->GetComponent<Core::ECS::Components::Camera>().GetComponent();
}

void CameraController::Init()
{
}

void CameraController::Update(float dt)
{
    /*
    if (*state == KeyState::PRESSED)
    {
        prevMousePos = currentMousePos;
        *state = KeyState::DOWN;
        mouseDelta = glm::vec2(0, 0);
        absDeltaX = 0.0;
        absDeltaY = 0.0;
        previousIntervaledMousePos = currentMousePos;
    }
    else if (*state == KeyState::DOWN)
    {
        switch (button)
        {
        case MouseButtons::Left:
            Rotate();
            break;

        case MouseButtons::Right:
            Revolve();
            break;
        }

        prevMousePos = currentMousePos;
    }*/
}

void CameraController::DeInit()
{
}

CameraController::CameraController() : Core::ECS::Components::Scriptable(true)
{
    /*EventBus::GetInstance()->Subscribe(this, &CameraController::KeyBoardEventHandler);
    EventBus::GetInstance()->Subscribe(this, &CameraController::MouseDragEventHandler);
    EventBus::GetInstance()->Subscribe(this, &CameraController::MouseButtonEventHandler);
    state = new KeyState;*/
}

CameraController::~CameraController()
{
    //delete state;
}
/*
void CameraController::MouseDragEventHandler(MouseDragEvent * evt)
{
    //return;
    glm::vec2 mouseScreenCoord = glm::vec2(evt->x, evt->y);
    
    if (mouseScreenCoord.x > 0 && mouseScreenCoord.x < Settings::windowWidth &&
        mouseScreenCoord.y > 0 && mouseScreenCoord.y < Settings::windowHeight)
    {
        float x = MathUtil::RangeConversion<float>(mouseScreenCoord.x / Settings::windowWidth, -1, 1);
        float y = -MathUtil::RangeConversion<float>(mouseScreenCoord.y / Settings::windowHeight, -1, 1);
        
        currentMousePos = glm::vec2(x, y);
    }
}

void CameraController::MouseButtonEventHandler(MouseButtonEvent * evt)
{
    switch (evt->keyState)
    {
    case KeyState::DOWN:
        PLOGD << "down";
        break;

    case KeyState::PRESSED:
        //PLOGD << "pressed";
        *state = KeyState::PRESSED;
        currentMousePos = TransformMouse( glm::vec2(evt->mouseX, evt->mouseY));
        button = evt->button;
        break;

    case KeyState::RELEASED:
        //PLOGD << "released";
        *state = KeyState::RELEASED;
        button = MouseButtons::None;
        break;
    }
}

void CameraController::KeyBoardEventHandler(KeyInputEvent * evt)
{
    const char * keyname = evt->keyname;

    const char * w = "W";
    const char * a = "A";
    const char * s = "S";
    const char * d = "D";

    switch (evt->keyState)
    {
    case KeyState::DOWN:
    case KeyState::PRESSED:
        {
            Core::ECS::Components::Transform * trf = entityHandle->GetTransform();
            //move the camera
            if (strcmp(w, evt->keyname) == 0)
            {
                glm::vec3 pos = cam->GetFront() * velocity + trf->GetLocalPosition();
                trf->SetLocalPosition(pos);
            }
            else if (strcmp(a, evt->keyname) == 0)
            {
                glm::vec3 pos = -cam->GetRight() * velocity + trf->GetLocalPosition();
                trf->SetLocalPosition(pos);
            }
            else if (strcmp(s, evt->keyname) == 0)
            {
                glm::vec3 pos = -cam->GetFront() * velocity + trf->GetLocalPosition();
                trf->SetLocalPosition(pos);
            }
            else if (strcmp(d, evt->keyname) == 0)
            {
                glm::vec3 pos = cam->GetRight() * velocity + trf->GetLocalPosition();
                trf->SetLocalPosition(pos);
            }
        }
        break;
    }
}

*/

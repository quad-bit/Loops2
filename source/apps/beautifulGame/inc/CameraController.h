#pragma once

#include "ECS/Components/Scriptable.h"
#include "ECS/Components/Camera.h"

#include "ECS/Events/Event.h"
#include "windowing/InputEvents.h"

namespace Renderer
{
    namespace Windowing
    {
        struct MouseDragEvent;
        struct KeyInputEvent;
        struct MouseButtonEvent;
        enum class KeyState;
    }
}

class CameraController : public Core::ECS::Components::Scriptable
{
private:
    Core::ECS::Components::Camera * cam;
    float velocity = .4f;
    //ComponentHandle<Camera> camHandle;

    bool dragStartred = false;
    glm::vec2 currentMousePos, prevMousePos;

    glm::vec2 mouseDelta;
    bool mousePressed = false;
    Renderer::Windowing::KeyState * state;
    float absDeltaX;
    float absDeltaY;
    Renderer::Windowing::MouseButtons button;
    glm::vec2 previousIntervaledMousePos;

    glm::vec2 TransformMouse(const glm::vec2 & mouse);

    void Rotate();
    void Revolve();

public:
    void Activated() override;
    void Init() override;
    void Update(float dt) override;
    void DeInit() override;

    CameraController();
    virtual ~CameraController();

    void MouseDragEventHandler(Renderer::Windowing::MouseDragEvent * evt);
    void MouseButtonEventHandler(Renderer::Windowing::MouseButtonEvent * evt);
    void KeyBoardEventHandler(Renderer::Windowing::KeyInputEvent * evt);
};

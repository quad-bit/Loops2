#pragma once

#include "ECS/Components/Scriptable.h"
#include "ECS/Components/Camera.h"

//#include "InputEvents.h"

//struct MouseDragEvent;
//struct KeyInputEvent;
//struct MouseButtonEvent;
//enum class KeyState;

class CameraController : public ECS::Components::Scriptable
{
private:
    ECS::Components::Camera * cam;
    float velocity = .4f;
    //ComponentHandle<Camera> camHandle;

    bool dragStartred = false;
    glm::vec2 currentMousePos, prevMousePos;

    glm::vec2 mouseDelta;
    bool mousePressed = false;
    //KeyState * state;
    float absDeltaX;
    float absDeltaY;
    //MouseButtons button;
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

    //void MouseDragEventHandler(MouseDragEvent * evt);
    //void MouseButtonEventHandler(MouseButtonEvent * evt);
    //void KeyBoardEventHandler(KeyInputEvent * evt);
};

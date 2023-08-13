#pragma once

#include "InputEvents.h"


class MouseInputManager
{

private:
    MouseInputManager(){}
    MouseInputManager(MouseInputManager const &) {}
    MouseInputManager const & operator= (MouseInputManager const &) {}

    static MouseInputManager* instance;

    std::vector<MouseButtonEvent*> mouseButtonEventPool;
    uint16_t buttonEventPoolSize = 10;
    uint16_t buttonEventCounter = 0;

    std::vector<MousePointerEvent*> mousePointerEventPool;
    uint16_t pointerEventPoolSize = 10;
    uint16_t pointerEventCounter = 0;

    std::vector<MouseDragEvent*> mouseDragEventPool;
    uint16_t dragEventPoolSize = 10;
    uint16_t dragEventCounter = 0;

    KeyState currentMouseButtonState;
    MouseButtons currentMouseButtonDown;
    double mouseX, mouseY;

    void CreateMouseDragEvent(KeyState state);
    MouseButtonEvent * FetchMouseButtonEvent();
    MousePointerEvent * FetchMousePointerEvent();
    MouseDragEvent * FetchMouseDragEvent();

public:
    void Init();
    void DeInit();
    void Update();
    static MouseInputManager* GetInstance();
    ~MouseInputManager();

    
    void MousePointerEventHandler(double x, double y);
    void MouseButtonEventHandler(const char* buttonName, const char* buttonEvent);
};

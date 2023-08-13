#pragma once

#include "ECS/Events/Event.h"
//#include "Camera.h"

class Camera;
class MeshRenderer;
class Light;
class Entity;

class CameraAdditionEvent : public Event
{
public:
    Camera * cam;
};

class MeshRendererAdditionEvent : public Event
{
public:
    MeshRenderer * renderer;
};

class LightAdditionEvent : public Event
{
public:
    Light * light;
};

class DepthPassAttachmentCreationEvent : public Event
{
public:
    std::vector<uint32_t> imageId;
    std::vector<uint32_t> imageViewId;
    std::vector<uint32_t> memoryId;
};

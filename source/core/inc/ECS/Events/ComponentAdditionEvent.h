#pragma once

#include "ECS/Events/Event.h"
//#include "Camera.h"

namespace Core
{
    namespace ECS
    {
        class Entity;
        namespace Components
        {
            class Camera;
            class MeshRenderer;
            class Light;
        }
    }
}

namespace Core
{
    namespace ECS
    {
        namespace Events
        {
            class CameraAdditionEvent : public Event
            {
            public:
                Core::ECS::Components::Camera* cam;
            };

            class MeshRendererAdditionEvent : public Event
            {
            public:
                Core::ECS::Components::MeshRenderer* renderer;
            };

            class LightAdditionEvent : public Event
            {
            public:
                Core::ECS::Components::Light* light;
            };

            class DepthPassAttachmentCreationEvent : public Event
            {
            public:
                std::vector<uint32_t> imageId;
                std::vector<uint32_t> imageViewId;
                std::vector<uint32_t> memoryId;
            };
        }
    }
}
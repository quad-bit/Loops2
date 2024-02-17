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
            class Material;
            class Transform;
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
                Core::ECS::Components::Transform* transform;
            };

            class MaterialCreationEvent : public Event
            {
            public:
                Core::ECS::Components::Material* material;
            };

            class MaterialAdditionEvent : public Event
            {
            public:
                Core::ECS::Components::Material* material;
                Core::ECS::Entity* entity;
            };
        }
    }
}
#pragma once

#include "ECS/Events/Event.h"
//#include "Camera.h"
#include "ECS/ECS_Helper.h"

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
            class Bound;
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

            class BoundAdditionEvent : public Event
            {
            public:
                Core::ECS::Components::Bound* bounds;
                Core::ECS::COMPONENT_TYPE boundParentType;
                //uint32_t boundParentComponentId;
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
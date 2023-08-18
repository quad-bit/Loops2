#pragma once

#include "ECS/Events/Event.h"
#include "ECS/SceneNode.h"

namespace Core
{
    namespace ECS
    {
        namespace Events
        {

            struct NodeAdditionEvent : public Event
            {
                Core::ECS::SceneNode* node;
            };

            struct NodeDeletionEvent : public Event
            {
                Core::ECS::SceneNode* node;

            };

            // parent changed
            struct TreeModificationEvent : public Event
            {
                enum class ModType
                {
                    SET_PARENT,
                    NODE_DELETION,
                    NONE
                };

                Core::ECS::SceneNode* node;
                Core::ECS::SceneNode* newParent;
                ModType mod;
            };
        }
    }
}
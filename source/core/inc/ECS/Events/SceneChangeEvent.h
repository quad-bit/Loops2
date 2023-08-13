#pragma once

#include "ECS/Events/Event.h"
#include "ECS/SceneNode.h"

struct NodeAdditionEvent : public Event
{
    SceneNode * node;
};

struct NodeDeletionEvent : public Event
{
    SceneNode * node;

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

    SceneNode * node;
    SceneNode * newParent;
    ModType mod;
};
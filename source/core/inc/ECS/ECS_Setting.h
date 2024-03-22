#pragma once

#define MAX_NUM_OF_UNIQUE_COMPONENT 600
#define MAX_POINT_LIGHTS 8 * 8 *8

namespace Core
{
    namespace ECS
    {
        class World;
        enum class APP_STATE
        {
            STARTED,
            RUNNING,
            PAUSED,
            STOPPED,
            NONE
        };

    }
}

extern Core::ECS::APP_STATE appState;
extern Core::ECS::World* worldObj;
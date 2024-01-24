#pragma once

#define MAX_NUM_OF_UNIQUE_COMPONENT 200

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
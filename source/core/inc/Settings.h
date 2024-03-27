#pragma once

#include <stdint.h>
#include <string>
#include "LightCullSettings.h"

//#define NUM_CLUSTERS_Z 1//24
//#define NUM_CLUSTERS_X 4//1280 / 80
//#define NUM_CLUSTERS_Y 1//720 / 80
//#define NUM_CLUSTERS NUM_CLUSTERS_X * NUM_CLUSTERS_Y * NUM_CLUSTERS_Z
//#define MAX_LIGHTS_PER_CLUSTER 7

#define MAX_POINT_LIGHTS MAX_LIGHTS


namespace Core
{
    /// <summary>
    /// Used to expose the internal settings to application
    /// </summary>
    struct Settings
    {
        static uint32_t m_swapBufferCount/*, m_currentSwapBufferIndex*/;
        static uint32_t m_maxFramesInFlight/*, m_currentFrameInFlight*/;
        static uint32_t m_maxFrameRate;
    };

    struct WindowSettings
    {
        uint32_t m_windowWidth;
        uint32_t m_windowHeight;
        uint32_t m_renderWidth;
        uint32_t m_renderHeight;
        std::string m_windowName;
    };
}

#pragma once

#include <stdint.h>
#include <string>

namespace Core
{
    /// <summary>
    /// Used to expose the internal settings to application
    /// </summary>
    struct Settings
    {
        static uint32_t m_swapBufferCount, m_currentSwapBufferIndex;
        static uint32_t m_maxFramesInFlight, m_currentFrameInFlight;
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

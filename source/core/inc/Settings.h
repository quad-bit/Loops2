#pragma once

#include <stdint.h>
#include <string>

namespace Core
{
    class Settings
    {
    private:
        //uint32_t m_windowWidth, m_windowHeight;
        //uint32_t m_renderWidth, m_renderHeight;
        //std::string m_windowName;
        uint32_t m_swapBufferCount;
        float m_depthClearValue, m_stencilClearValue;
        float m_clearColorValue[4];
        uint32_t m_maxFramesInFlight, m_currentFrameInFlight;
        uint32_t m_maxFrameRate;

    public:
        Settings() = delete;
        ~Settings() {}
        Settings(Settings const&) = delete;
        Settings const& operator= (Settings const&) = delete;

        Settings(const std::string& windowName = std::string{"Loops"},
            uint32_t windowWidth = 600,
            uint32_t windowHeight = 600,
            uint32_t renderWidth = 600,
            uint32_t renderHeight = 600
            );

        void SetSwapBufferCount(uint32_t swapBufferCount);
        void SetDepthClearValue(float depthClearValue);
        void SetStencilClearValue(float stencilClearValue);
        void SetClearColorValue(float clearColorValue[4]);
        //std::tuple<uint32_t, uint32_t> GetWindowResolution() const;
        //std::tuple<uint32_t, uint32_t> GetRenderResolution() const;
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

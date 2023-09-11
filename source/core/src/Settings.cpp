#include "Settings.h"

Core::Settings::Settings(
    const std::string& windowName,
    uint32_t windowWidth,
    uint32_t windowHeight,
    uint32_t renderWidth,
    uint32_t renderHeight):
    m_depthClearValue(1.0f),
    m_stencilClearValue(0.0),
    m_clearColorValue{{34.0f/256.0f}, {34.0f/256.0f}, {34.0f/256.0f}, {1.0f }},
    m_maxFrameRate(60)
{
}

void Core::Settings::SetSwapBufferCount(uint32_t swapBufferCount)
{
    m_swapBufferCount = swapBufferCount;
}

void Core::Settings::SetDepthClearValue(float depthClearValue)
{
    m_depthClearValue = depthClearValue;
}

void Core::Settings::SetStencilClearValue(float stencilClearValue)
{
    m_stencilClearValue = stencilClearValue;
}

void Core::Settings::SetClearColorValue(float clearColorValue[4])
{
    m_clearColorValue[0] = clearColorValue[0];
    m_clearColorValue[1] = clearColorValue[1];
    m_clearColorValue[2] = clearColorValue[2];
    m_clearColorValue[3] = clearColorValue[3];
}

//std::tuple<uint32_t, uint32_t> Core::Settings::GetWindowResolution() const
//{
//    return { m_windowWidth, m_windowHeight };
//}
//
//std::tuple<uint32_t, uint32_t> Core::Settings::GetRenderResolution() const
//{
//    return { m_renderWidth, m_renderHeight };
//}
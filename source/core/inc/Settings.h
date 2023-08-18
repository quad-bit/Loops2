#pragma once

#include <stdint.h>
#include <string>

class Settings
{
public:
    static uint32_t windowWidth, windowHeight;
    static std::string windowName;
    static uint32_t swapBufferCount, currentSwapChainIndex;
    static float depthClearValue, stencilClearValue;
    static float clearColorValue[4];
    static uint32_t maxFramesInFlight, currentFrameInFlight;
    static uint32_t maxFrameRate;
};

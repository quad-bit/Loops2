#include "Settings.h"

uint32_t Settings::windowWidth, Settings::windowHeight;
std::string Settings::windowName;
uint32_t Settings::swapBufferCount;
uint32_t Settings::currentSwapChainIndex;
float Settings::depthClearValue, Settings::stencilClearValue;
float Settings::clearColorValue[4];
uint32_t Settings::maxFramesInFlight;
uint32_t Settings::currentFrameInFlight;
uint32_t Settings::maxFrameRate;


#include "windowing/WindowManager.h"
#include <CorePrecompiled.h>

Renderer::Windowing::WindowManager::WindowManager(uint32_t winWidth, uint32_t winHeight, uint32_t renderWidth, uint32_t renderHeight, std::string winName):
    m_windowWidth(winWidth), m_windowHeight(winHeight), m_renderHeight(renderHeight), m_renderWidth(renderHeight), m_windowName(winName)
{
}

void Renderer::Windowing::WindowManager::Init()
{
    PLOGD << "Window manager Init";
    InitOSWindow();
}

void Renderer::Windowing::WindowManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";
    DeInitOSWindow();
}

void Renderer::Windowing::WindowManager::Close()
{
    windowShouldRun = false;
}

bool Renderer::Windowing::WindowManager::Update()
{
    UpdateOSWindow();
    return windowShouldRun;
}

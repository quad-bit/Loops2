#include "windowing/WindowManager.h"
#include <CorePrecompiled.h>

Renderer::Windowing::WindowManager::WindowManager(const Core::WindowSettings& windowSettings):
    m_windowSettings(windowSettings)
{
}

void Renderer::Windowing::WindowManager::Init()
{
    PLOGD << "Window manager Init";
    InitOSWindow();
}

void Renderer::Windowing::WindowManager::DeInit()
{
    PLOGD << "Window manager DeInit";
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

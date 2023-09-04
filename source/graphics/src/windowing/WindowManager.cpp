#include "windowing/WindowManager.h"
#include <CorePrecompiled.h>

Renderer::Windowing::WindowManager* Renderer::Windowing::WindowManager::windowManagerInstance = nullptr;

Renderer::Windowing::WindowManager::~WindowManager()
{
}

Renderer::Windowing::WindowManager * Renderer::Windowing::WindowManager::GetInstance()
{
    if (windowManagerInstance == nullptr)
    {
        windowManagerInstance = new Renderer::Windowing::WindowManager();
    }
    return windowManagerInstance;
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

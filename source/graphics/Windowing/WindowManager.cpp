#include "WindowManager.h"
#include <CorePrecompiled.h>

WindowManager* WindowManager::windowManagerInstance = nullptr;

WindowManager::~WindowManager()
{
}

WindowManager * WindowManager::GetInstance()
{
    if (windowManagerInstance == nullptr)
    {
        windowManagerInstance = new WindowManager();
    }
    return windowManagerInstance;
}

void WindowManager::Init()
{
    PLOGD << "Window manager Init";

    InitOSWindow();
}

void WindowManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    DeInitOSWindow();
}

void WindowManager::Close()
{
    windowShouldRun = false;
}

bool WindowManager::Update()
{
    UpdateOSWindow();
    return windowShouldRun;
}

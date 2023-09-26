#include "EngineManager.h"
#include "ApplicationController.h"
#include <stdio.h>

void main()
{
    ApplicationController appController;

    Engine::EngineManager::GetInstance()->Init("Loops", 1024, 1024, 1024, 1024);
    appController.Init();

    appController.Update();
    Engine::EngineManager::GetInstance()->Update();

    appController.DeInit();
    Engine::EngineManager::GetInstance()->DeInit();
    delete Engine::EngineManager::GetInstance();

    getchar();
}
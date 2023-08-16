#include "EngineManager.h"
#include "ApplicationController.h"
#include <stdio.h>

void main()
{
    ApplicationController appController;

    EngineManager::GetInstance()->Init();
    appController.Init();

    appController.Update();
    EngineManager::GetInstance()->Update();

    appController.DeInit();
    EngineManager::GetInstance()->DeInit();
    delete EngineManager::GetInstance();

    //getchar();
}
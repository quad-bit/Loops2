#include "EngineManager.h"
#include "ApplicationController.h"
#include <stdio.h>

void main()
{
    ApplicationController appController;

    Engine::Utility::SceneLoadInfo sceneLoadInfo
    {
        ASSETS_PATH + std::string{ "\\models\\ABeautifulGame\\glTF\\ABeautifulGame.gltf" },
        100.0f, false 
    };
    Engine::EngineManager::GetInstance()->Init(sceneLoadInfo, "Loops", 1280, 720, 1280, 720);
    appController.Init();

    appController.Update();
    Engine::EngineManager::GetInstance()->Update();

    appController.DeInit();
    Engine::EngineManager::GetInstance()->DeInit();
    delete Engine::EngineManager::GetInstance();

    getchar();
}
#include "CorePrecompiled.h"
#include "EngineManager.h"
#include <CoreManager.h>
#include <Settings.h>
//#include <GraphicsManager.h>
#include <ECS/ECS_Manager.h>
//#include <InputManager.h>
#include <Utility/Timer.h>
#include "SceneManager.h"

Engine::EngineManager* Engine::EngineManager::instance = nullptr;

void Engine::EngineManager::Init()
{
    //PLOGD << "ENGINE MANAGER Init";
    Settings::maxFrameRate = 60;
    Settings::windowWidth = 1024;
    Settings::windowHeight = 1024;
    Settings::windowName = "Loops";

    Core::CoreManager::GetInstance()->Init();
    //GraphicsManager::GetInstance()->Init(1024, 1024, "Loops");
    //GraphicsManager::GetInstance()->SetupRenderer();
    ECS_Manager::GetInstance()->Init();
    sceneManagerObj = new Engine::SceneManager();
    Core::Utility::Timer::GetInstance()->Init();
}

void Engine::EngineManager::DeInit()
{
    Core::Utility::Timer::GetInstance()->DeInit();
    delete Core::Utility::Timer::GetInstance();

    delete sceneManagerObj;

    ECS_Manager::GetInstance()->DeInit();
    delete ECS_Manager::GetInstance();

    //GraphicsManager::GetInstance()->DislogeRenderer();
    //GraphicsManager::GetInstance()->DeInit();
    //delete GraphicsManager::GetInstance();

    Core::CoreManager::GetInstance()->DeInit();
    delete Core::CoreManager::GetInstance();
}

void Engine::EngineManager::Update()
{
#if 0
    // Before the update, will run just once
    {
        GraphicsManager::GetInstance()->PreUpdate();
    }

    typedef double precision;

    precision msPerUpdate = 1000.0f / (precision)Settings::maxFrameRate;
    precision lag = 0.0f;

    Timer::GetInstance()->Reset();
    while (GraphicsManager::GetInstance()->IsWindowActive())
    {
        precision delta = Timer::GetInstance()->GetDeltaTime<precision>();
        lag += delta;
        //if (delta > msPerUpdate)
            //delta = msPerUpdate;
        //PLOGD << "delta : " << delta << "  lag : " << lag;
        InputManager::GetInstance()->Update();
        CoreManager::GetInstance()->Update();

        uint16_t iterations = 0, maxIterations = 5;
        while (lag >= msPerUpdate && iterations < maxIterations)
        {
            ECS_Manager::GetInstance()->Update((float)msPerUpdate);
            lag -= msPerUpdate;
            iterations++;
            //PLOGD << "Test";
        }

        // to make it more smooth pass this(lag / MS_PER_UPDATE) to renderer, advance the render
        ECS_Manager::GetInstance()->Update((float)(lag / msPerUpdate));
        // lag = msPerUpdate;
        GraphicsManager::GetInstance()->Update();
    }

    // After the update, will run just once
    {
        GraphicsManager::GetInstance()->PostUpdate();
    }
#endif
}



/*
void EngineManager::Update()
{
    // Before the update, will run just once
    {
        GraphicsManager::GetInstance()->PreUpdate();
    }

    typedef double precision;

    precision msPerUpdate = 1000.0f / (precision)Settings::maxFrameRate;
    precision lag = msPerUpdate;// 0.0f;

    Timer::GetInstance()->Reset();
    while (GraphicsManager::GetInstance()->IsWindowActive())
    {
        precision delta = Timer::GetInstance()->GetDeltaTime<precision>();
        lag += delta;
        //PLOGD << delta << "    " << lag;

        InputManager::GetInstance()->Update();
        CoreManager::GetInstance()->Update();

        uint16_t iterations = 0, maxIterations = 5;
        while (lag >= msPerUpdate && iterations < maxIterations)
        {
            ECS_Manager::GetInstance()->Update(1.0f);
            lag -= msPerUpdate;
            iterations++;
        }
        // to make it more smooth pass this(lag / MS_PER_UPDATE) to renderer, advance the render
        ECS_Manager::GetInstance()->Render(lag / msPerUpdate);
        lag = msPerUpdate;// 0.0f;

        GraphicsManager::GetInstance()->Update();
    }

    // After the update, will run just once
    {
        GraphicsManager::GetInstance()->PostUpdate();
    }
}
*/
Engine::EngineManager * Engine::EngineManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Engine::EngineManager();
    }
    return instance;
}

Engine::EngineManager::~EngineManager()
{
    
}

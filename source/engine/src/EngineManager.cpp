#include "CorePrecompiled.h"
#include <GraphicsManager.h>
#include "EngineManager.h"
#include <CoreManager.h>
#include <Settings.h>
#include <ECS/ECS_Manager.h>
#include <Windowing/InputManager.h>
#include <Windowing/MouseInputManager.h>
#include <Utility/Timer.h>
#include "ECS/SceneManager.h"

#include <renderGraph/pipelines/LowEndPipeline.h>
#include <renderGraph/Graph.h>

Engine::EngineManager* Engine::EngineManager::instance = nullptr;

void Engine::EngineManager::Init(const std::string& windowName,
    uint32_t windowWidth,
    uint32_t windowHeight,
    uint32_t renderWidth,
    uint32_t renderHeight)

{
    //PLOGD << "ENGINE MANAGER Init";
    m_windowSettings.m_windowWidth = windowWidth;
    m_windowSettings.m_windowHeight = windowHeight;
    m_windowSettings.m_renderWidth = renderWidth;
    m_windowSettings.m_renderHeight = renderHeight;
    m_windowSettings.m_windowName = windowName;

    Core::CoreManager::GetInstance()->Init();
    
    m_graphicsMngrObj = new Renderer::GraphicsManager(m_windowSettings, m_renderData);
    m_graphicsMngrObj->Init();
    m_graphicsMngrObj->SetupRenderer();

    Renderer::Windowing::InputManager::GetInstance()->Init(m_graphicsMngrObj->GetGlfwWindow());
    Renderer::Windowing::MouseInputManager::GetInstance()->Init();

    ECS_Manager::GetInstance()->Init(m_renderData, m_gltfLoader);
    sceneManagerObj = new Engine::SceneManager();
    Core::Utility::Timer::GetInstance()->Init();

}

void Engine::EngineManager::DeInit()
{
    /*m_renderGraphManager->DeInit();
    m_renderGraphManager.reset();*/

    Core::Utility::Timer::GetInstance()->DeInit();
    delete Core::Utility::Timer::GetInstance();

    delete sceneManagerObj;

    ECS_Manager::GetInstance()->DeInit();
    delete ECS_Manager::GetInstance();

    Renderer::Windowing::MouseInputManager::GetInstance()->DeInit();
    delete Renderer::Windowing::MouseInputManager::GetInstance();

    Renderer::Windowing::InputManager::GetInstance()->DeInit();
    delete Renderer::Windowing::InputManager::GetInstance();

    m_graphicsMngrObj->DeInit();
    delete m_graphicsMngrObj;

    Core::CoreManager::GetInstance()->DeInit();
    delete Core::CoreManager::GetInstance();
}

void Engine::EngineManager::Update()
{
    
    typedef double precision;

    precision msPerUpdate = 1000.0f / (precision)60.0f;// Settings::maxFrameRate;
    precision lag = 0.0f;

    Core::Utility::Timer::GetInstance()->Reset();
    while (m_graphicsMngrObj->IsWindowActive())
    {
        // Before the update
        {
            m_graphicsMngrObj->PreRender(m_frameInfo);
        }

        precision delta = Core::Utility::Timer::GetInstance()->GetDeltaTime<precision>();
        lag += delta;
        //if (delta > msPerUpdate)
            //delta = msPerUpdate;
        //PLOGD << "delta : " << delta << "  lag : " << lag;
        Renderer::Windowing::InputManager::GetInstance()->Update();
        Core::CoreManager::GetInstance()->Update();

        uint16_t iterations = 0, maxIterations = 5;
        while (lag >= msPerUpdate && iterations < maxIterations)
        {
            ECS_Manager::GetInstance()->Update((float)msPerUpdate, m_frameInfo);
            lag -= msPerUpdate;
            iterations++;
            //PLOGD << "Test";
        }

        // to make it more smooth pass this(lag / MS_PER_UPDATE) to renderer, advance the render
        ECS_Manager::GetInstance()->Update((float)(lag / msPerUpdate), m_frameInfo);
        // lag = msPerUpdate;

        //Renderer::Windowing::MouseInputManager::GetInstance()->Update();

        m_graphicsMngrObj->Update(m_frameInfo);

        // After the update
        {
            m_graphicsMngrObj->PostRender();
        }
    }

    m_graphicsMngrObj->RenderExit();
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

const Core::Wrappers::FrameInfo& Engine::EngineManager::GetFrameInfo()
{
    return m_frameInfo;
}

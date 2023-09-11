#include "RenderingManager.h"
#include "GraphicsManager.h"
#include "windowing/WindowManager.h"
#include "VulkanInterface.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "RendererSettings.h"
#include "windowing/InputManager.h"
#include "windowing/MouseInputManager.h"
#include <CorePrecompiled.h>
#include <Settings.h>
#include <ECS/Events/EventBus.h>


void Renderer::GraphicsManager::Init()
{
    PLOGD << "Graphics manager Init";

    m_windowMngrObj->Init();
    Core::ECS::Events::EventBus::GetInstance()->Subscribe(this, &GraphicsManager::KeyBoardEventHandler);

    m_renderingMngrObj->Init();

    /*
    Renderer::Windowing::InputManager::GetInstance()->Init();
    Renderer::Windowing::MouseInputManager::GetInstance()->Init();
    */
}

Renderer::GraphicsManager::GraphicsManager(const Core::WindowSettings& windowSettings):
    m_windowSettings{windowSettings}
{
    //m_settings = std::make_unique<Core::Settings>(winName, winWidth, winHeight, renderWidth, renderHeight);
    m_windowMngrObj = std::make_unique<Windowing::WindowManager>(m_windowSettings);
    m_renderingMngrObj = std::make_unique<Renderer::RenderingManager>(m_windowSettings);
}

void Renderer::GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    m_renderingMngrObj->DeInit();
    m_renderingMngrObj.reset();

    m_windowMngrObj->DeInit();
    m_windowMngrObj.reset();
}

void Renderer::GraphicsManager::Update()
{
    //Renderer::Windowing::MouseInputManager::GetInstance()->Update();
    m_renderingMngrObj->Render();
}

void Renderer::GraphicsManager::KeyBoardEventHandler(Renderer::Windowing::KeyInputEvent* evt)
{
    if (strcmp(evt->keyname, "ESCAPE") == 0 && evt->keyState == Renderer::Windowing::KeyState::PRESSED)
    {
        m_windowMngrObj->Close();
    }
}

Renderer::GraphicsManager::~GraphicsManager()
{
}

void Renderer::GraphicsManager::SetupRenderer()
{
    m_renderingMngrObj->SetupRenderer();
}

void Renderer::GraphicsManager::DislogeRenderer()
{
    m_renderingMngrObj->DislogeRenderer();
}

void Renderer::GraphicsManager::PreUpdate()
{
    m_renderingMngrObj->PreRender();
}

void Renderer::GraphicsManager::PostUpdate()
{
    m_renderingMngrObj->PostRenderLoopEnd();
}

bool Renderer::GraphicsManager::IsWindowActive()
{
    return m_windowMngrObj->Update();
}

GLFWwindow* Renderer::GraphicsManager::GetGlfwWindow()
{
    return m_windowMngrObj->glfwWindow;
}

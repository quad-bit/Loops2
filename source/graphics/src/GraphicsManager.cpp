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

    //m_renderingMngrObj->Init();

    /*
    Renderer::Windowing::InputManager::GetInstance()->Init();
    Renderer::Windowing::MouseInputManager::GetInstance()->Init();

#if (RENDERING_API == VULKAN)
    Core::RendererSettings::queueRequirementCount = 4;
    Core::RendererSettings::queueReq = new Core::Wrappers::QueueWrapper[Core::RendererSettings::queueRequirementCount];
    
    Core::RendererSettings::queueReq[0].purpose = new Core::Enums::QueueType{ Core::Enums::QueueType::RENDER };
    Core::RendererSettings::queueReq[0].queueType = new Core::Enums::PipelineType{ Core::Enums::PipelineType::GRAPHICS };

    Core::RendererSettings::queueReq[1].purpose = new Core::Enums::QueueType{ Core::Enums::QueueType::PRESENT };
    Core::RendererSettings::queueReq[1].queueType = new Core::Enums::PipelineType{ Core::Enums::PipelineType::GRAPHICS };

    Core::RendererSettings::queueReq[2].purpose = new Core::Enums::QueueType{ Core::Enums::QueueType::COMPUTE };
    Core::RendererSettings::queueReq[2].queueType = new Core::Enums::PipelineType{ Core::Enums::PipelineType::COMPUTE };

    Core::RendererSettings::queueReq[3].purpose = new Core::Enums::QueueType{ Core::Enums::QueueType::TRANSFER };
    Core::RendererSettings::queueReq[3].queueType = new Core::Enums::PipelineType{ Core::Enums::PipelineType::TRANSFER };
    
    apiInterface = new Renderer::VulkanInterface();
    m_renderingMngrObj = std::make_unique<Renderer::RenderingManager>();

#elif (RENDERING_API == DX)
    apiInterface = new DxInterface();
#endif
    */
    //m_renderingMngrObj->Init(apiInterface);
}

Renderer::GraphicsManager::GraphicsManager()
{
}

Renderer::GraphicsManager::GraphicsManager(uint32_t winWidth, uint32_t winHeight, uint32_t renderWidth, uint32_t renderHeight, std::string winName)
{
    m_settings = std::make_unique<Core::Settings>(winName, winWidth, winHeight, renderWidth, renderHeight);
    m_windowMngrObj = std::make_unique<Windowing::WindowManager>(winWidth, winHeight, renderWidth, renderHeight, winName);
    m_renderingMngrObj = std::make_unique<Renderer::RenderingManager>();
}

void Renderer::GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    m_renderingMngrObj->DeInit();
    m_renderingMngrObj.reset();
    /*
    delete apiInterface;

    for (uint32_t i = 0; i < 4; i++)
    {
        delete Core::RendererSettings::queueReq[i].purpose;
        delete Core::RendererSettings::queueReq[i].queueType;
    }

    delete[] Core::RendererSettings::queueReq;
    */

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

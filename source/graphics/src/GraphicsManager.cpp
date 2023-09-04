#include "GraphicsManager.h"
#include "windowing/WindowManager.h"
#include <Settings.h>
#include "VulkanInterface.h"
#include "RenderingManager.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "RendererSettings.h"
#include "windowing/InputManager.h"
#include "windowing/MouseInputManager.h"
#include <CorePrecompiled.h>

Renderer::GraphicsManager* Renderer::GraphicsManager::instance = nullptr;

void Renderer::GraphicsManager::Init(uint32_t winWidth, uint32_t winHeight, std::string winName)
{
    PLOGD << "Graphics manager Init";

    Settings::windowWidth   = winWidth;
    Settings::windowHeight  = winHeight;
    Settings::windowName    = winName;
    Renderer::Windowing::WindowManager::GetInstance()->Init();
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
    renderingMngrObj = new Renderer::RenderingManager();

#elif (RENDERING_API == DX)
    apiInterface = new DxInterface();
#endif
    
    renderingMngrObj->Init(apiInterface);
}

void Renderer::GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    renderingMngrObj->DeInit();
    delete renderingMngrObj;

    delete apiInterface;

    for (uint32_t i = 0; i < 4; i++)
    {
        delete Core::RendererSettings::queueReq[i].purpose;
        delete Core::RendererSettings::queueReq[i].queueType;
    }

    delete[] Core::RendererSettings::queueReq;

    Renderer::Windowing::MouseInputManager::GetInstance()->DeInit();
    delete Renderer::Windowing::MouseInputManager::GetInstance();

    Renderer::Windowing::InputManager::GetInstance()->DeInit();
    delete Renderer::Windowing::InputManager::GetInstance();

    Renderer::Windowing::WindowManager::GetInstance()->DeInit();
    delete Renderer::Windowing::WindowManager::GetInstance();
}

void Renderer::GraphicsManager::Update()
{
    Renderer::Windowing::MouseInputManager::GetInstance()->Update();
    renderingMngrObj->Render();
}

Renderer::GraphicsManager * Renderer::GraphicsManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::GraphicsManager();
    }
    return instance;
}

Renderer::GraphicsManager::~GraphicsManager()
{
}

void Renderer::GraphicsManager::SetupRenderer()
{
    renderingMngrObj->SetupRenderer();
}

void Renderer::GraphicsManager::DislogeRenderer()
{
    renderingMngrObj->DislogeRenderer();
}

void Renderer::GraphicsManager::PreUpdate()
{
    renderingMngrObj->PreRender();
}

void Renderer::GraphicsManager::PostUpdate()
{
    renderingMngrObj->PostRenderLoopEnd();
}

bool Renderer::GraphicsManager::IsWindowActive()
{
    return Renderer::Windowing::WindowManager::GetInstance()->Update();
}

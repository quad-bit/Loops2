#include "GraphicsManager.h"
#include "WindowManager.h"
#include <Settings.h>
#include "VulkanInterface.h"
#include "RenderingInterface.h"
#include "RenderingWrapper.h"
#include "RendererSettings.h"
#include "InputManager.h"
#include "MouseInputManager.h"
#include <CorePrecompiled.h>

GraphicsManager* GraphicsManager::instance = nullptr;

void GraphicsManager::Init(uint32_t winWidth, uint32_t winHeight, std::string winName)
{
    PLOGD << "Graphics manager Init";

    Settings::windowWidth   = winWidth;
    Settings::windowHeight  = winHeight;
    Settings::windowName    = winName;
    WindowManager::GetInstance()->Init();
    InputManager::GetInstance()->Init();
    MouseInputManager::GetInstance()->Init();
#if (RENDERING_API == VULKAN)
    
    RendererSettings::queueRequirementCount = 4;
    RendererSettings::queueReq = new QueueWrapper[RendererSettings::queueRequirementCount];
    
    RendererSettings::queueReq[0].purpose = new QueuePurpose{ QueuePurpose::RENDER };
    RendererSettings::queueReq[0].queueType = new PipelineType{ PipelineType::GRAPHICS };

    RendererSettings::queueReq[1].purpose = new QueuePurpose{ QueuePurpose::PRESENT };
    RendererSettings::queueReq[1].queueType = new PipelineType{ PipelineType::GRAPHICS };

    RendererSettings::queueReq[2].purpose = new QueuePurpose{ QueuePurpose::COMPUTE };
    RendererSettings::queueReq[2].queueType = new PipelineType{ PipelineType::COMPUTE };

    RendererSettings::queueReq[3].purpose = new QueuePurpose{ QueuePurpose::TRANSFER };
    RendererSettings::queueReq[3].queueType = new PipelineType{ PipelineType::TRANSFER };
    
    apiInterface = new VulkanInterface();
    renderingInterfaceObj = new RenderingInterface<VulkanInterface>();

#elif (RENDERING_API == DX)
    apiInterface = new DxInterface();
#endif
    
    renderingInterfaceObj->Init(apiInterface);
}

void GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    renderingInterfaceObj->DeInit();
    delete renderingInterfaceObj;

    delete apiInterface;

    for (uint32_t i = 0; i < 4; i++)
    {
        delete RendererSettings::queueReq[i].purpose;
        delete RendererSettings::queueReq[i].queueType;
    }

    delete[] RendererSettings::queueReq;

    MouseInputManager::GetInstance()->DeInit();
    delete MouseInputManager::GetInstance();

    InputManager::GetInstance()->DeInit();
    delete InputManager::GetInstance();

    WindowManager::GetInstance()->DeInit();
    delete WindowManager::GetInstance();
}

void GraphicsManager::Update()
{
    MouseInputManager::GetInstance()->Update();
    renderingInterfaceObj->Render();
}

GraphicsManager * GraphicsManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GraphicsManager();
    }
    return instance;
}

GraphicsManager::~GraphicsManager()
{
}

void GraphicsManager::SetupRenderer()
{
    renderingInterfaceObj->SetupRenderer();
}

void GraphicsManager::DislogeRenderer()
{
    renderingInterfaceObj->DislogeRenderer();
}

void GraphicsManager::PreUpdate()
{
    renderingInterfaceObj->PreRender();
}

void GraphicsManager::PostUpdate()
{
    renderingInterfaceObj->PostRenderLoopEnd();
}

bool GraphicsManager::IsWindowActive()
{
    return WindowManager::GetInstance()->Update();
}

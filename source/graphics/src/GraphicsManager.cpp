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
#include <utility/VulkanUtility.h>

#include <shading/VkDescriptorPoolFactory.h>
#include <shading/VkShaderResourceManager.h>
#include <shading/VkBufferFactory.h>
#include <utility/VulkanMemoryManager.h>

#include <shaderResource/UniformFactory.h>

void Renderer::GraphicsManager::Init()
{
    PLOGD << "Graphics manager Init";

    Core::ECS::Events::EventBus::GetInstance()->Subscribe(this, &GraphicsManager::KeyBoardEventHandler);

    m_renderingMngrObj->Init(m_windowMngrObj->glfwWindow);

    // first api level
    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->Init();
    GfxVk::Shading::VkShaderResourceManager::GetInstance()->Init();
    GfxVk::Shading::VkBufferFactory::GetInstance()->Init();
    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->Init(DeviceInfo::GetPhysicalDeviceMemProps());

    // next high level wrappers
    Renderer::ShaderResource::UniformFactory::GetInstance()->Init();

    Core::Settings::m_swapBufferCount = RendererSettings::GetSwapBufferCount();
    Core::Settings::m_maxFramesInFlight = RendererSettings::GetMaxFramesInFlightCount();
}

Renderer::GraphicsManager::GraphicsManager(const Core::WindowSettings& windowSettings):
    m_windowSettings{windowSettings}
{
    m_windowMngrObj = std::make_unique<Windowing::WindowManager>(m_windowSettings);
    m_windowMngrObj->Init();

    m_renderingMngrObj = std::make_unique<Renderer::RenderingManager>(m_windowSettings);
}

void Renderer::GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    Renderer::ShaderResource::UniformFactory::GetInstance()->DeInit();
    delete Renderer::ShaderResource::UniformFactory::GetInstance();

    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->DeInit();
    delete GfxVk::Utility::VulkanMemoryManager::GetSingleton();

    GfxVk::Shading::VkBufferFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkBufferFactory::GetInstance();

    GfxVk::Shading::VkShaderResourceManager::GetInstance()->DeInit();
    delete GfxVk::Shading::VkShaderResourceManager::GetInstance();

    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkDescriptorPoolFactory::GetInstance();

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

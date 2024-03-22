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

#include <renderGraph/RenderGraphManager.h>
#include <renderGraph/pipelines/LowEndPipeline.h>
#include <renderGraph/pipelines/DepthViewPipeline.h>

#include <shading/VkDescriptorPoolFactory.h>
#include <shading/VkShaderResourceManager.h>
#include <shading/VkShaderFactory.h>
#include <utility/VkBufferFactory.h>
#include <utility/VulkanMemoryManager.h>
#include <VkCommandBufferFactory.h>
#include <synchronisation/VkSynchroniserFactory.h>
#include <utility/VkImageFactory.h>
#include <VkRenderPassFactory.h>
#include <pipeline/VulkanGraphicsPipelineFactory.h>
#include <VkSamplerFactory.h>

#include <resourceManagement/ShaderResourceManager.h>
#include <resourceManagement/MeshFactory.h>
#include <resourceManagement/MaterialFactory.h>

void Renderer::GraphicsManager::Init()
{
    PLOGD << "Graphics manager Init";

    Core::ECS::Events::EventBus::GetInstance()->Subscribe(this, &GraphicsManager::KeyBoardEventHandler);

    m_renderingMngrObj->Init(m_windowMngrObj->glfwWindow);

    Core::Settings::m_swapBufferCount = RendererSettings::GetSwapBufferCount();
    Core::Settings::m_maxFramesInFlight = RendererSettings::GetMaxFramesInFlightCount();

    // first api level
    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->Init();
    GfxVk::Shading::VkShaderFactory::GetInstance()->Init();
    GfxVk::Shading::VkShaderResourceManager::GetInstance()->Init();
    GfxVk::Utility::VkBufferFactory::GetInstance()->Init();
    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->Init(DeviceInfo::GetPhysicalDeviceMemProps());
    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->Init(
        Renderer::RendererSettings::GetRenderQueueId(),
        Renderer::RendererSettings::GetComputeQueueId(),
        Renderer::RendererSettings::GetTransferQueueId());
    GfxVk::Sync::VkSynchroniserFactory::GetInstance()->Init();
    GfxVk::Utility::VkImageFactory::GetInstance()->Init(Core::Settings::m_swapBufferCount, Renderer::RendererSettings::GetTransferQueueId());
    GfxVk::Shading::VkSamplerFactory::GetInstance()->Init();
    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->Init();
    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->Init(m_windowSettings);
    GfxVk::VulkanPipeline::VulkanComputePipelineFactory::GetInstance()->Init(m_windowSettings);

    // next high level wrappers
    Renderer::ResourceManagement::ShaderResourceManager::GetInstance()->Init();
    Renderer::ResourceManagement::MeshFactory::GetInstance()->Init();
    Renderer::ResourceManagement::MaterialFactory::GetInstance()->Init();

    std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline = std::make_unique<Renderer::RenderGraph::Pipelines::LowEndPipeline>(m_renderData, m_windowSettings, "LowEndPipeline");
    //std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline = std::make_unique<Renderer::RenderGraph::Pipelines::DepthViewPipeline>(m_renderData, m_windowSettings, "DepthView");
    m_renderGraphManager->Init(
        Renderer::RendererSettings::GetRenderQueueId(),
        Renderer::RendererSettings::GetComputeQueueId(),
        Renderer::RendererSettings::GetTransferQueueId(),
        Renderer::RendererSettings::GetPresentationQueueId(),
        std::move(pipeline));
}

Renderer::GraphicsManager::GraphicsManager(const Core::WindowSettings& windowSettings, Core::Utility::RenderData& renderData):
    m_windowSettings{windowSettings}, m_renderData(renderData)
{
    m_windowMngrObj = std::make_unique<Windowing::WindowManager>(m_windowSettings);
    m_windowMngrObj->Init();

    m_renderingMngrObj = std::make_unique<Renderer::RenderingManager>(m_windowSettings);

    m_renderGraphManager = std::make_unique<Renderer::RenderGraph::RenderGraphManager>(m_renderData, m_windowSettings);
}

void Renderer::GraphicsManager::DeInit()
{
    PLOGD << "Graphics manager DeInit";

    m_renderGraphManager->DeInit();

    Renderer::ResourceManagement::MaterialFactory::GetInstance()->DeInit();
    delete Renderer::ResourceManagement::MaterialFactory::GetInstance();

    Renderer::ResourceManagement::MeshFactory::GetInstance()->DeInit();
    delete Renderer::ResourceManagement::MeshFactory::GetInstance();

    Renderer::ResourceManagement::ShaderResourceManager::GetInstance()->DeInit();
    delete Renderer::ResourceManagement::ShaderResourceManager::GetInstance();

    GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()->DeInit();
    delete GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance();

    GfxVk::VulkanPipeline::VulkanComputePipelineFactory::GetInstance()->DeInit();
    delete GfxVk::VulkanPipeline::VulkanComputePipelineFactory::GetInstance();

    GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->DeInit();
    delete GfxVk::Renderpass::VkRenderPassFactory::GetInstance();

    GfxVk::Shading::VkSamplerFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkSamplerFactory::GetInstance();

    GfxVk::Utility::VkImageFactory::GetInstance()->DeInit();
    delete GfxVk::Utility::VkImageFactory::GetInstance();

    GfxVk::Sync::VkSynchroniserFactory::GetInstance()->DeInit();
    delete GfxVk::Sync::VkSynchroniserFactory::GetInstance();

    GfxVk::CommandPool::VkCommandBufferFactory::GetInstance()->DeInit();
    delete GfxVk::CommandPool::VkCommandBufferFactory::GetInstance();

    GfxVk::Utility::VulkanMemoryManager::GetSingleton()->DeInit();
    delete GfxVk::Utility::VulkanMemoryManager::GetSingleton();

    GfxVk::Utility::VkBufferFactory::GetInstance()->DeInit();
    delete GfxVk::Utility::VkBufferFactory::GetInstance();

    GfxVk::Shading::VkShaderResourceManager::GetInstance()->DeInit();
    delete GfxVk::Shading::VkShaderResourceManager::GetInstance();

    GfxVk::Shading::VkShaderFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkShaderFactory::GetInstance();

    GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()->DeInit();
    delete GfxVk::Shading::VkDescriptorPoolFactory::GetInstance();

    m_renderGraphManager.reset();

    m_renderingMngrObj->DeInit();
    m_renderingMngrObj.reset();

    m_windowMngrObj->DeInit();
    m_windowMngrObj.reset();
}

void Renderer::GraphicsManager::Update(const Core::Wrappers::FrameInfo& frameInfo)
{
    //Renderer::Windowing::MouseInputManager::GetInstance()->Update();
    //m_renderingMngrObj->Render();
    m_renderGraphManager->Update(frameInfo);
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

void Renderer::GraphicsManager::PreRender(Core::Wrappers::FrameInfo& frameInfo)
{
    //m_renderingMngrObj->PreRender();
    m_renderGraphManager->SetupFrame(frameInfo);
}

void Renderer::GraphicsManager::PostRender()
{
    //m_renderingMngrObj->PostRenderLoopEnd();
    m_renderGraphManager->EndFrame();
}

void Renderer::GraphicsManager::RenderExit()
{
    m_renderGraphManager->OnExit();
}

bool Renderer::GraphicsManager::IsWindowActive()
{
    return m_windowMngrObj->Update();
}

GLFWwindow* Renderer::GraphicsManager::GetGlfwWindow()
{
    return m_windowMngrObj->glfwWindow;
}

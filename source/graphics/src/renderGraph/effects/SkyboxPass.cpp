#include "renderGraph/effects/SkyboxPass.h"
#include "renderGraph/techniques/Skybox.h"

Renderer::RenderGraph::Effects::SkyboxPass::SkyboxPass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& inputNodes):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    auto effectId = VulkanInterfaceAlias::GetEffectId(name);
    Core::Utility::EffectId techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "Skybox");
    Core::Utility::EffectInfo info{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> skyboxTech =
        std::make_unique<Renderer::RenderGraph::Techniques::Skybox>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "Skybox", m_name, info, inputNodes[0], inputNodes[1]);

    //auto taskNode = skyboxTech->GetTaskNode("SkyboxRenderTask");

    //Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, inputNodes[0], taskNode,
    //    Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
    //    Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    //Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, inputNodes[1], taskNode,
    //    Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
    //    Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    m_techniques.push_back(std::move(skyboxTech));
}

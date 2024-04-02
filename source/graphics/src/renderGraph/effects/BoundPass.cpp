#include "renderGraph/effects/BoundPass.h"
#include "renderGraph/techniques/BoundRenderer.h"

Renderer::RenderGraph::Effects::BoundPass::BoundPass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& inputNodes):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    auto effectId = VulkanInterfaceAlias::GetEffectId(name);
    Core::Utility::EffectId techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "BoundRenderer");
    Core::Utility::EffectInfo info{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> boundRenderer =
        std::make_unique<Renderer::RenderGraph::Techniques::BoundRenderer>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "BoundRenderer", m_name, info, inputNodes[0], inputNodes[1], false);

    m_techniques.push_back(std::move(boundRenderer));
}

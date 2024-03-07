#include "renderGraph/effects/LightCullPass.h"
#include "renderGraph/techniques/LightCull.h"

Renderer::RenderGraph::Effects::LightCullPass::LightCullPass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    auto effectId = VulkanInterfaceAlias::GetEffectId(name);
    Core::Utility::EffectId techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "LightCull");
    Core::Utility::EffectInfo info{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> lightCullTech =
        std::make_unique<Renderer::RenderGraph::Techniques::LightCull>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "LightCull", m_name, info);

    m_techniques.push_back(std::move(lightCullTech));
}

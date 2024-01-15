#include "renderGraph/effects/Opaque.h"
#include "renderGraph/techniques/OpaqueUnlit.h"

Renderer::RenderGraph::Effects::OpaquePass::OpaquePass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    std::unique_ptr<Renderer::RenderGraph::Technique> tech =
        std::make_unique<Renderer::RenderGraph::Techniques::OpaqueUnlit>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "OpaqueUnlit", m_name);
    m_techniques.push_back(std::move(tech));
}

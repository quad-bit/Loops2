#include "renderGraph/effects/DepthPass.h"
#include "renderGraph/utility/Utils.h"
#include "renderGraph/techniques/DepthTechnique.h"

Renderer::RenderGraph::Effects::DepthPass::DepthPass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    std::unique_ptr<Renderer::RenderGraph::Technique> tech =
        std::make_unique<Renderer::RenderGraph::Techniques::DepthTechnique>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "DepthTechnique", m_name);
    m_techniques.push_back(std::move(tech));
}

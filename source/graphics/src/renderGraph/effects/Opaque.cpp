#include "renderGraph/effects/Opaque.h"
#include "renderGraph/techniques/OpaqueUnlit.h"
#include "renderGraph/techniques/OpaqueTexturedUnlit.h"

Renderer::RenderGraph::Effects::OpaquePass::OpaquePass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{
    auto effectId = VulkanInterfaceAlias::GetEffectId(name);
    Core::Utility::EffectId techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "OpaqueUnlit");
    Core::Utility::EffectInfo info{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> unlitTech =
        std::make_unique<Renderer::RenderGraph::Techniques::OpaqueUnlit>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "OpaqueUnlit", m_name, info);

    auto unlitOutputs = unlitTech->GetGraphEndResourceNodes();
    auto& depthImages = unlitTech->GetResource(unlitOutputs[0]);
    auto& colorImages = unlitTech->GetResource(unlitOutputs[1]);

    techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "OpaqueTexturedUnlit");
    Core::Utility::EffectInfo info2{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> unlitTexturedTech =
        std::make_unique<Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "OpaqueTexturedUnlit", m_name, colorImages, depthImages, info2);
    
    auto unlitTexturedTaskNode = unlitTexturedTech->GetTaskNode("OpaqueRenderTask");

    // Connect techniques

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, unlitOutputs[0], unlitTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, unlitOutputs[1], unlitTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    m_techniques.push_back(std::move(unlitTech));
    m_techniques.push_back(std::move(unlitTexturedTech));
}

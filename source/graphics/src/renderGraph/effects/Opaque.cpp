#include "renderGraph/effects/Opaque.h"
#include "renderGraph/techniques/OpaqueUnlit.h"
#include "renderGraph/techniques/OpaqueTexturedUnlit.h"
#include "renderGraph/techniques/OpaqueTexturedLit.h"

Renderer::RenderGraph::Effects::OpaquePass::OpaquePass(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& inputNodes):
    Effect(graph, name, funcs), m_renderData(renderData), m_windowSettings(windowSettings)
{

    // === OpaqueUnlit
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

    // === OpaqueTexturedUnlit
    techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "OpaqueTexturedUnlit");
    Core::Utility::EffectInfo info2{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> unlitTexturedTech =
        std::make_unique<Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "OpaqueTexturedUnlit", m_name, colorImages, depthImages, info2);
    
    auto unlitTexturedTaskNode = unlitTexturedTech->GetTaskNode("OpaqueRenderTask");

    // === OpaqueTexturedLit
    techId = VulkanInterfaceAlias::GetTechniqueId(effectId, "OpaqueTexturedLit");
    Core::Utility::EffectInfo info3{ effectId, techId };

    std::unique_ptr<Renderer::RenderGraph::Technique> litTexturedTech =
        std::make_unique<Renderer::RenderGraph::Techniques::OpaqueTexturedLit>(
            m_renderData, m_windowSettings,
            graph, m_callbackUtility, "OpaqueTexturedLit", m_name, colorImages, depthImages, info3);

    auto litTexturedTaskNode = litTexturedTech->GetTaskNode("OpaqueRenderTask");

    // Connect techniques

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, unlitOutputs[0], unlitTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, unlitOutputs[1], unlitTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    auto unlitTexturedOutputs = unlitTexturedTech->GetGraphEndResourceNodes();
    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, unlitTexturedOutputs[0], litTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, unlitTexturedOutputs[1], litTexturedTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    // Connect light cull data to opaqueLit
    {
        Renderer::RenderGraph::Utils::BufferResourceConnectionInfo bufInfo{};
        bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufInfo.expectedShader = Core::Enums::ShaderType::FRAGMENT;
        bufInfo.prevShader = Core::Enums::ShaderType::COMPUTE;
        bufInfo.previousUsage = Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT;

        Renderer::RenderGraph::Utils::ConnectionInfo connection{};
        connection.m_bufInfo = bufInfo;
        connection.m_resource = static_cast<Renderer::RenderGraph::ResourceNode*>(inputNodes[0]->GetNodeData())->GetResource();
        connection.m_resourceParentNodeId = inputNodes[0]->GetNodeId();
        connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
        Renderer::RenderGraph::Utils::AddEdge(m_graph, inputNodes[0], litTexturedTaskNode, connection);
    }

    m_techniques.push_back(std::move(unlitTech));
    m_techniques.push_back(std::move(unlitTexturedTech));
    m_techniques.push_back(std::move(litTexturedTech));
}

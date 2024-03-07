#include "renderGraph/techniques/BoundRenderer.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>
#include <Math/Cube.h>
#include <Utility/PipelineStateWrappers.h>

void Renderer::RenderGraph::Techniques::BoundRenderer::CreateResources()
{
    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    CreateRenderTaskGraphNode(GetNodeName("BoundRenderTask"), m_parentEffectName, m_name, g_renderArea,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_taskNode);

    Core::Utility::RasterizationStateWrapper rasterWrapper{};
    Core::Wrappers::RasterizationState rasterState{};
    rasterState.cullMode = Core::Enums::CullMode::CULL_MODE_NONE;
    rasterState.depthBiasClamp = 0 ;
    rasterState.depthBiasConstantFactor = 0;
    rasterState.depthBiasEnable = false;
    rasterState.depthBiasSlopeFactor = 0;
    rasterState.depthClampEnable = false;
    rasterState.frontFace = Core::Enums::FrontFace::FRONT_FACE_CLOCKWISE;
    rasterState.lineWidth = 2.0f;
    rasterState.polygonMode = Core::Enums::PolygonMode::POLYGON_MODE_LINE;
    rasterState.rasterizerDiscardEnable = false;
    rasterState.state = Core::Enums::PipelineStates::RasterizationState;
    rasterWrapper.rasterState = &rasterState;
    auto rasterStateId = VulkanInterfaceAlias::CreateRasterisationState(rasterWrapper);

    auto taskObj = static_cast<Renderer::RenderGraph::Tasks::RenderTask*>(((Renderer::RenderGraph::TaskNode*)(m_taskNode.m_nodeBase.get()))->GetTask());
    taskObj->SetRasterisationStateId(rasterStateId);
}

Renderer::RenderGraph::Techniques::BoundRenderer::BoundRenderer(
    Core::Utility::RenderData& renderData, 
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::string& name, const std::string& effectName,
    const Core::Utility::EffectInfo& effectInfo,
    Renderer::RenderGraph::Technique::graphNodeAlias* depthInput,
    Renderer::RenderGraph::Technique::graphNodeAlias* colorInput,
    bool active = true):
    Technique(graph, name, funcs, effectName, effectInfo, active),
    m_renderData(renderData),
    m_renderWidth(windowSettings.m_renderWidth),
    m_renderHeight(windowSettings.m_renderHeight)
{
    CreateResources();

    auto& colorImages = ((Renderer::RenderGraph::ResourceNode*)colorInput->GetNodeData())->GetResource();
    CreateResourceNode(GetNodeName("ColorOutputNode"), colorImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_colorOutput);

    auto& depthImages = ((Renderer::RenderGraph::ResourceNode*)depthInput->GetNodeData())->GetResource();
    CreateResourceNode(GetNodeName("DepthOutputNode"), depthImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_depthOutput);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_colorOutput.m_graphNode);
    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_depthOutput.m_graphNode);

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, depthInput, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, colorInput, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
    auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, name);
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)m_taskNode.m_nodeBase.get())->GetTask();
    auto taskName = taskObj->GetTaskName();
    m_boundRenderTaskId = VulkanInterfaceAlias::GetTaskId(effectId, techId, taskName);
}

Renderer::RenderGraph::Techniques::BoundRenderer::~BoundRenderer()
{
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::BoundRenderer::GetGraphOriginResourceNodes()
{
    return {};
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::BoundRenderer::GetGraphEndResourceNodes()
{
    return { m_depthOutput.m_graphNode, m_colorOutput.m_graphNode };
}

void Renderer::RenderGraph::Techniques::BoundRenderer::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
    Renderer::RenderGraph::Tasks::DrawInfo drawInfo{};
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)(m_taskNode.m_nodeBase.get()))->GetTask();

    std::map<Core::Enums::ResourceSets, std::map<uint32_t, Renderer::RenderGraph::SetInfo>> setInfoMap;

    // Filtering =====================================
    std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>> filteredDataList;

    FilterInfo camFilterInfo{};
    camFilterInfo.m_data = (void*)m_renderData.m_cameraData.data();
    camFilterInfo.m_dataCount = m_renderData.m_cameraData.size();
    camFilterInfo.m_setType = Core::Enums::CAMERA;
    camFilterInfo.m_stride = sizeof(Core::Utility::CameraData);
    Filter(camFilterInfo, Renderer::RenderGraph::CameraFilter, filteredDataList, setInfoMap);

    auto boundFilter = [](uint32_t index, void* data, void* next)
    {
        return true;
    };

    FilterInfo boundFilterInfo{};
    boundFilterInfo.m_data = (void*)m_renderData.m_boundData.data();
    boundFilterInfo.m_dataCount = m_renderData.m_boundData.size();
    boundFilterInfo.m_setType = Core::Enums::TRANSFORM;
    boundFilterInfo.m_stride = sizeof(Core::Utility::BoundData);
    Filter(boundFilterInfo, boundFilter, filteredDataList, setInfoMap);

    // ======================
    if (setInfoMap.find(Core::Enums::TRANSFORM) != setInfoMap.end())
    {
        for( auto& data : m_renderData.m_boundData)
            Renderer::RenderGraph::CreateDrawInfo(data, m_boundRenderTaskId, drawInfo);

        CreateSetInfo(setInfoMap, drawInfo);
        ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
    }
    else
    {
        m_techniqueActive = false;
    }
}


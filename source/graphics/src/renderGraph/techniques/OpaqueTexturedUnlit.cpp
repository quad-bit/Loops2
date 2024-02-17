#include "renderGraph/techniques/OpaqueTexturedUnlit.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>

void Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::CreateResources()
{

    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    CreateTaskGraphNode(GetNodeName("OpaqueRenderTask"), m_parentEffectName, m_name, g_renderArea,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_taskNode);

}

Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::OpaqueTexturedUnlit(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs, const std::string& name,
    const std::string& effectName,
    const std::vector<ResourceAlias*>& colorImages,
    const std::vector<ResourceAlias*>& depthImages,
    const Core::Utility::EffectInfo& effectInfo) :
    Technique(graph, name, funcs, effectName, effectInfo),
    m_renderData(renderData),
    m_renderWidth(windowSettings.m_renderWidth),
    m_renderHeight(windowSettings.m_renderHeight)
{
    CreateResources();
    CreateResourceNode(GetNodeName("ColorOutputNode"), colorImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_colorOutput);
    CreateResourceNode(GetNodeName("DepthOutputNode"), depthImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_depthOutput);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_colorOutput.m_graphNode);
    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_depthOutput.m_graphNode);

    auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
    auto techId = VulkanInterfaceAlias::GetTechniqueId(effectId, name);
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)m_taskNode.m_nodeBase.get())->GetTask();
    auto taskName = taskObj->GetTaskName();
    m_opaqueRenderTaskId = VulkanInterfaceAlias::GetTaskId(effectId, techId, taskName);
}

Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::~OpaqueTexturedUnlit()
{
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::GetGraphOriginResourceNodes()
{
    return { };
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::GetGraphEndResourceNodes()
{
    return { m_depthOutput.m_graphNode, m_colorOutput.m_graphNode};
}

void Renderer::RenderGraph::Techniques::OpaqueTexturedUnlit::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
    Renderer::RenderGraph::Tasks::DrawInfo drawInfo{};
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)m_taskNode.m_nodeBase.get())->GetTask();

    std::map<Core::Enums::ResourceSets, std::map<uint32_t, Renderer::RenderGraph::SetInfo>> setInfoMap;

    // Filtering =====================================
    std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>> filteredDataList;

    FilterInfo camFilterInfo{};
    camFilterInfo.m_data = (void*)m_renderData.m_cameraData.data();
    camFilterInfo.m_dataCount = m_renderData.m_cameraData.size();
    camFilterInfo.m_setType = Core::Enums::CAMERA;
    camFilterInfo.m_stride = sizeof(Core::Utility::CameraData);
    Filter(camFilterInfo, Renderer::RenderGraph::CameraFilter, filteredDataList, setInfoMap);

    FilterInfo matFilterInfo{};
    matFilterInfo.m_data = (void*)m_renderData.m_materialData.data();
    matFilterInfo.m_dataCount = m_renderData.m_materialData.size();
    matFilterInfo.m_setType = Core::Enums::MATERIAL;
    matFilterInfo.m_stride = sizeof(Core::Utility::MaterialData);
    matFilterInfo.m_next = (void*)&m_effectInfo;
    Filter(matFilterInfo, Renderer::RenderGraph::MaterialFilter, filteredDataList, setInfoMap);

    // Pick =============================
    struct PickInfo
    {
        uint32_t m_taskId;
        Renderer::RenderGraph::Tasks::DrawInfo& m_drawInfo;

        PickInfo(
            uint32_t taskId,
            Renderer::RenderGraph::Tasks::DrawInfo& drawInfo
        ) :
            m_taskId(taskId),
            m_drawInfo(drawInfo)
        {
        }
    };


    // the pick function know how to use the funcData ( pick data in this case )
    auto taskName = taskObj->GetTaskName();
    PickInfo pickInfo(m_opaqueRenderTaskId, drawInfo);
    auto PickFunc = [](void* setData, void* funcData)
    {
        auto pickInfo = static_cast<PickInfo*>(funcData);
        auto transformData = static_cast<Core::Utility::TransformData*>(setData);
        Renderer::RenderGraph::CreateDrawInfo(*transformData, pickInfo->m_taskId, pickInfo->m_drawInfo);
    };

    auto funcData = static_cast<void*>(&pickInfo);
    Renderer::RenderGraph::Pick<Core::Utility::MaterialData, Core::Utility::TransformData, Core::Enums::TRANSFORM>(
        filteredDataList[Core::Enums::MATERIAL], filteredDataList, setInfoMap,
        (void*)m_renderData.m_transformData.data(),
        m_renderData.m_transformData.size(), sizeof(Core::Utility::TransformData),
        PickFunc, funcData);

    // ======================

    // This tech requires material set id and its transform child indicies
    if (filteredDataList[Core::Enums::MATERIAL].size() > 0)
    {
        CreateSetInfo(setInfoMap, drawInfo);
        ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
    }
}


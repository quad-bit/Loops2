#include "renderGraph/techniques/OpaqueUnlit.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>


void Renderer::RenderGraph::Techniques::OpaqueUnlit::CreateResources()
{
    Core::Enums::ImageType type{ Core::Enums::ImageType::IMAGE_TYPE_2D };
    std::vector<Core::Enums::ImageUsage> usages{
        Core::Enums::ImageUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };

    Core::Wrappers::ImageCreateInfo info{};
    info.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.m_depth = 1;
    info.m_format = VulkanInterfaceAlias::FindBestDepthFormat();
    info.m_height = m_renderHeight;
    info.m_width = m_renderWidth;
    info.m_imageType = type;
    info.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
    info.m_layers = 1;
    info.m_mips = 1;
    info.m_sampleCount = Core::Enums::Samples::SAMPLE_COUNT_1_BIT;
    info.m_usages = usages;
    info.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;

    m_depthAttachments = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(
        info, std::vector<std::string>({ "Depth_0", "Depth_1", "Depth_2" }));

    CreateResourceNode(GetNodeName("DepthInputNode"), m_depthAttachments,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_depthInput);

    m_backBufferImages = m_callbackUtility.m_resourceCreationCallback.GetSwapchainImagesFunc();

    CreateResourceNode(GetNodeName("ColorInputNode"), m_backBufferImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_colorInput);

    CreateResourceNode(GetNodeName("ColorOutputNode"), m_backBufferImages,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_colorOutput);

    CreateResourceNode(GetNodeName("DepthOutputNode"), m_depthAttachments,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_depthOutput);


    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    CreateTaskGraphNode("OpaqueRenderTask", m_parentEffectName, m_name, g_renderArea,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_taskNode);

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(m_graph, m_depthInput.m_graphNode, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
        Core::Enums::ImageLayout::LAYOUT_UNDEFINED);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(m_graph, m_colorInput.m_graphNode, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR, 0);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_colorOutput.m_graphNode);
    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_depthOutput.m_graphNode);
}

Renderer::RenderGraph::Techniques::OpaqueUnlit::OpaqueUnlit(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs, const std::string& name,
    const std::string& effectName,
    const Core::Utility::EffectInfo& effectInfo) :
    Technique(graph, name, funcs, effectName, effectInfo),
    m_renderData(renderData),
    m_renderWidth(windowSettings.m_renderWidth),
    m_renderHeight(windowSettings.m_renderHeight)
{
    CreateResources();
}

Renderer::RenderGraph::Techniques::OpaqueUnlit::~OpaqueUnlit()
{
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueUnlit::GetGraphOriginResourceNodes()
{
    return { m_depthInput.m_graphNode, m_colorInput.m_graphNode };
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueUnlit::GetGraphEndResourceNodes()
{
    return { m_depthOutput.m_graphNode, m_colorOutput.m_graphNode };
}

void Renderer::RenderGraph::Techniques::OpaqueUnlit::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
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
        const std::string& m_parentEffectName;
        const std::string& m_techName;
        const std::string& m_taskName;
        Renderer::RenderGraph::Tasks::DrawInfo& m_drawInfo;

        PickInfo(
            const std::string& parentEffectName,
            const std::string& techName,
            const std::string& taskName,
            Renderer::RenderGraph::Tasks::DrawInfo& drawInfo
        ):
            m_parentEffectName(parentEffectName),
            m_techName(techName),
            m_taskName(taskName),
            m_drawInfo(drawInfo)
        {
        }
    };

    auto taskName = taskObj->GetTaskName();
    PickInfo pickInfo(m_parentEffectName, m_name, taskName, drawInfo);
    auto PickFunc = [](void* setData, void* funcData)
    {
        auto pickInfo = static_cast<PickInfo*>(funcData);
        auto transformData = static_cast<Core::Utility::TransformData*>(setData);
        Renderer::RenderGraph::CreateDrawInfo(*transformData,
            pickInfo->m_parentEffectName, pickInfo->m_techName, pickInfo->m_taskName,
            pickInfo->m_drawInfo);
    };

    auto funcData = static_cast<void*>(&pickInfo);
    Renderer::RenderGraph::Pick<Core::Utility::MaterialData, Core::Utility::TransformData, Core::Enums::TRANSFORM>(
        filteredDataList[Core::Enums::MATERIAL], filteredDataList, setInfoMap,
        (void*)m_renderData.m_transformData.data(),
        m_renderData.m_transformData.size(), sizeof(Core::Utility::TransformData),
        PickFunc, funcData);

    // ======================

    CreateSetInfo(setInfoMap, drawInfo);

    ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
}


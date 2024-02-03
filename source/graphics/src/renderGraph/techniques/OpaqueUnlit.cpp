#include "renderGraph/techniques/OpaqueUnlit.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>

namespace
{
    void CreateTaskGraphNode(
        const std::string& taskName,
        const std::string& m_parentEffectName,
        const std::string& m_name,
        const Core::Wrappers::Rect2D& renderArea,
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& taskNodeBase,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* graphNode,
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
        const Renderer::RenderGraph::Utils::GraphTraversalCallback& graphTraversal,
        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& taskNodes)
    {
        /*std::unique_ptr<Renderer::RenderGraph::Task> task = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>(taskName, renderArea, m_parentEffectName, m_name);
        taskNodeBase = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task), graphTraversal);
        graphNode = graph.Push(taskNodeBase.get());
        taskNodes.push_back(graphNode);*/

    }
}

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

    m_depthInputNodeBase = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_depthAttachments, "DepthInputNode", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    m_depthInputGraphNode = m_graph.Push(m_depthInputNodeBase.get());
    m_resourceNodes.push_back(m_depthInputGraphNode);

    m_backBufferImages = m_callbackUtility.m_resourceCreationCallback.GetSwapchainImagesFunc();

    m_colorInputNodeBase = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_backBufferImages, "ColorInputNode", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    m_colorInputGraphNode = m_graph.Push(m_colorInputNodeBase.get());
    m_resourceNodes.push_back(m_colorInputGraphNode);

    m_colorOutputNodeBase = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_backBufferImages, "ColorOutputNode", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    m_colorOutputGraphNode = m_graph.Push(m_colorOutputNodeBase.get());
    m_resourceNodes.push_back(m_colorOutputGraphNode);

    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    m_opaqueRenderTask = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("OpaqueRenderTask", g_renderArea, m_parentEffectName, m_name);
    m_renderTaskNodeBase = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(m_opaqueRenderTask), m_callbackUtility.m_graphTraversalCallback);
    m_renderTaskGraphNode = m_graph.Push(m_renderTaskNodeBase.get());
    m_taskNodes.push_back(m_renderTaskGraphNode);

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(m_graph, m_depthInputGraphNode, m_renderTaskGraphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
        Core::Enums::ImageLayout::LAYOUT_UNDEFINED);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(m_graph, m_colorInputGraphNode, m_renderTaskGraphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR, 0);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_renderTaskGraphNode, m_colorOutputGraphNode);
}

Renderer::RenderGraph::Techniques::OpaqueUnlit::OpaqueUnlit(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs, const std::string& name,
    const std::string& effectName) :
    Technique(graph, name, funcs, effectName),
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
    return { m_depthInputGraphNode, m_colorInputGraphNode };
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueUnlit::GetGraphEndResourceNodes()
{
    return { m_colorOutputGraphNode };
}


void Renderer::RenderGraph::Techniques::OpaqueUnlit::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
    Renderer::RenderGraph::Tasks::DrawInfo drawInfo{};
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)m_renderTaskNodeBase.get())->GetTask();

    std::map<Core::Enums::ResourceSets, std::map<uint32_t, Renderer::RenderGraph::SetInfo>> setInfoMap;

    // Filtering =====================================
    std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>> filteredDataList;

    /*std::function<bool(uint32_t index, void* data, const Core::Enums::ResourceSets& setType)> camFilter =
    [](uint32_t index, void* data, const Core::Enums::ResourceSets& setType) -> bool
    {
        if (index == 0)
            return true;
        else
            return false;
    };

    std::function<bool(uint32_t index, void* data, const Core::Enums::ResourceSets& setType)> matFilter =
    [this](uint32_t index, void* data, const Core::Enums::ResourceSets& setType) -> bool
    {
        auto matData = static_cast<Core::Utility::MaterialData*>(data);
        if (matData->m_effectType == Core::ECS::Components::EffectType::OPAQUE_E &&
            matData->m_techniqueName == m_name)
        {
            return true;
        }
        return false;
    };*/

    /*auto Filter = [&filteredDataList](void* data, uint32_t count, uint32_t stride,
        const Core::Enums::ResourceSets& setType,
        const std::function<bool(uint32_t index, void* data, const Core::Enums::ResourceSets& setType)>& filterFunc)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (filterFunc(i, data, setType))
            {
                auto pair = std::pair(i, data);
                if (filteredDataList.find(setType) == filteredDataList.end())
                {
                    filteredDataList.insert({ setType, {{pair}} });
                }
                else
                {
                    filteredDataList[setType].push_back(pair);
                }
            }

            data = static_cast<char*>(data) + stride;
        }
    };*/

    /*Filter((void*)m_renderData.m_cameraData.data(), m_renderData.m_cameraData.size(),
        sizeof(Core::Utility::CameraData), Core::Enums::CAMERA, Renderer::RenderGraph::CameraFilter,
        filteredDataList, setInfoMap);*/

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
    matFilterInfo.m_next = (void*)&m_name;
    Filter(matFilterInfo, Renderer::RenderGraph::MaterialFilter, filteredDataList, setInfoMap);

    // as child indicies are available for transform in mat and there are no filtering params 
    // in transform at this moment
    auto& matDataList = filteredDataList[Core::Enums::MATERIAL];
    for (auto& matData : matDataList)
    {
        auto mat = static_cast<Core::Utility::MaterialData*>(matData.second);
        auto& transformIndicies = mat->m_childSetIndicies;

        for (auto index : transformIndicies)
        {
            auto pair = std::pair(index, (void*)&m_renderData.m_transformData[index]);
            if (filteredDataList.find(Core::Enums::TRANSFORM) == filteredDataList.end())
            {
                filteredDataList.insert({ Core::Enums::TRANSFORM, {{pair}} });
            }
            else
            {
                filteredDataList[Core::Enums::TRANSFORM].push_back(pair);
            }

            CreateDrawInfo(m_renderData.m_transformData[index],
                m_parentEffectName, m_name, taskObj->GetTaskName(),
                drawInfo);

            SetInfo setInfo{};
            setInfo.m_setValue = Core::Enums::TRANSFORM;
            setInfo.m_descriptorSetId = m_renderData.m_transformData[index].m_descriptorSetId;

            if (setInfoMap.find(Core::Enums::TRANSFORM) == setInfoMap.end())
            {
                setInfoMap.insert({ Core::Enums::TRANSFORM, { { index, setInfo } } });
            }
            else
            {
                setInfoMap[Core::Enums::TRANSFORM].insert({ index, setInfo });
            }
        }
    }

    CreateSetInfo(setInfoMap, drawInfo);

    ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
}


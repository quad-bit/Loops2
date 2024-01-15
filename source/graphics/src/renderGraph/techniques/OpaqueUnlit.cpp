#include "renderGraph/techniques/OpaqueUnlit.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>

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

void Renderer::RenderGraph::Techniques::OpaqueUnlit::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
    auto taskObj = ((Renderer::RenderGraph::TaskNode*)m_renderTaskNodeBase.get())->GetTask();

    Renderer::RenderGraph::Tasks::DrawInfo drawInfo{};
    std::map<uint32_t, std::vector<uint32_t>> setIdMap;

    uint32_t minSetVal = Core::Enums::ResourceSets::CAMERA, maxSetVal = Core::Enums::ResourceSets::TRANSFORM;
    for (uint32_t i = minSetVal; i < maxSetVal; i++)
    {
        setIdMap.insert({ i, {} });
    }

    setIdMap[Core::Enums::ResourceSets::CAMERA].push_back(m_renderData.m_cameraData[0].m_descriptorSetId);

    auto& transformData = m_renderData.m_transformData;
    for (auto& data : transformData)
    {
        Renderer::RenderGraph::Tasks::MeshDrawInfo meshInfo{};
        if (data.m_indexBufferId.has_value())
        {
            Core::Wrappers::IndexBufferBindingInfo indexInfo{};
            indexInfo.bufferId = data.m_indexBufferId.value();
            indexInfo.indexType = Core::Enums::IndexType::INDEX_TYPE_UINT32;
            indexInfo.offset = 0;
            meshInfo.m_indexBufferInfo = indexInfo;
            meshInfo.m_indexCount = data.m_indexCount.value();
        }

        auto& vertexBufferBindingTypeList = VulkanInterfaceAlias::GetVertexBindingTypeInfo(m_parentEffectName, m_name, taskObj->GetTaskName());

        Core::Wrappers::VertexBufferBindingInfo vertexInfo{};
        vertexInfo.bindingCount = 0;
        vertexInfo.firstBinding = 0;
        for (auto& bindingType : vertexBufferBindingTypeList)
        {
            vertexInfo.pOffsets.push_back({ 0 });
            vertexInfo.bindingCount++;

            switch (bindingType.m_bindingType)
            {
            case Core::Enums::VertexAttributeType::POSITION:
                vertexInfo.bufferIds.push_back(data.m_positionBufferId);
                break;
            case Core::Enums::VertexAttributeType::COLOR:
                if (!data.m_colorBufferId.has_value())
                    ASSERT_MSG_DEBUG(0, "No Color attrib in the json file");
                vertexInfo.bufferIds.push_back(data.m_colorBufferId.value());
                break;
            case Core::Enums::VertexAttributeType::NORMAL:
                if (!data.m_normalBufferId.has_value())
                    ASSERT_MSG_DEBUG(0, "No Normal attrib in the json file");
                vertexInfo.bufferIds.push_back(data.m_normalBufferId.value());
                break;
                /*case Core::Enums::VertexAttributeType::TANGENT:
                    vertexInfo.bufferIds.push_back(data.m_positionBufferId);
                    break;*/
            }
        }
        meshInfo.m_vertexBufferInfo.push_back(vertexInfo);

        meshInfo.m_vertexCount = data.m_vertexCount;

        setIdMap[Core::Enums::ResourceSets::TRANSFORM].push_back(data.m_descriptorSetId);
        drawInfo.m_meshInfoList.push_back(meshInfo);
    }

    //setIdMap.insert({ 0, {2, 3} });
    //setIdMap.insert({ 1, {5, 8, 10} });
    //setIdMap.insert({ 2, {12, 15} });
    //setIdMap.insert({ 3, {} });
    //setIdMap.insert({ 4, {17, 21, 25, 26} });

    uint32_t totalSetCount = (uint32_t)Core::Enums::ResourceSets::TRANSFORM + 1;
    uint32_t bindCount = 0, firstSet = totalSetCount;
    std::vector<int> idList;
    std::function<void(uint32_t)> iterate;

    auto& meshInfoList = drawInfo.m_meshInfoList;

    uint32_t index = 0;
    iterate = [&iterate, &setIdMap, &maxSetVal, &idList, &bindCount, &firstSet, &meshInfoList, &index](uint32_t setVal)
    {
        if (setVal > maxSetVal)
            return;

        auto FillStack = [&iterate, &setIdMap, &maxSetVal, &idList, &bindCount, &firstSet, &meshInfoList, &index](int id, uint32_t setVal)
        {
            idList.push_back(id);
            bindCount++;
            firstSet--;
            iterate(setVal + 1);

            if (setVal == maxSetVal)
            {
                /*for (auto& id : idList)
                    std::cout << id << " ";
                std::cout << "\t" << bindCount << "\t" << firstSet << "\n";*/

                Core::Wrappers::DescriptorSetBindingInfo info{};
                info.descriptorSetIds = idList;
                info.firstSet = firstSet;
                info.dynamicOffsetCount = 0;
                info.numSetsToBind = bindCount;
                info.pDynamicOffsets = nullptr;
                info.pipelineBindPoint = Core::Enums::PipelineType::GRAPHICS;

                meshInfoList[index++].m_descriptorInfo = info;
            }

            bindCount = 0;

            firstSet = maxSetVal + 1;
            idList.erase(idList.end() - 1);
        };

        if (setIdMap[setVal].size() == 0)
        {
            FillStack(-1, setVal);
        }
        else
        {
            for (auto& id : setIdMap[setVal])
            {
                FillStack(id, setVal);
            }
        }
    };

    iterate(minSetVal);

    ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueUnlit::GetGraphOriginResourceNodes()
{
    return { m_depthInputGraphNode, m_colorInputGraphNode };
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::OpaqueUnlit::GetGraphEndResourceNodes()
{
    return { m_colorOutputGraphNode };
}

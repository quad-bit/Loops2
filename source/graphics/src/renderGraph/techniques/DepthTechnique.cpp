#include "renderGraph/techniques/DepthTechnique.h"
#include "Settings.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>
#include <stack>

namespace
{
    template <typename T>
    void dump_stack(std::stack<T>& stack) {
        std::stack<T> temp;
        while (!stack.empty()) {
            T top = stack.top(); stack.pop();
            std::cout << top << " ";
            temp.push(top);
        }
        while (!temp.empty()) {
            T top = temp.top(); temp.pop();
            stack.push(top);
        }
    }

};


void Renderer::RenderGraph::Techniques::DepthTechnique::CreateResources()
{
    Core::Enums::ImageType type{ Core::Enums::ImageType::IMAGE_TYPE_2D };
    std::vector<Core::Enums::ImageUsage> usages{
        Core::Enums::ImageUsage::USAGE_SAMPLED_BIT,
        Core::Enums::ImageUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        Core::Enums::ImageUsage::USAGE_TRANSFER_SRC_BIT };

    Core::Wrappers::ImageCreateInfo info{};
    info.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.m_depth = 1;
    info.m_format = VulkanInterfaceAlias::FindBestDepthFormat();
    info.m_height = m_renderHeight;
    info.m_imageType = type;
    info.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
    info.m_layers = 1;
    info.m_mips = 1;
    info.m_sampleCount = Core::Enums::Samples::SAMPLE_COUNT_1_BIT;
    info.m_usages = usages;
    info.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;
    info.m_width = m_renderWidth;

    m_depthAttachments = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(
        info, std::vector<std::string>({ "Depth_0", "Depth_1", "Depth_2" }));

    depthInputRes = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_depthAttachments, "DepthInputNode", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    depthResInputNode = m_graph.Push(depthInputRes.get());
    m_resourceNodes.push_back(depthResInputNode);

    depthOutputRes = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_depthAttachments, "DepthOutputNode", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    depthResOutputNode = m_graph.Push(depthOutputRes.get());
    m_resourceNodes.push_back(depthResOutputNode);

    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    task = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("DepthPassRenderTask", g_renderArea, m_parentEffectName, m_name);
    taskNodeBase = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task), m_callbackUtility.m_graphTraversalCallback);
    taskNode = m_graph.Push(taskNodeBase.get());
    m_taskNodes.push_back(taskNode);

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(m_graph, depthResInputNode, taskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
        Core::Enums::ImageLayout::LAYOUT_UNDEFINED);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, taskNode, depthResOutputNode);

    transferTask = std::make_unique<Renderer::RenderGraph::Tasks::TransferTask>("CopyToBackBufferTask");
    transferNodeBase = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(transferTask), m_callbackUtility.m_graphTraversalCallback);
    transferTaskNode = m_graph.Push(transferNodeBase.get());
    m_taskNodes.push_back(transferTaskNode);

    m_backBufferImages = m_callbackUtility.m_resourceCreationCallback.GetSwapchainImagesFunc();
    transferInput = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_backBufferImages, "transferInput", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    transferInputNode = m_graph.Push(transferInput.get());
    m_resourceNodes.push_back(transferInputNode);

    transferOutput = std::make_unique<Renderer::RenderGraph::ResourceNode>(m_backBufferImages, "transferOutput", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
    transferOutputNode = m_graph.Push(transferOutput.get());
    m_resourceNodes.push_back(transferOutputNode);

    Renderer::RenderGraph::Utils::AddEdge(m_graph, depthResOutputNode, transferTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
        Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddEdge(m_graph, transferInputNode, transferTaskNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
        Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL,
        Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, transferTaskNode, transferOutputNode);
}

Renderer::RenderGraph::Techniques::DepthTechnique::DepthTechnique(
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

Renderer::RenderGraph::Techniques::DepthTechnique::~DepthTechnique()
{
    //((Renderer::RenderGraph::Tasks::RenderTask*)task.get())->DestroyGraphicsPipeline();
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::DepthTechnique::GetGraphOriginResourceNodes()
{
    return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>();
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::DepthTechnique::GetGraphEndResourceNodes()
{
    return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>();
}

void Renderer::RenderGraph::Techniques::DepthTechnique::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
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

        Core::Wrappers::VertexBufferBindingInfo vertexInfo{};
        vertexInfo.bindingCount = 1;

        ASSERT_MSG_DEBUG(0, "Fix this");
        //vertexInfo.bufferIds = data.m_vertexBufferId;
        
        vertexInfo.firstBinding = 0;
        vertexInfo.pOffsets = { 0 };
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

    auto taskObj = ((Renderer::RenderGraph::TaskNode*)taskNodeBase.get())->GetTask();
    ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
}

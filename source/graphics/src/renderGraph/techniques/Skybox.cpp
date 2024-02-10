#include "renderGraph/techniques/Skybox.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/RenderTask.h>
#include <renderGraph/tasks/TransferTask.h>
#include <Math/Cube.h>
#include <Utility/PipelineStateWrappers.h>

void Renderer::RenderGraph::Techniques::Skybox::CreateResources()
{
    Core::Wrappers::Rect2D g_renderArea{};
    g_renderArea.lengthX = m_renderWidth;
    g_renderArea.lengthY = m_renderHeight;
    g_renderArea.offsetX = 0;
    g_renderArea.offsetY = 0;

    CreateTaskGraphNode(GetNodeName("SkyboxRenderTask"), m_parentEffectName, m_name, g_renderArea,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_taskNode);

    Core::Utility::DepthStencilStateWrapper depthWrapper{};
    Core::Wrappers::DepthStencilState depthState{};
    depthState.back.compareMask = 0;
    depthState.back.compareOp = Core::Enums::CompareOp::COMPARE_OP_ALWAYS;
    depthState.back.depthFailOp = Core::Enums::StencilOp::STENCIL_OP_KEEP;
    depthState.back.failOp = Core::Enums::StencilOp::STENCIL_OP_KEEP;
    depthState.back.passOp = Core::Enums::StencilOp::STENCIL_OP_KEEP;
    depthState.back.reference = 0;
    depthState.back.writeMask = 0;
    depthState.depthBoundsTestEnable = false;
    depthState.depthCompareOp = Core::Enums::CompareOp::COMPARE_OP_LESS_OR_EQUAL;
    depthState.depthTestEnable = true;
    depthState.depthWriteEnable = true;
    depthState.front = depthState.back;
    depthState.depthBoundsTestEnable = false;
    depthState.maxDepthBounds = 0;
    depthState.minDepthBounds = 0;
    depthState.state = Core::Enums::PipelineStates::DepthStencilState;
    depthState.stencilTestEnable = false;
    depthWrapper.depthState = &depthState;
    auto depthStateId = VulkanInterfaceAlias::CreateDepthStencilState(depthWrapper);

    Core::Utility::RasterizationStateWrapper rasterWrapper{};
    Core::Wrappers::RasterizationState rasterState{};
    rasterState.cullMode = Core::Enums::CullMode::CULL_MODE_BACK_BIT;
    rasterState.depthBiasClamp =0 ;
    rasterState.depthBiasConstantFactor = 0;
    rasterState.depthBiasEnable = false;
    rasterState.depthBiasSlopeFactor = 0;
    rasterState.depthClampEnable = false;
    rasterState.frontFace = Core::Enums::FrontFace::FRONT_FACE_COUNTER_CLOCKWISE;
    rasterState.lineWidth = 1.0f;
    rasterState.polygonMode = Core::Enums::PolygonMode::POLYGON_MODE_FILL;
    rasterState.rasterizerDiscardEnable = false;
    rasterState.state = Core::Enums::PipelineStates::RasterizationState;
    rasterWrapper.rasterState = &rasterState;
    auto rasterStateId = VulkanInterfaceAlias::CreateRasterisationState(rasterWrapper);

    auto taskObj = static_cast<Renderer::RenderGraph::Tasks::RenderTask*>(((Renderer::RenderGraph::TaskNode*)(m_taskNode.m_nodeBase.get()))->GetTask());
    taskObj->SetDepthStencilStateId(depthStateId);
    taskObj->SetRasterisationStateId(rasterStateId);
}

Renderer::RenderGraph::Techniques::Skybox::Skybox(
    Core::Utility::RenderData& renderData, 
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::string& name, const std::string& effectName,
    const Core::Utility::EffectInfo& effectInfo,
    Renderer::RenderGraph::Technique::graphNodeAlias* depthInput,
    Renderer::RenderGraph::Technique::graphNodeAlias* colorInput):
    Technique(graph, name, funcs, effectName, effectInfo),
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

    Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(graph, depthInput, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, colorInput, m_taskNode.m_graphNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
        Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0);

    {
        Core::Math::CubeIndexed obj{};

        std::vector<glm::vec3> positions = obj.positions;
        size_t numVertices = obj.indices.size();
        positions.resize(numVertices);

        for (int i = 0; i < numVertices; i++)
        {
            positions[i] = obj.positions[obj.indices[i]];
        }


        auto CreateBuffer = [](const size_t& dataSize, uint32_t& bufferId, uint32_t& memId)
        {
            Core::Enums::BufferType bufferType = Core::Enums::BufferType::VERTEX_BUFFER_BIT;
            Core::Enums::MemoryType memType = Core::Enums::MemoryType::HOST_VISIBLE_BIT;
            Core::Wrappers::BufferInfo info = {};
            info.bufType = &bufferType;
            info.memType = &memType;
            info.memTypeCount = 1;
            info.dataSize = dataSize;

            bufferId = *VulkanInterfaceAlias::CreateBuffers(&info, 1);
            memId = *VulkanInterfaceAlias::AllocateBufferMemory(&bufferId, 1);
        };

        auto UploadData = [](uint32_t bufferId, size_t dataSize, void* data)
        {
            size_t memAlignedSize = dataSize;// VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
            VulkanInterfaceAlias::CopyBufferDataToMemory(bufferId, dataSize, memAlignedSize, data, 0, false);
        };

        //position buffer
        uint32_t memId;
        size_t dataSize = sizeof(glm::vec3) * positions.size();
        CreateBuffer(dataSize, m_transformData.m_positionBufferId, memId);
        UploadData(m_transformData.m_positionBufferId, dataSize, positions.data());

        //m_transformData.m_descriptorSetId = ;
        m_transformData.m_modelMat = glm::mat4(1.0f);
        m_transformData.m_position = glm::vec4(0.0f);
        m_transformData.m_vertexCount = positions.size();
    }

}

Renderer::RenderGraph::Techniques::Skybox::~Skybox()
{
    VulkanInterfaceAlias::DestroyBuffer(&m_transformData.m_positionBufferId, 1);
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::Skybox::GetGraphOriginResourceNodes()
{
    return {};
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::Skybox::GetGraphEndResourceNodes()
{
    return { m_depthOutput.m_graphNode, m_colorOutput.m_graphNode };
}

void Renderer::RenderGraph::Techniques::Skybox::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
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

    auto taskName = taskObj->GetTaskName();

    Renderer::RenderGraph::CreateDrawInfo(m_transformData,
        m_parentEffectName, m_name, taskName, drawInfo);

    CreateSetInfo(setInfoMap, drawInfo);

    ((Renderer::RenderGraph::Tasks::RenderTask*)taskObj)->UpdateDrawInfo(drawInfo);
}


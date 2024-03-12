#include "renderGraph/techniques/LightCull.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/ComputeTask.h>
#include <Utility/PipelineStateWrappers.h>
#include <resourceManagement/UniformFactory.h>
#include <Utility/ResourceAllocationHelper.h>
#include <ECS/ECS_Setting.h>

void Renderer::RenderGraph::Techniques::LightCull::CreateResources()
{
    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(SceneInfo);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT };
        info.m_name = "SceneBuffer";
        m_sceneBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "sceneBuffer_0", "sceneBuffer_1"}));
    }

    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(AABB) * MAX_POINT_LIGHTS;
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT };
        info.m_name = "BoundsBuffer";
        m_boundsBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "boundsBuffer_0", "boundsBuffer_1"}));
    }

    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(TileData);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT, Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT };
        info.m_name = "TileBuffer";
        m_tileBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "TileBuffer_0", "TileBuffer_1"}));
    }

    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(LightDepthInfo);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT, Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT };
        info.m_name = "LightDepthBuffer";
        m_lightDepthBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "LightDepthBuffer_0", "LightDepthBuffer_1" }));
    }

    CreateResourceNode(GetNodeName("SceneInputData"), m_sceneBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_sceneBufferInput);

    CreateResourceNode(GetNodeName("BoundsInputData"), m_boundsBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_boundBufferInput);

    CreateResourceNode(GetNodeName("TileInputData"), m_tileBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_tileBufferInput);

    CreateResourceNode(GetNodeName("TileOutputData"), m_tileBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_tileBufferOutput);

    CreateResourceNode(GetNodeName("LightDepthInputData"), m_lightDepthBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_lightDepthBufferInput);

    CreateResourceNode(GetNodeName("LightDepthOutputData"), m_lightDepthBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_lightDepthBufferOutput);

    uint32_t workGroupX = 10, workGroupY = 10, workGroupZ = 10;
    CreateComputeTaskGraphNode("LightCullTask", m_parentEffectName, m_name,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_taskNode,
        workGroupX, workGroupY, workGroupZ);

    Renderer::RenderGraph::Utils::BufferResourceConnectionInfo bufInfo{};
    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.expectedShader = Core::Enums::ShaderType::COMPUTE;

    Renderer::RenderGraph::Utils::ConnectionInfo connection{};
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_sceneBuffer;
    connection.m_resourceParentNodeId = m_sceneBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_sceneBufferInput.m_graphNode, m_taskNode.m_graphNode, connection);

    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_boundsBuffer;
    connection.m_resourceParentNodeId = m_boundBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_boundBufferInput.m_graphNode, m_taskNode.m_graphNode, connection);

    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_tileBuffer;
    connection.m_resourceParentNodeId = m_tileBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_tileBufferInput.m_graphNode, m_taskNode.m_graphNode, connection);

    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_lightDepthBuffer;
    connection.m_resourceParentNodeId = m_lightDepthBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_lightDepthBufferInput.m_graphNode, m_taskNode.m_graphNode, connection);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_tileBufferOutput.m_graphNode);
    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_taskNode.m_graphNode, m_lightDepthBufferOutput.m_graphNode);

    // Create descriptorset
    {
        uint32_t numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

        Core::Utility::GlobalResourceAllocationConfig allocConfig{};
        allocConfig.numDescriptorSets = numDescriptorSetsPerUniform;
        allocConfig.numMemories = 1; // 1 memory
        allocConfig.numResources = 1; //1 buff

        Core::Utility::GlobalResourceSharingConfig resourceSharingConfig{};
        resourceSharingConfig.maxUniformPerResource = 1;
        resourceSharingConfig.allocatedUniformCount = 0;

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_setNumber = 0;

        {
            size_t sceneUniformSize = sizeof(SceneInfo);
            uint32_t memoryAlignedSceneUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(sceneUniformSize);

            Core::Utility::BufferBindingInfo sceneBufInfo{};
            sceneBufInfo.info.allocationConfig = allocConfig;
            sceneBufInfo.info.dataSizePerDescriptor = sceneUniformSize;
            sceneBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedSceneUniformSize;
            sceneBufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            sceneBufInfo.info.sharingConfig = resourceSharingConfig;
            sceneBufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            sceneBufInfo.bufferIdList.push_back(m_sceneBuffer[0]->GetPhysicalResourceId());
            sceneBufInfo.bufferIdList.push_back(m_sceneBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "Scene";
            bindingDescription.m_bindingNumber = 0;
            bindingDescription.m_numElements = 6;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = sceneBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t boundUniformSize = sizeof(AABB);
            uint32_t memoryAlignedBoundUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(boundUniformSize);

            Core::Utility::BufferBindingInfo boundBufInfo{};
            boundBufInfo.info.allocationConfig = allocConfig;
            boundBufInfo.info.dataSizePerDescriptor = boundUniformSize;
            boundBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedBoundUniformSize;
            boundBufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedBoundUniformSize, allocConfig, resourceSharingConfig);
            boundBufInfo.info.sharingConfig = resourceSharingConfig;
            boundBufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedBoundUniformSize, allocConfig, resourceSharingConfig);
            boundBufInfo.bufferIdList.push_back(m_boundsBuffer[0]->GetPhysicalResourceId());
            boundBufInfo.bufferIdList.push_back(m_boundsBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "AABB";
            bindingDescription.m_bindingNumber = 1;
            bindingDescription.m_numElements = 2;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = boundBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t tileUniformSize = sizeof(TileData);
            uint32_t memoryAlignedTileUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(tileUniformSize);

            Core::Utility::BufferBindingInfo tileBufInfo{};
            tileBufInfo.info.allocationConfig = allocConfig;
            tileBufInfo.info.dataSizePerDescriptor = tileUniformSize;
            tileBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedTileUniformSize;
            tileBufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedTileUniformSize, allocConfig, resourceSharingConfig);
            tileBufInfo.info.sharingConfig = resourceSharingConfig;
            tileBufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedTileUniformSize, allocConfig, resourceSharingConfig);
            tileBufInfo.bufferIdList.push_back(m_tileBuffer[0]->GetPhysicalResourceId());
            tileBufInfo.bufferIdList.push_back(m_tileBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "TileBuffer";
            bindingDescription.m_bindingNumber = 2;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::STORAGE_BUFFER;
            bindingDescription.m_bindingInfo = tileBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t lightDepthUniformSize = sizeof(LightDepthInfo);
            uint32_t memoryAlignedLightDepthUniformSize = UniFactAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(lightDepthUniformSize);

            Core::Utility::BufferBindingInfo lightDepthBufInfo{};
            lightDepthBufInfo.info.allocationConfig = allocConfig;
            lightDepthBufInfo.info.dataSizePerDescriptor = lightDepthUniformSize;
            lightDepthBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedLightDepthUniformSize;
            lightDepthBufInfo.info.offsetsForEachDescriptor = Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedLightDepthUniformSize, allocConfig, resourceSharingConfig);
            lightDepthBufInfo.info.sharingConfig = resourceSharingConfig;
            lightDepthBufInfo.info.totalSize = Core::Utility::GetDataSizeMeantForSharing(memoryAlignedLightDepthUniformSize, allocConfig, resourceSharingConfig);
            lightDepthBufInfo.bufferIdList.push_back(m_lightDepthBuffer[0]->GetPhysicalResourceId());
            lightDepthBufInfo.bufferIdList.push_back(m_lightDepthBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "DepthInfo";
            bindingDescription.m_bindingNumber = 3;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::STORAGE_BUFFER;
            bindingDescription.m_bindingInfo = lightDepthBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }
        setDescription.m_numBindings = setDescription.m_setBindings.size();


        /*auto setWrapper = UniFactAlias::GetInstance()->AllocateSetResources(setDescription);

        resourceSharingConfig.allocatedUniformCount += 1;

        UniFactAlias::GetInstance()->AllocateDescriptorSets(setWrapper, setDescription, allocConfig.numDescriptorSets);*/
    }

}

Renderer::RenderGraph::Techniques::LightCull::LightCull(
    Core::Utility::RenderData& renderData, 
    const Core::WindowSettings& windowSettings,
    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
    const std::string& name, const std::string& effectName,
    const Core::Utility::EffectInfo& effectInfo):
    Technique(graph, name, funcs, effectName, effectInfo),
    m_renderData(renderData),
    m_renderWidth(windowSettings.m_renderWidth),
    m_renderHeight(windowSettings.m_renderHeight)
{
    CreateResources();
}

Renderer::RenderGraph::Techniques::LightCull::~LightCull()
{
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::LightCull::GetGraphOriginResourceNodes()
{
    return {};
}

std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> Renderer::RenderGraph::Techniques::LightCull::GetGraphEndResourceNodes()
{
    return { m_tileBufferOutput.m_graphNode, m_lightDepthBufferOutput.m_graphNode };
}

void Renderer::RenderGraph::Techniques::LightCull::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
}


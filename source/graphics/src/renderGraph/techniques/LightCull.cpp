#include "renderGraph/techniques/LightCull.h"
#include <VulkanInterface.h>
#include <renderGraph/tasks/ComputeTask.h>
#include <Utility/PipelineStateWrappers.h>
#include <resourceManagement/ShaderResourceManager.h>
#include <Utility/ResourceAllocationHelper.h>
#include <ECS/ECS_Setting.h>

void Renderer::RenderGraph::Techniques::LightCull::CreateResources()
{
    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(SceneInfo);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT };
        info.m_name = "SceneBuffer";
        m_sceneBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "SceneBuffer_0", "SceneBuffer_1"}));
    }

    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(LightInfo);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT };
        info.m_name = "LightInfoBuffer";
        m_lightInfoBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "LightInfoBuffer_0", "LightInfoBuffer_1"}));
    }

    {
        Core::Wrappers::BufferCreateInfo info{};
        info.size = sizeof(ClusterInfo);
        info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT };
        info.m_name = "ClusterInfoBuffer";
        m_clusterInfoBuffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "ClusterBoundsBuffer_0", "ClusterBoundsBuffer_1" }));
        m_renderData.m_lightData.m_lightClusterBufferIds.push_back(m_clusterInfoBuffer[0]->GetPhysicalResourceId());
        m_renderData.m_lightData.m_lightClusterBufferIds.push_back(m_clusterInfoBuffer[1]->GetPhysicalResourceId());
    }

    CreateResourceNode(GetNodeName("SceneInputData1"), m_sceneBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_sceneBufferInput_1);

    // Cluster builder
    CreateResourceNode(GetNodeName("ClusterBoundsInputData"), m_clusterInfoBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_clusterInfoBufferInput);

    CreateResourceNode(GetNodeName("ClusterBoundsOutputData"), m_clusterInfoBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_clusterInfoBufferOutput_1);

    // Light culler
    CreateResourceNode(GetNodeName("LightBoundsInputData"), m_lightInfoBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_lightBoundBufferInput);

    CreateResourceNode(GetNodeName("SceneInputData2"), m_sceneBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_sceneBufferInput_2);

    CreateResourceNode(GetNodeName("ClusterLightOutputData"), m_clusterInfoBuffer,
        m_resourceNodes, m_graph, m_callbackUtility.m_graphTraversalCallback,
        m_clusterInfoBufferOutput_2);

    CreateComputeTaskGraphNode("ClusterBuilderTask", m_parentEffectName, m_name,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_clusterBuilderTaskNode,
        CLUSTER_X, CLUSTER_Y, CLUSTER_Z);

    CreateComputeTaskGraphNode("LightCullTask", m_parentEffectName, m_name,
        m_graph, m_callbackUtility.m_graphTraversalCallback, m_taskNodes, m_lightCullTaskNode,
        CLUSTER_X, CLUSTER_Y, CLUSTER_Z);

    Renderer::RenderGraph::Utils::BufferResourceConnectionInfo bufInfo{};
    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.expectedShader = Core::Enums::ShaderType::COMPUTE;

    // Cluster builder
    Renderer::RenderGraph::Utils::ConnectionInfo connection{};
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_sceneBuffer;
    connection.m_resourceParentNodeId = m_sceneBufferInput_1.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_sceneBufferInput_1.m_graphNode, m_clusterBuilderTaskNode.m_graphNode, connection);

    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT;
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_clusterInfoBuffer;
    connection.m_resourceParentNodeId = m_clusterInfoBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_clusterInfoBufferInput.m_graphNode, m_clusterBuilderTaskNode.m_graphNode, connection);

    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_clusterBuilderTaskNode.m_graphNode, m_clusterInfoBufferOutput_1.m_graphNode);

    // Light culler
    bufInfo.expectedShader = Core::Enums::ShaderType::COMPUTE;
    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.previousUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.prevShader = Core::Enums::ShaderType::COMPUTE;
    // cluster bound input which was output of cluster bound task
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_sceneBuffer;
    connection.m_resourceParentNodeId = m_sceneBufferInput_2.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_sceneBufferInput_2.m_graphNode, m_lightCullTaskNode.m_graphNode, connection);

    bufInfo.expectedShader = Core::Enums::ShaderType::COMPUTE;
    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufInfo.previousUsage = Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufInfo.prevShader = Core::Enums::ShaderType::COMPUTE;
    // cluster bound input which was output of cluster bound task
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_clusterInfoBuffer;
    connection.m_resourceParentNodeId = m_clusterInfoBufferOutput_1.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_clusterInfoBufferOutput_1.m_graphNode, m_lightCullTaskNode.m_graphNode, connection);

    bufInfo = {};
    bufInfo.expectedShader = Core::Enums::ShaderType::COMPUTE;
    bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    // light bounds buffer input
    connection.m_bufInfo = bufInfo;
    connection.m_resource = m_lightInfoBuffer;
    connection.m_resourceParentNodeId = m_lightBoundBufferInput.m_graphNode->GetNodeId();
    connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
    Renderer::RenderGraph::Utils::AddEdge(m_graph, m_lightBoundBufferInput.m_graphNode, m_lightCullTaskNode.m_graphNode, connection);

    // cluster light info output
    Renderer::RenderGraph::Utils::AddTaskOutput(m_graph, m_lightCullTaskNode.m_graphNode, m_clusterInfoBufferOutput_2.m_graphNode);
    /*
    // Create descriptorset
    {
        uint32_t numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

        Core::Utility::GlobalResourceAllocationConfig allocConfig{};
        allocConfig.numDescriptorSets = 2;// numDescriptorSetsPerUniform;
        allocConfig.numMemories = 2; // 1 memory
        allocConfig.numResources = 2; //1 buff

        Core::Utility::GlobalResourceSharingConfig resourceSharingConfig{};
        resourceSharingConfig.maxUniformPerResource = 1;
        resourceSharingConfig.allocatedUniformCount = 0;

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_setNumber = 0;


        {
            size_t sceneUniformSize = sizeof(SceneInfo);
            uint32_t memoryAlignedSceneUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(sceneUniformSize);

            Core::Utility::BufferBindingInfo sceneBufInfo{};
            sceneBufInfo.info.allocationConfig = allocConfig;
            sceneBufInfo.info.dataSizePerDescriptor = sceneUniformSize;
            sceneBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedSceneUniformSize;
            sceneBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            sceneBufInfo.info.sharingConfig = resourceSharingConfig;
            sceneBufInfo.info.totalSize = memoryAlignedSceneUniformSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
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
            size_t boundUniformSize = sizeof(BoundInfo);
            uint32_t memoryAlignedBoundUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(boundUniformSize);

            Core::Utility::BufferBindingInfo boundBufInfo{};
            boundBufInfo.info.allocationConfig = allocConfig;
            boundBufInfo.info.dataSizePerDescriptor = boundUniformSize;
            boundBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedBoundUniformSize;
            boundBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedBoundUniformSize, allocConfig, resourceSharingConfig);
            boundBufInfo.info.sharingConfig = resourceSharingConfig;
            boundBufInfo.info.totalSize = memoryAlignedBoundUniformSize;//Core::Utility::GetDataSizeMeantForSharing(memoryAlignedBoundUniformSize, allocConfig, resourceSharingConfig);
            boundBufInfo.bufferIdList.push_back(m_lightBoundsBuffer[0]->GetPhysicalResourceId());
            boundBufInfo.bufferIdList.push_back(m_lightBoundsBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "Bounds";
            bindingDescription.m_bindingNumber = 1;
            bindingDescription.m_numElements = 2;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = boundBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t tileUniformSize = sizeof(TileInfo);
            uint32_t memoryAlignedTileUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(tileUniformSize);

            Core::Utility::BufferBindingInfo tileBufInfo{};
            tileBufInfo.info.allocationConfig = allocConfig;
            tileBufInfo.info.dataSizePerDescriptor = tileUniformSize;
            tileBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedTileUniformSize;
            tileBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedTileUniformSize, allocConfig, resourceSharingConfig);
            tileBufInfo.info.sharingConfig = resourceSharingConfig;
            tileBufInfo.info.totalSize = memoryAlignedTileUniformSize;//Core::Utility::GetDataSizeMeantForSharing(memoryAlignedTileUniformSize, allocConfig, resourceSharingConfig);
            tileBufInfo.bufferIdList.push_back(m_tileBuffer[0]->GetPhysicalResourceId());
            tileBufInfo.bufferIdList.push_back(m_tileBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "TileInfo";
            bindingDescription.m_bindingNumber = 2;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::STORAGE_BUFFER;
            bindingDescription.m_bindingInfo = tileBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t lightDepthUniformSize = sizeof(LightDepthInfo);
            uint32_t memoryAlignedLightDepthUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(lightDepthUniformSize);

            Core::Utility::BufferBindingInfo lightDepthBufInfo{};
            lightDepthBufInfo.info.allocationConfig = allocConfig;
            lightDepthBufInfo.info.dataSizePerDescriptor = lightDepthUniformSize;
            lightDepthBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedLightDepthUniformSize;
            lightDepthBufInfo.info.offsetsForEachDescriptor = { 0,0 };// Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedLightDepthUniformSize, allocConfig, resourceSharingConfig);
            lightDepthBufInfo.info.sharingConfig = resourceSharingConfig;
            lightDepthBufInfo.info.totalSize = memoryAlignedLightDepthUniformSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedLightDepthUniformSize, allocConfig, resourceSharingConfig);
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

        auto setWrapper = ShdrResMgrAlias::GetInstance()->GetSetWrapper(setDescription);
        ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(setWrapper, setDescription, numDescriptorSetsPerUniform);

        for(auto& id : setDescription.m_descriptorSetIds)
            m_descriptorSetIds.push_back(id);
    }*/

    // Cluster builder
    {
        uint32_t numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

        Core::Utility::GlobalResourceAllocationConfig allocConfig{};
        allocConfig.numDescriptorSets = numDescriptorSetsPerUniform;
        allocConfig.numMemories = 2; // 2 memory
        allocConfig.numResources = 2; //2 buff

        Core::Utility::GlobalResourceSharingConfig resourceSharingConfig{};
        resourceSharingConfig.maxUniformPerResource = 1;
        resourceSharingConfig.allocatedUniformCount = 0;

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_setNumber = 0;
        {
            size_t sceneUniformSize = sizeof(SceneInfo);
            uint32_t memoryAlignedSceneUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(sceneUniformSize);

            Core::Utility::BufferBindingInfo sceneBufInfo{};
            sceneBufInfo.info.allocationConfig = allocConfig;
            sceneBufInfo.info.dataSizePerDescriptor = sceneUniformSize;
            sceneBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedSceneUniformSize;
            sceneBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            sceneBufInfo.info.sharingConfig = resourceSharingConfig;
            sceneBufInfo.info.totalSize = memoryAlignedSceneUniformSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
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
            size_t clusterBoundDataSize = sizeof(ClusterInfo);
            uint32_t memoryAlignedClusterBoundsDataSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(clusterBoundDataSize);

            Core::Utility::BufferBindingInfo clusterBoundBufInfo{};
            clusterBoundBufInfo.info.allocationConfig = allocConfig;
            clusterBoundBufInfo.info.dataSizePerDescriptor = clusterBoundDataSize;
            clusterBoundBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedClusterBoundsDataSize;
            clusterBoundBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.info.sharingConfig = resourceSharingConfig;
            clusterBoundBufInfo.info.totalSize = memoryAlignedClusterBoundsDataSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.bufferIdList.push_back(m_clusterInfoBuffer[0]->GetPhysicalResourceId());
            clusterBoundBufInfo.bufferIdList.push_back(m_clusterInfoBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "ClusterInfo";
            bindingDescription.m_bindingNumber = 1;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::STORAGE_BUFFER;
            bindingDescription.m_bindingInfo = clusterBoundBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }
        setDescription.m_numBindings = setDescription.m_setBindings.size();

        auto setWrapper = ShdrResMgrAlias::GetInstance()->GetSetWrapper(setDescription);
        ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(setWrapper, setDescription, numDescriptorSetsPerUniform);

        for (auto& id : setDescription.m_descriptorSetIds)
            m_clusterBuilderDescriptorSetIds.push_back(id);
    }

    // Light cull
    {
        uint32_t numDescriptorSetsPerUniform = Core::Settings::m_maxFramesInFlight;

        Core::Utility::GlobalResourceAllocationConfig allocConfig{};
        allocConfig.numDescriptorSets = numDescriptorSetsPerUniform;
        allocConfig.numMemories = 2; // 2 memory
        allocConfig.numResources = 2; //2 buff

        Core::Utility::GlobalResourceSharingConfig resourceSharingConfig{};
        resourceSharingConfig.maxUniformPerResource = 1;
        resourceSharingConfig.allocatedUniformCount = 0;

        Core::Utility::DescriptorSetInfo setDescription;
        setDescription.m_setNumber = 0;
        {
            size_t sceneUniformSize = sizeof(SceneInfo);
            uint32_t memoryAlignedSceneUniformSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(sceneUniformSize);

            Core::Utility::BufferBindingInfo sceneBufInfo{};
            sceneBufInfo.info.allocationConfig = allocConfig;
            sceneBufInfo.info.dataSizePerDescriptor = sceneUniformSize;
            sceneBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedSceneUniformSize;
            sceneBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            sceneBufInfo.info.sharingConfig = resourceSharingConfig;
            sceneBufInfo.info.totalSize = memoryAlignedSceneUniformSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
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
            size_t lightInfoDataSize = sizeof(LightInfo);
            uint32_t memoryAlignedLightInfoDataSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(lightInfoDataSize);

            Core::Utility::BufferBindingInfo lightBufInfo{};
            lightBufInfo.info.allocationConfig = allocConfig;
            lightBufInfo.info.dataSizePerDescriptor = lightInfoDataSize;
            lightBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedLightInfoDataSize;
            lightBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            lightBufInfo.info.sharingConfig = resourceSharingConfig;
            lightBufInfo.info.totalSize = memoryAlignedLightInfoDataSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            lightBufInfo.bufferIdList.push_back(m_lightInfoBuffer[0]->GetPhysicalResourceId());
            lightBufInfo.bufferIdList.push_back(m_lightInfoBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "LightInfo";
            bindingDescription.m_bindingNumber = 1;
            bindingDescription.m_numElements = 2;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::UNIFORM_BUFFER;
            bindingDescription.m_bindingInfo = lightBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        {
            size_t clusterBoundDataSize = sizeof(ClusterInfo);
            uint32_t memoryAlignedClusterBoundsDataSize = ShdrResMgrAlias::GetInstance()->GetMemoryAlignedDataSizeForBuffer(clusterBoundDataSize);

            Core::Utility::BufferBindingInfo clusterBoundBufInfo{};
            clusterBoundBufInfo.info.allocationConfig = allocConfig;
            clusterBoundBufInfo.info.dataSizePerDescriptor = clusterBoundDataSize;
            clusterBoundBufInfo.info.dataSizePerDescriptorAligned = memoryAlignedClusterBoundsDataSize;
            clusterBoundBufInfo.info.offsetsForEachDescriptor = { 0,0 }; //Core::Utility::CalculateOffsetsForDescInUniform(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.info.sharingConfig = resourceSharingConfig;
            clusterBoundBufInfo.info.totalSize = memoryAlignedClusterBoundsDataSize;// Core::Utility::GetDataSizeMeantForSharing(memoryAlignedSceneUniformSize, allocConfig, resourceSharingConfig);
            clusterBoundBufInfo.bufferIdList.push_back(m_clusterInfoBuffer[0]->GetPhysicalResourceId());
            clusterBoundBufInfo.bufferIdList.push_back(m_clusterInfoBuffer[1]->GetPhysicalResourceId());

            Core::Utility::DescriptorSetBindingInfo bindingDescription;
            bindingDescription.m_bindingName = "ClusterInfo";
            bindingDescription.m_bindingNumber = 2;
            bindingDescription.m_numElements = 1;
            bindingDescription.m_resourceType = Core::Enums::DescriptorType::STORAGE_BUFFER;
            bindingDescription.m_bindingInfo = clusterBoundBufInfo;
            setDescription.m_setBindings.push_back(bindingDescription);
        }

        setDescription.m_numBindings = setDescription.m_setBindings.size();

        auto setWrapper = ShdrResMgrAlias::GetInstance()->GetSetWrapper(setDescription);
        ShdrResMgrAlias::GetInstance()->AllocateDescriptorSets(setWrapper, setDescription, numDescriptorSetsPerUniform);

        for (auto& id : setDescription.m_descriptorSetIds)
            m_lightCullerDescriptorSetIds.push_back(id);
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
    m_cachingEnabled = false;
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
    return { m_clusterInfoBufferOutput_2.m_graphNode};
}

void Renderer::RenderGraph::Techniques::LightCull::SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
{
    Renderer::RenderGraph::Tasks::DispatchInfo dispatchInfo{};
    auto clusterBuilderTask = ((Renderer::RenderGraph::TaskNode*)(m_clusterBuilderTaskNode.m_nodeBase.get()))->GetTask();
    auto lightCullerTask = ((Renderer::RenderGraph::TaskNode*)(m_lightCullTaskNode.m_nodeBase.get()))->GetTask();

    /*glm::vec4 posVs(-262.7f, -147.8f, -256.0f, 0.0f);
    glm::vec4 posWs = glm::inverse(m_renderData.m_cameraData[0].m_viewMat) * posVs;

    glm::vec4 pos2Cs = m_renderData.m_cameraData[0].m_projectionMat * m_renderData.m_cameraData[0].m_viewMat * glm::vec4(-50.f, 10.f, 10.f, 1.0f);
    glm::vec4 ndc = pos2Cs / pos2Cs.w;

    glm::vec4 ndcRangeCoverted = (ndc + 1.0f) * .5f;
    glm::vec2 screenPos = glm::vec2(ndcRangeCoverted.x * m_renderWidth, ndcRangeCoverted.y * m_renderHeight);*/

    //glm::vec2 screenPos2(0, 0);
    //glm::vec2 screenPos2(1279, 0);
    //glm::vec2 screenPos2(1279, 719);
    //glm::vec4 screenPos2(189.5, 664.5, 0.99615, 1);
    //glm::vec4 screenPos2(189.5, 664.5, 0.0, 1);
    //glm::vec4 temp = glm::vec4(screenPos2.x, screenPos2.y, screenPos2.z, screenPos2.w);

    //temp = glm::vec4(screenPos2.x / m_renderWidth, screenPos2.y / m_renderHeight, temp.z, temp.w);
    //glm::vec4 clipPos = glm::vec4(glm::vec2(temp.x * 2.0 - 1.0, (1.0 - temp.y) * 2.0 - 1.0), (temp.z), temp.w);
    //glm::vec4 view = glm::inverse(m_renderData.m_cameraData[0].m_projectionMat) * clipPos;
    //view = (view / view.w);
    ////glm::vec4 worldPos(-15, 10, 4.22f, 1.0f);
    //glm::vec4 worldPos(264, 100, -200.0f, 1.0f);
    //glm::vec4 viewSpacePos = m_renderData.m_cameraData[0].m_viewMat * worldPos;

    {
        SceneInfo scene{};
        scene.m_camFar = m_renderData.m_cameraData[0].m_far;
        scene.m_camNear = m_renderData.m_cameraData[0].m_near;
        scene.m_projection = m_renderData.m_cameraData[0].m_projectionMat;
        scene.m_screenHeight = m_renderHeight;
        scene.m_screenWidth = m_renderWidth;
        scene.m_view = m_renderData.m_cameraData[0].m_viewMat;

        //upload data to buffers
        {
            auto sceneUniformSize = sizeof(SceneInfo);
            auto memoryAlignedSceneUniformSize = VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(sceneUniformSize);
            ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(m_sceneBuffer[frameInfo.m_frameInFlightIndex]->GetPhysicalResourceId(),
                sceneUniformSize, memoryAlignedSceneUniformSize, &scene, 0, false);
        }
    }
    {
        const uint32_t numLights = m_renderData.m_lightData.m_pointLights.size();
        LightInfo lightInfo{};
        lightInfo.m_numLights = numLights;
        for (uint32_t i = 0; i < numLights; i++)
        {
            lightInfo.m_lights[i].m_position = m_renderData.m_lightData.m_pointLights[i].m_position;
            lightInfo.m_lights[i].m_radius = m_renderData.m_lightData.m_pointLights[i].radius;
        }
        //upload data to buffers
        {
            auto lightUniformSize = sizeof(LightInfo);
            auto memoryAlignedLightUniformSize = VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(lightUniformSize);
            ShdrResMgrAlias::GetInstance()->UploadDataToBuffers(m_lightInfoBuffer[frameInfo.m_frameInFlightIndex]->GetPhysicalResourceId(),
                lightUniformSize, memoryAlignedLightUniformSize, &lightInfo, 0, false);
        }
    }

    if (!clusterBuilderTask->IsDataCached())
    {
        dispatchInfo.m_descriptorInfo.descriptorSetIds.push_back(m_clusterBuilderDescriptorSetIds[frameInfo.m_frameInFlightIndex]);
        dispatchInfo.m_descriptorInfo.dynamicOffsetCount = 0;
        dispatchInfo.m_descriptorInfo.firstSet = 0;
        dispatchInfo.m_descriptorInfo.numSetsToBind = 1;
        dispatchInfo.m_descriptorInfo.pDynamicOffsets = 0;
        dispatchInfo.m_descriptorInfo.pipelineBindPoint = Core::Enums::PipelineType::COMPUTE;

        ((Renderer::RenderGraph::Tasks::ComputeTask*)clusterBuilderTask)->UpdateDispatchInfo(dispatchInfo);
    }

    // light cull
    if (!lightCullerTask->IsDataCached())
    {
        dispatchInfo = {};
        dispatchInfo.m_descriptorInfo.descriptorSetIds.push_back(m_lightCullerDescriptorSetIds[frameInfo.m_frameInFlightIndex]);
        dispatchInfo.m_descriptorInfo.dynamicOffsetCount = 0;
        dispatchInfo.m_descriptorInfo.firstSet = 0;
        dispatchInfo.m_descriptorInfo.numSetsToBind = 1;
        dispatchInfo.m_descriptorInfo.pDynamicOffsets = 0;
        dispatchInfo.m_descriptorInfo.pipelineBindPoint = Core::Enums::PipelineType::COMPUTE;

        ((Renderer::RenderGraph::Tasks::ComputeTask*)lightCullerTask)->UpdateDispatchInfo(dispatchInfo);
    }
}


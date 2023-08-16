#pragma once
#include "RenderingWrapper.h"
#include "HashManager.h"
#include "PipelineStateWrappers.h"
#include <Graph.h>
#include <vector>
#include "DrawGraphNode.h"
#include "RendererSettings.h"

class PipelineDrawNode : public DrawGraphNode
{
private:

public:
    uint32_t pipelineLayoutId;
    uint32_t pipelineId;
    PipelineType pipelineBindPoint;

    virtual void Entry() override;
    virtual void Exit() override;
    PipelineDrawNode()
    {
        drawNodeType = DrawNodeTypes::PIPELINE;
    }
};

struct PipelineWrapper
{
    uint32_t id;
    std::vector<uint32_t> meshList;
};

template <typename T>
class GraphicsPipelineManager
{
private:
    GraphicsPipelineManager(){}
    GraphicsPipelineManager(GraphicsPipelineManager const &) {}
    GraphicsPipelineManager const & operator= (GraphicsPipelineManager const &) {}

    static GraphicsPipelineManager* instance;
    
    T * apiInterface;

    std::vector<VertexInputWrapper *> vertexInputStateWrapperList;
    std::vector<InputAssemblyWrapper *> vertexInputAssemblyWrapperList;
    std::vector<ShaderStateWrapper *> shaderStateWrapperList;
    std::vector<ShaderResourceStateWrapper *> shaderResourceStateWrapperList;
    std::vector<TessellationStateWrapper *> tessellationStateWrapperList;
    std::vector<ViewPortStateWrapper *> viewportStateWrapperList;
    std::vector<RasterizationStateWrapper *> rasterizationWrapperList;
    std::vector<MultiSampleStateWrapper *> multisampleStateWrapperList;
    std::vector<DepthStencilStateWrapper *> depthStencilStateWrapperList;
    std::vector<ColorBlendStateWrapper *> colorBlendStateWrapperList;
    std::vector<DynamicStateWrapper *> dynamicStateWrapperList;

    std::map<uint32_t, std::vector<GraphNode<StateWrapperBase>*>> meshToGraphNodeMap;
    void InsertToMeshList(const uint32_t & meshId, PipelineStates state, GraphNode<StateWrapperBase>* node);

    int maxGraphNodes = 30;
    Graph<StateWrapperBase> * pipelineGraph;// TODO: add the graph construction without predefined length
    std::vector<PipelineStates> pipelineStateList;
    std::map<PipelineStates, std::vector<GraphNode<StateWrapperBase> *>> stateToNodeMap; // nodes per pipeline state
    std::map<PipelineStates, int> stateToDefaultNodeIndex; // index into stateToNodeMap's vector
    GraphNode<StateWrapperBase> * CreateGraphNode(StateWrapperBase * stateWrapper);
    std::map<RenderPassTag, std::vector<uint32_t>> renderPassToShaderStageIdMap;

    // for a pipeline
    uint32_t renderPassID, subPassId;
    uint32_t pipelineIdCounter = 0;
    uint32_t GeneratePipelineId();
    
    std::vector<PipelineCreateInfo> pipelineCreateInfoList;
    std::vector<PipelineWrapper> pipelineList;
    std::vector<GraphNode<DrawGraphNode> * > pipelineDrawNodeList;

    //really heavy function
    void CreateGraphEdges();

    //connects the nodes containing the mesh id, if not connects it with default node
    void CreateGraphEdges(const uint32_t & meshId, PipelineStates src, PipelineStates dest); 

    // connects all the src state state node to the give dest node
    void CreateGraphEdges(GraphNode<StateWrapperBase> * destNode, PipelineStates src, PipelineStates dest); 

    // connects all destination state nodes to the src node
    void CreateGraphEdges(PipelineStates src, GraphNode<StateWrapperBase> * srcNode, PipelineStates dest);

    // connects all the src state state node to the give dest node
    void CreateGraphEdges(GraphNode<StateWrapperBase> * srcNode, GraphNode<StateWrapperBase> * destNode);
    
    GraphNode<StateWrapperBase> * GetNode(PipelineStates state, StateWrapperBase * stateObj);

    uint32_t defaultAssemblyListIndex;

    void CreateVertexAssemblyDefault();
    void CreateTessellationDefault();
    void CreateViewportDefault();
    void CreateRasterizationDefault();
    void CreateMultiSampleDefault();
    void CreateDepthStencilDefault();
    void CreateColorblendDefault();
    void CreateDynamicStateDefault();

    void DestroyVertexInputState();
    void DestroyShaderState();
    void DestroyShaderResourceState();
    void DestroyVertexAssembly();
    void DestroyTessellation();
    void DestroyViewport();
    void DestroyRasterization();
    void DestroyMultiSample();
    void DestroyDepthStencil();
    void DestroyColorblend();
    void DestroyDynamicState();

public:
    void Init(T * apiInterface);
    void DeInit();
    void Update();
    static GraphicsPipelineManager* GetInstance();
    ~GraphicsPipelineManager();

    void CreateVertexInputState(const uint32_t & meshId, VertexInputState * inputStateInfo );
    void CreateVertexAssemblyState(const uint32_t & meshId, InputAssemblyState * assembly);
    void CreateMultiSampleState(MultiSampleState * multiSample, const uint16_t & tagMask);
    void AssignDefaultState(const uint32_t & meshId, const PipelineStates & state);
    void CreatShaderPipelineState(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount, const RenderPassTag & tag = RenderPassTag::ColorPass);
    void CreatShaderPipelineState(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount, const uint16_t & tagMask);
    std::vector<SetWrapper*> CreatResourceLayoutState(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount);

    void TraversalEventHandler(GraphTraversalEvent * event);
    void GenerateAllPipelines(const uint32_t & renderPassId, const uint32_t & subpassId);
    void GenerateAllPipelines(std::vector<RenderingPassInfo> & passInfo);
};

template<typename T>
inline void GraphicsPipelineManager<T>::InsertToMeshList(const uint32_t & meshId, PipelineStates state, GraphNode<StateWrapperBase>* node)
{
    auto it = meshToGraphNodeMap.find(meshId);

    if (it == meshToGraphNodeMap.end())
        ASSERT_MSG_DEBUG(0, "mesh Id not found");

    it->second[(uint32_t)state] = node;
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateVertexAssemblyDefault()
{
    defaultAssemblyListIndex = (uint32_t)vertexInputAssemblyWrapperList.size();
    InputAssemblyWrapper * wrapper = new InputAssemblyWrapper;
    wrapper->assemblyState = new InputAssemblyState;
    wrapper->assemblyState->primitiveType = new PrimtiveType{ PrimtiveType::TOPOLOGY_TRIANGLE_LIST };
    wrapper->assemblyState->isPrimtiveRestartEnabled = false;

    HashManager::GetInstance()->FindInputAssemblyStateHash(wrapper->assemblyState, wrapper->GetId());

    vertexInputAssemblyWrapperList.push_back(wrapper);

    CreateGraphNode(wrapper);
    apiInterface->CreateInputAssemblyState(wrapper);
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateTessellationDefault()
{
    PipelineStates currentState = PipelineStates::TessellationState;

    TessellationStateWrapper * wrapper = new TessellationStateWrapper;
    wrapper->tessellationState = new TessellationState;
    wrapper->tessellationState->patchControlPoints = 0;
    tessellationStateWrapperList.push_back(wrapper);

    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindTessellationStateHash(wrapper->tessellationState, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    // WONT CREATE EDGE AS SHADER RESOURCE LAYOUT WONT BE CREATED BY DEFAULT
    //CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);

    apiInterface->CreateTessellationState(wrapper);
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateViewportDefault()
{
    PipelineStates currentState = PipelineStates::ViewportState;

    ViewPortStateWrapper * wrapper = new ViewPortStateWrapper;
    ViewportState * viewport = new ViewportState;
    viewport->pScissors = nullptr; // as viewport and scissor are dynamic states
    viewport->pViewports = nullptr;
    viewport->scissorCount = 1; // probably because its enabled as a dynamic state
    viewport->viewportCount = 1; // probably because its enabled as a dynamic state
    wrapper->viewportState = viewport;
    viewportStateWrapperList.push_back(wrapper);
    
    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindViewportStateHash(viewport, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);

    apiInterface->CreateViewportState(wrapper);

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateRasterizationDefault()
{
    PipelineStates currentState = PipelineStates::RasterizationState;
    
    RasterizationStateWrapper * wrapper = new RasterizationStateWrapper;
    RasterizationState * raster = new RasterizationState;
    raster->cullMode = CullMode::CULL_MODE_BACK_BIT;
    raster->depthBiasClamp = 0;
    raster->depthBiasConstantFactor = 0;
    raster->depthBiasEnable = false;
    raster->depthBiasSlopeFactor = 0;
    raster->depthClampEnable = false;
    raster->frontFace = FrontFace::FRONT_FACE_CLOCKWISE;
    raster->lineWidth = 1.0f;
    raster->polygonMode = PolygonMode::POLYGON_MODE_FILL;
    raster->rasterizerDiscardEnable = false;
    wrapper->rasterState = raster;
    rasterizationWrapperList.push_back(wrapper);

    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindRasterizationHash(raster, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");
    
    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
    apiInterface->CreateRasterisationState(wrapper);

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateMultiSampleDefault()
{
    PipelineStates currentState = PipelineStates::MultisampleState;

    MultiSampleStateWrapper * wrapper = new MultiSampleStateWrapper;
    MultiSampleState * multisample = new MultiSampleState;
    multisample->alphaToCoverageEnable = false;
    multisample->alphaToOneEnable = false;
    multisample->sampleCount = *RendererSettings::sampleCount;
    multisample->sampleShadingEnable = RendererSettings::sampleRateShadingEnabled;
    multisample->minSampleShading = 0.5f;
    wrapper->multiSampleState = multisample;
    wrapper->tagMask = (uint16_t)RenderPassTag::ColorPass;
    
    multisampleStateWrapperList.push_back(wrapper);
    
    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindMultiSampleHash(multisample, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
    
    apiInterface->CreateMultiSampleState(wrapper);
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateDepthStencilDefault()
{
    PipelineStates currentState = PipelineStates::DepthStencilState;

    DepthStencilStateWrapper * wrapper = new DepthStencilStateWrapper;
    DepthStencilState * depthStencil = new DepthStencilState;
    depthStencil->back.passOp = StencilOp::STENCIL_OP_KEEP;
    depthStencil->back.failOp = StencilOp::STENCIL_OP_KEEP;
    depthStencil->back.compareMask = 0;
    depthStencil->back.compareOp = CompareOp::COMPARE_OP_ALWAYS;
    depthStencil->back.reference = 0;
    depthStencil->back.writeMask = 0;
    depthStencil->back.depthFailOp = StencilOp::STENCIL_OP_KEEP;

    depthStencil->depthBoundsTestEnable = false;
    depthStencil->depthCompareOp = CompareOp::COMPARE_OP_LESS_OR_EQUAL;
    depthStencil->depthTestEnable = true;
    depthStencil->depthWriteEnable = true;
    depthStencil->front = depthStencil->back;
    depthStencil->maxDepthBounds = 0;
    depthStencil->minDepthBounds = 0;
    depthStencil->stencilTestEnable = false;

    wrapper->depthState = depthStencil;

    depthStencilStateWrapperList.push_back(wrapper);

    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindDepthStencilHash(depthStencil, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
    apiInterface->CreateDepthStencilState(wrapper);

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateColorblendDefault()
{
    PipelineStates currentState = PipelineStates::ColorBlendState;

    ColorBlendStateWrapper * wrapper = new ColorBlendStateWrapper;

    PipelineColorBlendAttachmentState * attachment = new PipelineColorBlendAttachmentState[1];
    attachment[0].alphaBlendOp = BlendOp::BLEND_OP_ADD;
    attachment[0].blendEnable = false;
    attachment[0].colorBlendOp = BlendOp::BLEND_OP_ADD;
    attachment[0].colorWriteMask = ColorComponentFlagBits::COLOR_COMPONENT_ALL_TRUE_BIT;
    attachment[0].dstAlphaBlendFactor = BlendFactor::BLEND_FACTOR_ZERO;
    attachment[0].dstColorBlendFactor = BlendFactor::BLEND_FACTOR_ZERO;
    attachment[0].srcAlphaBlendFactor = BlendFactor::BLEND_FACTOR_ZERO;
    attachment[0].srcColorBlendFactor = BlendFactor::BLEND_FACTOR_ZERO;

    ColorBlendState * colorBlend = new ColorBlendState;
    colorBlend->attachmentCount = 1;
    colorBlend->blendConstants[0] = 1.0f;
    colorBlend->blendConstants[1] = 1.0f;
    colorBlend->blendConstants[2] = 1.0f;
    colorBlend->blendConstants[3] = 1.0f;
    colorBlend->logicOp = LogicOp::LOGIC_OP_NO_OP;
    colorBlend->logicOpEnable = false;
    colorBlend->pAttachments = attachment;

    wrapper->colorBlendState = colorBlend;
    colorBlendStateWrapperList.push_back(wrapper);

    // Find hash so that the hash manager stores the id of this state.
    int test = HashManager::GetInstance()->FindColorBlendHash(colorBlend, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
    apiInterface->CreateColorBlendState(wrapper);

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateDynamicStateDefault()
{
    PipelineStates currentState = PipelineStates::DynamicState;

    DynamicStateWrapper * wrapper = new DynamicStateWrapper;
    DynamicStateList * dynamic = new DynamicStateList;
    DynamicState * states = new DynamicState[2];
    states[0] = DynamicState::DYNAMIC_STATE_VIEWPORT;
    states[1] = DynamicState::DYNAMIC_STATE_SCISSOR;
    dynamic->dynamicStateCount = 2;
    dynamic->pDynamicStates = states;
    wrapper->dynamicStates = dynamic;

    dynamicStateWrapperList.push_back(wrapper);
    // Find hash so that the hash manager stores the id of this state.

    int test = HashManager::GetInstance()->FindDynamicStateHash(dynamic, wrapper->GetId());
    ASSERT_MSG_DEBUG(test == -1, "Issue with hashing, should return -1");

    //Create graph node
    GraphNode<StateWrapperBase> * node = CreateGraphNode(wrapper);

    //Create edges
    CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
    apiInterface->CreateDynamicState(wrapper);
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyVertexInputState()
{
    for each(VertexInputWrapper * wrapper in vertexInputStateWrapperList)
    {
        if(wrapper->inputState->attribCount > 0)
            delete[] wrapper->inputState->attribInfo;
        
        if (wrapper->inputState->bindingCount > 0)
            delete[] wrapper->inputState->bindingInfo;

        delete wrapper->inputState;
        wrapper->meshIdList.clear();
        delete wrapper;
    }

    vertexInputStateWrapperList.clear();
    vertexInputStateWrapperList.resize(0);
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyShaderState()
{
    for each(ShaderStateWrapper * wrapper in shaderStateWrapperList)
    {
        //if (wrapper->shaderCount > 0)
            //delete[] wrapper->shader;  // getting deleted in shader factory

        wrapper->meshIdList.clear();
        delete wrapper;
    }

    shaderStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyShaderResourceState()
{
    for each(ShaderResourceStateWrapper * wrapper in shaderResourceStateWrapperList)
    {
        delete wrapper;
    }

    shaderResourceStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyVertexAssembly()
{
    for each(InputAssemblyWrapper * wrapper in vertexInputAssemblyWrapperList)
    {
        delete wrapper->assemblyState->primitiveType;
        delete wrapper->assemblyState;
        wrapper->meshIdList.clear();
        delete wrapper;
    }

    vertexInputAssemblyWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyTessellation()
{
    for each(TessellationStateWrapper * wrapper in tessellationStateWrapperList)
    {
        delete wrapper->tessellationState;
        delete wrapper;
    }
    tessellationStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyViewport()
{
    for each(ViewPortStateWrapper * wrapper in viewportStateWrapperList)
    {
        if (wrapper->viewportState->scissorCount > 0)
            delete wrapper->viewportState->pScissors;

        if (wrapper->viewportState->viewportCount > 0)
            delete wrapper->viewportState->pViewports;

        delete wrapper->viewportState;
        delete wrapper;
    }

    viewportStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyRasterization()
{
    for each(RasterizationStateWrapper * wrapper in rasterizationWrapperList)
    {
        delete wrapper->rasterState;
        delete wrapper;
    }

    rasterizationWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyMultiSample()
{
    for each(MultiSampleStateWrapper* wrapper in multisampleStateWrapperList)
    {
        delete wrapper->multiSampleState;
        delete wrapper;
    }

    multisampleStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyDepthStencil()
{
    for each(DepthStencilStateWrapper * wrapper in depthStencilStateWrapperList)
    {
        delete wrapper->depthState;
        delete wrapper;
    }

    depthStencilStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyColorblend()
{
    for each(ColorBlendStateWrapper * wrapper in colorBlendStateWrapperList)
    {
        if (wrapper->colorBlendState->attachmentCount > 0)
            delete[] wrapper->colorBlendState->pAttachments;

        delete wrapper->colorBlendState;
        delete wrapper;
    }

    colorBlendStateWrapperList.clear();
}

template<typename T>
inline void GraphicsPipelineManager<T>::DestroyDynamicState()
{
    for each(DynamicStateWrapper * wrapper in dynamicStateWrapperList)
    {
        if (wrapper->dynamicStates->dynamicStateCount > 0)
            delete[] wrapper->dynamicStates->pDynamicStates;
        delete wrapper->dynamicStates;
        delete wrapper;
    }
    dynamicStateWrapperList.clear();
}

template<typename T>
inline GraphNode<StateWrapperBase> * GraphicsPipelineManager<T>::CreateGraphNode(StateWrapperBase * stateWrapper)
{
    GraphNode<StateWrapperBase> * node = new GraphNode<StateWrapperBase>(stateWrapper);
    pipelineGraph->Push(node);

    //if entry present then insert a new node
    auto it = stateToNodeMap.find(stateWrapper->state);

    if (it != stateToNodeMap.end())
    {
        it->second.push_back(node);
    }
    else
    {
        stateToNodeMap.insert(std::pair< PipelineStates, std::vector<GraphNode<StateWrapperBase> *>>({
            stateWrapper->state, std::vector<GraphNode<StateWrapperBase> *>{node } }));
    }

    return node;
}

template<typename T>
inline uint32_t GraphicsPipelineManager<T>::GeneratePipelineId()
{
    return pipelineIdCounter ++;
}

//really heavy function, use the other one
template<typename T>
inline void GraphicsPipelineManager<T>::CreateGraphEdges()
{
    // iterate through state nodes (state0, state1 ...)
    PipelineStates src, dest;
    uint32_t stateCounter = 0;

    do
    {
        src = pipelineStateList[stateCounter];
        dest = pipelineStateList[stateCounter + 1];

        std::vector<GraphNode<StateWrapperBase>*> srcGraphNodeList = stateToNodeMap[src];
        std::vector<GraphNode<StateWrapperBase>*> destGraphNodeList = stateToNodeMap[dest];

        for each(GraphNode<StateWrapperBase> * srcNode in srcGraphNodeList)
        {
            for each(GraphNode<StateWrapperBase> * destNode in destGraphNodeList)
            {
                uint32_t size = (uint32_t)srcNode->node->meshIdList.size();
                std::vector<uint32_t> srchMeshIdList = srcNode->node->meshIdList;
                std::vector<uint32_t>::iterator it;// = destNode->node->meshIdList.begin();

                for (uint32_t i = 0; i < size; i++)
                {
                    it = std::find_if(destNode->node->meshIdList.begin(), destNode->node->meshIdList.end(), [&](uint32_t id) { return id == srchMeshIdList[i]; });
                    //ASSERT_MSG(it != destNode->node->meshIdList.end(), "Queue family index not found");

                    if (it != destNode->node->meshIdList.end())
                    {
                        pipelineGraph->AttachDirectedEdge(srcNode, destNode);
                        break;
                    }
                }
            }
        }

        stateCounter++;

    } while (dest != pipelineStateList.back());

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateGraphEdges(const uint32_t & meshId, PipelineStates src, PipelineStates dest)
{
    
    auto it = meshToGraphNodeMap.find(meshId);
    if (it == meshToGraphNodeMap.end())
    {
        ASSERT_MSG_DEBUG(0, "id not found");
    }

    GraphNode<StateWrapperBase>* srcNode = it->second[(uint32_t)src];
    if (srcNode == NULL)
    {
        //ASSERT_MSG(0, "src node not found");
        int id = stateToDefaultNodeIndex[src];
        ASSERT_MSG_DEBUG(id != -1, "default stage doesn't exist");
        srcNode = stateToNodeMap[src].at(id);
    }

    GraphNode<StateWrapperBase>* destNode = it->second[(uint32_t)dest];
    if (destNode == NULL)
    {
        int id = stateToDefaultNodeIndex[dest];
        ASSERT_MSG_DEBUG(id != -1, "default stage doesn't exist");
        destNode = stateToNodeMap[dest].at(id);
    }

    pipelineGraph->AttachDirectedEdge(srcNode, destNode);

}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateGraphEdges(GraphNode<StateWrapperBase>* destNode, PipelineStates src, PipelineStates dest)
{
    auto it = stateToNodeMap.find(src);
    if (it == stateToNodeMap.end())
    {
        ASSERT_MSG_DEBUG(0, "node not found");
    }

    // Get all the node of the source state and connect it to destNode
    // uint32_t numNodes = (uint32_t)stateToNodeMap[src].size();
    for each (auto var in stateToNodeMap[src])
    {
        pipelineGraph->AttachDirectedEdge(var, destNode);
    }
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateGraphEdges(PipelineStates src, GraphNode<StateWrapperBase>* srcNode, PipelineStates dest)
{
    auto it = stateToNodeMap.find(dest);
    if (it == stateToNodeMap.end())
    {
        ASSERT_MSG_DEBUG(0, "node not found");
    }

    // Get all the node of the destination state and connect it to srcNode
    for each (auto var in stateToNodeMap[dest])
    {
        pipelineGraph->AttachDirectedEdge(srcNode, var);
    }
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateGraphEdges(GraphNode<StateWrapperBase>* srcNode, GraphNode<StateWrapperBase>* destNode)
{
    if (srcNode->GetNodeData()->state == destNode->GetNodeData()->state)
    {
        ASSERT_MSG_DEBUG(0, "Circular graph");
    }

    pipelineGraph->AttachDirectedEdge(srcNode, destNode);
}

template<typename T>
inline GraphNode<StateWrapperBase>* GraphicsPipelineManager<T>::GetNode(PipelineStates state, StateWrapperBase * stateObj)
{
    auto itt = stateToNodeMap.find(state);
    if (itt == stateToNodeMap.end())
    {
        ASSERT_MSG_DEBUG(0, "state not found");
    }

    std::vector<GraphNode<StateWrapperBase>*> nodeList = itt->second;

    std::vector<GraphNode<StateWrapperBase>*>::iterator it;
    it = std::find_if(nodeList.begin(), nodeList.end(), [&](GraphNode<StateWrapperBase>* e) {
        return (e->GetNodeData()->state == state) && (e->GetNodeData()->GetId() == stateObj->GetId()); });

    ASSERT_MSG_DEBUG(it != nodeList.end(), "Node not found");
    return (*it);
}

template<typename T>
inline void GraphicsPipelineManager<T>::Init(T * apiInterface)
{
    this->apiInterface = apiInterface;

    pipelineGraph = new Graph<StateWrapperBase>(maxGraphNodes);

    pipelineStateList.push_back(PipelineStates::VertexInputState);
    pipelineStateList.push_back(PipelineStates::InputAssemblyState);
    pipelineStateList.push_back(PipelineStates::ShaderStage);
    pipelineStateList.push_back(PipelineStates::TessellationState);
    pipelineStateList.push_back(PipelineStates::ViewportState);
    pipelineStateList.push_back(PipelineStates::RasterizationState);
    pipelineStateList.push_back(PipelineStates::MultisampleState);
    pipelineStateList.push_back(PipelineStates::DepthStencilState);
    pipelineStateList.push_back(PipelineStates::ColorBlendState);
    pipelineStateList.push_back(PipelineStates::DynamicState);
    
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::VertexInputState , -1 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::InputAssemblyState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::ShaderStage , -1 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::ShaderResourcesLayout , -1 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::TessellationState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::ViewportState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::RasterizationState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::MultisampleState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::DepthStencilState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::ColorBlendState , 0 }));
    stateToDefaultNodeIndex.insert(std::pair<PipelineStates, int>({ PipelineStates::DynamicState , 0}));

    CreateVertexAssemblyDefault();
    CreateTessellationDefault();
    CreateViewportDefault();
    CreateRasterizationDefault();
    CreateMultiSampleDefault();
    CreateDepthStencilDefault();
    CreateColorblendDefault();
    CreateDynamicStateDefault();

    EventBus::GetInstance()->Subscribe<GraphicsPipelineManager<T>, GraphTraversalEvent>(this, &GraphicsPipelineManager<T>::TraversalEventHandler);
}

template<typename T>
inline void GraphicsPipelineManager<T>::DeInit()
{
    DestroyVertexInputState();
    DestroyShaderState();
    DestroyShaderResourceState();
    DestroyVertexAssembly();
    DestroyTessellation();
    DestroyViewport();
    DestroyRasterization();
    DestroyMultiSample();
    DestroyDepthStencil();
    DestroyColorblend();
    DestroyDynamicState();

    std::map < PipelineStates, std::vector<GraphNode<StateWrapperBase> *>>::iterator it;
    for (it = stateToNodeMap.begin(); it != stateToNodeMap.end(); it++)
    {
        for each (GraphNode<StateWrapperBase> * node in it->second)
        {
            delete node;
        }

        it->second.clear();
    }

    stateToNodeMap.clear();
    pipelineStateList.clear();
    meshToGraphNodeMap.clear();
    delete pipelineGraph;

    for each(auto obj in pipelineDrawNodeList)
    {
        delete obj->node;
        delete obj;
    }
}

template<typename T>
inline void GraphicsPipelineManager<T>::Update()
{
}

template<typename T>
inline GraphicsPipelineManager<T>::~GraphicsPipelineManager()
{
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateVertexInputState(const uint32_t & meshId, VertexInputState * inputStateInfo)
{
    VertexInputWrapper* wrapper =  new VertexInputWrapper;
    wrapper->inputState = inputStateInfo;

    int id = HashManager::GetInstance()->FindVertexInputStateHash(inputStateInfo, wrapper->GetId());
    
    // vertex input will be the first state hence the vector can be resized
    std::vector<GraphNode<StateWrapperBase>*> nodeList;
    nodeList.resize((size_t)PipelineStates::NumStates);
    GraphNode<StateWrapperBase> * node;
    if (id == -1)
    {
        // add wrapper to list
        wrapper->meshIdList.push_back(meshId);
        vertexInputStateWrapperList.push_back(wrapper);

        // create a new pipeline node encapsulate in graph node, add it to graph
        // TODO : Create graph node, push to graph, make the connections
        node = CreateGraphNode(wrapper);
    
        // TODO : as this is vertex input state the connection won't be made here, Done.

        // TODO : Create vulkan vertexinput object, done.

        apiInterface->CreateVertexInputState(wrapper);
    }
    else
    {
        // reduce meshid counter by 1
        DecrementIdCounter<VertexInputWrapper>();
        // delete input info
        delete inputStateInfo;
        // add the mesh id to the existing wrapper obj in the list
        std::vector<VertexInputWrapper*>::iterator it;
        it = std::find_if(vertexInputStateWrapperList.begin(), vertexInputStateWrapperList.end(), [&](VertexInputWrapper * e) { return e->GetId() == id; });

        ASSERT_MSG_DEBUG(it != vertexInputStateWrapperList.end(), "wrapper not found");
        (*it)->meshIdList.push_back(meshId);
        node = GetNode((*it)->state, (*it));
    //    nodeList[(uint32_t)PipelineStates::VertexInputState] = GetNode((*it)->state, (*it));
    }

    nodeList[(uint32_t)PipelineStates::VertexInputState] = node;
    meshToGraphNodeMap.insert(std::pair < uint32_t, std::vector<GraphNode<StateWrapperBase>*>>({
        meshId, nodeList }));
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateVertexAssemblyState(const uint32_t & meshId, InputAssemblyState * assembly)
{
    InputAssemblyWrapper * wrapper = new InputAssemblyWrapper;
    wrapper->assemblyState = assembly;

    int id = HashManager::GetInstance()->FindInputAssemblyStateHash(assembly, wrapper->GetId());
    GraphNode<StateWrapperBase> * node;

    if (id == -1)
    {
        // add wrapper to list
        wrapper->meshIdList.push_back(meshId);
        vertexInputAssemblyWrapperList.push_back(wrapper);

        // create a new pipeline node encapsulate in graph node, add it to graph
        // TODO : Create pipeline node, done.
        node = CreateGraphNode(wrapper);

        // TODO : Create vulkan inputAssembly object, done.
        apiInterface->CreateInputAssemblyState(wrapper);
    }
    else
    {
        // reduce meshid counter by 1
        DecrementIdCounter<InputAssemblyWrapper>();

        // delete input info
        delete assembly->primitiveType;
        delete assembly;
        delete wrapper;

        // add the mesh id to the existing wrapper obj in the list
        std::vector<InputAssemblyWrapper*>::iterator it;
        it = std::find_if(vertexInputAssemblyWrapperList.begin(), vertexInputAssemblyWrapperList.end(), [&](InputAssemblyWrapper * e) { return e->GetId() == id; });

        ASSERT_MSG_DEBUG(it != vertexInputAssemblyWrapperList.end(), "wrapper not found");

        (*it)->meshIdList.push_back(meshId);

        node = GetNode((*it)->state, (*it));
    }

    InsertToMeshList(meshId, PipelineStates::InputAssemblyState, node);
    CreateGraphEdges(meshId, PipelineStates::VertexInputState, PipelineStates::InputAssemblyState);
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreateMultiSampleState(MultiSampleState * multiSample, const uint16_t & tagMask)
{
    PipelineStates currentState = PipelineStates::MultisampleState;

    MultiSampleStateWrapper * wrapper = new MultiSampleStateWrapper;
    wrapper->multiSampleState = multiSample;
    wrapper->tagMask = tagMask;

    int id = HashManager::GetInstance()->FindMultiSampleHash(multiSample, wrapper->GetId());
    GraphNode<StateWrapperBase> * node;

    if (id == -1)
    {
        multisampleStateWrapperList.push_back(wrapper);

        // create a new pipeline node encapsulate in graph node, add it to graph
        // TODO : Create pipeline node, done.
        node = CreateGraphNode(wrapper);

        // TODO : Create vulkan inputAssembly object, done.
        apiInterface->CreateMultiSampleState(wrapper);

        CreateGraphEdges(node, PipelineStates((uint32_t)currentState - 1), currentState);
        CreateGraphEdges(currentState, node, PipelineStates((uint32_t)currentState + 1));

    }
    else
    {
        // reduce meshid counter by 1
        DecrementIdCounter<MultiSampleStateWrapper>();

        // delete input info
        delete multiSample;
        delete wrapper;

        // this wont happen as meshId is not provided
        // add the mesh id to the existing wrapper obj in the list
        /*std::vector<MultiSampleStateWrapper*>::iterator it;
        it = std::find_if(multisampleStateWrapperList.begin(), multisampleStateWrapperList.end(), [&](MultiSampleStateWrapper * e) { return e->GetId() == id; });

        ASSERT_MSG_DEBUG(it != multisampleStateWrapperList.end(), "wrapper not found");

        (*it)->meshIdList.push_back(meshId);
        node = GetNode((*it)->state, (*it));*/
    }
    
}

template<typename T>
inline void GraphicsPipelineManager<T>::AssignDefaultState(const uint32_t & meshId, const PipelineStates & state)
{
    switch (state)
    {
    case PipelineStates::InputAssemblyState :
        vertexInputAssemblyWrapperList[defaultAssemblyListIndex]->meshIdList.push_back(meshId);
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Invalid state");
    }
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreatShaderPipelineState(const uint32_t & meshId, Shader * shaders, 
    const uint32_t & shaderCount, const RenderPassTag & tag)
{
    ShaderStateWrapper * wrapper = new ShaderStateWrapper;
    wrapper->shader = shaders;
    wrapper->shaderCount = shaderCount;
    wrapper->tag = tag;
    
    // check if the hash exist for the above object
    int id = HashManager::GetInstance()->FindShaderStateHash(shaders, shaderCount, wrapper->GetId(), &wrapper->state);
    GraphNode<StateWrapperBase> * node;

    // if not add the wrapper to the list
    if (id == -1)
    {
        // add wrapper to list
        wrapper->meshIdList.push_back(meshId);
        shaderStateWrapperList.push_back(wrapper);
        // create a new pipeline node encapsulate in graph node, add it to graph
        // TODO : Create pipeline node , done.
        node = CreateGraphNode(wrapper);

        // TODO : Create vulkan pipeline shader stage object 
        apiInterface->CreateShaderState(wrapper);

        auto it = renderPassToShaderStageIdMap.find(tag);

        if (it != renderPassToShaderStageIdMap.end())
        {
            it->second.push_back(wrapper->GetId());
        }
        else
        {
            renderPassToShaderStageIdMap.insert(std::pair< RenderPassTag, std::vector<uint32_t>>({
                tag, std::vector<uint32_t>{ wrapper->GetId()} }));
        }
    }
    else
    {
        // if it exists push the mesh id to the list of the object
        // reduce meshid counter by 1
        DecrementIdCounter<ShaderStateWrapper>();
        
        // delete input info
        delete wrapper;

        // add the mesh id to the existing wrapper obj in the list
        std::vector<ShaderStateWrapper*>::iterator it;
        it = std::find_if(shaderStateWrapperList.begin(), shaderStateWrapperList.end(), [&](ShaderStateWrapper* e) { return e->GetId() == id; });
        ASSERT_MSG_DEBUG(it != shaderStateWrapperList.end(), "wrapper not found");
        (*it)->meshIdList.push_back(meshId);
        
        node = GetNode((*it)->state, (*it));

    }

    InsertToMeshList(meshId, PipelineStates::ShaderStage, node);
    CreateGraphEdges(meshId, PipelineStates::InputAssemblyState, PipelineStates::ShaderStage);

    //pipelineGraph->PrintAdjMatrix();
}

template<typename T>
inline void GraphicsPipelineManager<T>::CreatShaderPipelineState(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount, const uint16_t & tagMask)
{
    ShaderStateWrapper * wrapper = new ShaderStateWrapper;
    wrapper->shader = shaders;
    wrapper->shaderCount = shaderCount;
    //wrapper->tag = tag;
    wrapper->tagMask = tagMask;

    // check if the hash exist for the above object
    int id = HashManager::GetInstance()->FindShaderStateHash(shaders, shaderCount, wrapper->GetId(), &wrapper->state);
    GraphNode<StateWrapperBase> * node;

    // if not add the wrapper to the list
    if (id == -1)
    {
        // add wrapper to list
        wrapper->meshIdList.push_back(meshId);
        shaderStateWrapperList.push_back(wrapper);
        // create a new pipeline node encapsulate in graph node, add it to graph
        // TODO : Create pipeline node , done.
        node = CreateGraphNode(wrapper);

        // Create vulkan pipeline shader stage object 
        apiInterface->CreateShaderState(wrapper);

        /*auto it = renderPassToShaderStageIdMap.find(tag);

        if (it != renderPassToShaderStageIdMap.end())
        {
            it->second.push_back(wrapper->GetId());
        }
        else
        {
            renderPassToShaderStageIdMap.insert(std::pair< RenderPassTag, std::vector<uint32_t>>({
                tag, std::vector<uint32_t>{ wrapper->GetId()} }));
        }*/
    }
    else
    {
        // if it exists push the mesh id to the list of the object
        // reduce meshid counter by 1
        DecrementIdCounter<ShaderStateWrapper>();

        // delete input info
        delete wrapper;

        // add the mesh id to the existing wrapper obj in the list
        std::vector<ShaderStateWrapper*>::iterator it;
        it = std::find_if(shaderStateWrapperList.begin(), shaderStateWrapperList.end(), [&](ShaderStateWrapper* e) { return e->GetId() == id; });
        ASSERT_MSG_DEBUG(it != shaderStateWrapperList.end(), "wrapper not found");
        (*it)->meshIdList.push_back(meshId);

        node = GetNode((*it)->state, (*it));

    }

    InsertToMeshList(meshId, PipelineStates::ShaderStage, node);
    CreateGraphEdges(meshId, PipelineStates::InputAssemblyState, PipelineStates::ShaderStage);

    //pipelineGraph->PrintAdjMatrix();
}

template<typename T>
inline std::vector<SetWrapper*> GraphicsPipelineManager<T>::CreatResourceLayoutState(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount)
{
    ShaderResourceStateWrapper * wrapper = new ShaderResourceStateWrapper;
    wrapper->shader = shaders;
    wrapper->shaderCount = shaderCount;
    
    std::vector<std::string> shaderNames;
    for (uint32_t i = 0; i < shaderCount; i++)
        shaderNames.push_back(shaders[i].shaderName);

    // get the setwrapper list for the shader combination
    wrapper->resourcesSetList = apiInterface->GetSetsForShaders(shaderNames);

    // check if the hash exist for the above object
    int id = HashManager::GetInstance()->FindResourceLayoutHash(wrapper->resourcesSetList.data(), 
        (uint32_t)wrapper->resourcesSetList.size(), wrapper->GetId());
    GraphNode<StateWrapperBase> * node;

    std::vector<SetWrapper*> setWrapperList;

    // if not add the wrapper to the list
    if (id == -1)
    {
        // add wrapper to list 
        std::vector<uint32_t>::iterator it;
        it = std::find_if(wrapper->meshIdList.begin(), wrapper->meshIdList.end(), [&](uint32_t id) { return id == meshId; });
        if(it == wrapper->meshIdList.end())
            wrapper->meshIdList.push_back(meshId);

        shaderResourceStateWrapperList.push_back(wrapper);
        // create a new pipeline node encapsulate in graph node, add it to graph
        node = CreateGraphNode(wrapper);

        //Create vulkan pipeline pipeline state object
        wrapper->pipelineLayoutId = apiInterface->CreatePipelineLayout(wrapper->resourcesSetList.data(),
            wrapper->resourcesSetList.size());
        setWrapperList = wrapper->resourcesSetList;
    }
    else
    {
        // if it exists push the mesh id to the list of the object
        // reduce meshid counter by 1
        DecrementIdCounter<ShaderResourceStateWrapper>();

        // delete input info
        delete wrapper;

        // add the mesh id to the existing wrapper obj in the list
        std::vector<ShaderResourceStateWrapper*>::iterator it;
        it = std::find_if(shaderResourceStateWrapperList.begin(), shaderResourceStateWrapperList.end(), [&](ShaderResourceStateWrapper* e) { return e->GetId() == id; });
        ASSERT_MSG_DEBUG(it != shaderResourceStateWrapperList.end(), "wrapper not found");
        
        // add wrapper to list 
        std::vector<uint32_t>::iterator itt;
        itt = std::find_if((*it)->meshIdList.begin(), (*it)->meshIdList.end(), [&](uint32_t id) { return id == meshId; });
        if (itt == (*it)->meshIdList.end())
            (*it)->meshIdList.push_back(meshId);

        node = GetNode((*it)->state, (*it));
        setWrapperList = (*it)->resourcesSetList;
    }

    InsertToMeshList(meshId, PipelineStates::ShaderResourcesLayout, node);

    // edge from a higher level
    CreateGraphEdges(meshId, PipelineStates::ShaderStage, PipelineStates::ShaderResourcesLayout);
    
    // edge to a lower level
    CreateGraphEdges(meshId, PipelineStates::ShaderResourcesLayout, PipelineStates::TessellationState);

    return setWrapperList;
}

template<typename T>
inline void GraphicsPipelineManager<T>::TraversalEventHandler(GraphTraversalEvent * event)
{
    //pipelineGraph->PrintAdjMatrix();

    uint16_t tag = PipelineUtil::globalTagMask;
    bool valid = PipelineUtil::tagValidity;
    if (PipelineUtil::pipelineStateMeshList.size() == 0 || PipelineUtil::tagValidity == false)
    {
        PipelineUtil::setsPerPipeline.clear();
        PipelineUtil::pipelineStateMeshList.clear();
        PipelineUtil::stateToIdMap.clear();
        PipelineUtil::globalTagMask = 0;
        PipelineUtil::tagValidity = true;
        return;
    }

    // pipeline create info
    PipelineCreateInfo info = {};
    //info.renderPassId = renderPassID;
    //info.subpassId = subPassId;
    info.statesToIdMap = PipelineUtil::stateToIdMap;
    info.meshList = PipelineUtil::pipelineStateMeshList;
    info.setsPerPipeline = PipelineUtil::setsPerPipeline;
    info.pipelineLayoutId = PipelineUtil::pipelineLayoutId;
    info.tagMask = PipelineUtil::globalTagMask;
    pipelineCreateInfoList.push_back(info);
    
    PipelineUtil::setsPerPipeline.clear();
    PipelineUtil::pipelineStateMeshList.clear();
    PipelineUtil::stateToIdMap.clear();
    PipelineUtil::globalTagMask = 0;
    PipelineUtil::tagValidity = true;
}

template<typename T>
inline void GraphicsPipelineManager<T>::GenerateAllPipelines(std::vector<RenderingPassInfo> & passInfo)
{
    // traversal
    //this->renderPassID = renderPassId;
    //this->subPassId = subpassId;

    // Get all the source states
    std::vector<GraphNode<StateWrapperBase> * > sourceNodeList = stateToNodeMap[PipelineStates::VertexInputState];

    // Get all the dest states
    std::vector<GraphNode<StateWrapperBase> * > destNodeList = stateToNodeMap[PipelineStates::DynamicState];

    // Execute Traversal, the above function TraversalEventHandler, gets triggered 
    // for every path traversal.
    for each(auto srcNode in sourceNodeList)
        for each (auto destNode in destNodeList)
        {
            pipelineGraph->FindAllPaths(srcNode->GetNodeId(), destNode->GetNodeId());
            // traversal event getting handled in TraversalEventHandler
        }

    // TraversalEventHandler gets triggered first

    uint32_t numPipelines = (uint32_t)pipelineCreateInfoList.size();
    uint32_t * ids = new uint32_t[numPipelines];

    for (uint32_t i = 0; i < numPipelines; i++)
    {
        // Generate pipeline
        PipelineWrapper wrapper = {};
        wrapper.id = GeneratePipelineId();
        wrapper.meshList = pipelineCreateInfoList[i].meshList;
        pipelineList.push_back(wrapper);

        ids[i] = wrapper.id;

        DrawGraphNode * pipelineNode = new PipelineDrawNode;
        pipelineNode->setWrapperList = pipelineCreateInfoList[i].setsPerPipeline;
        pipelineNode->meshList = pipelineCreateInfoList[i].meshList;
        ((PipelineDrawNode*)pipelineNode)->pipelineLayoutId = pipelineCreateInfoList[i].pipelineLayoutId;
        ((PipelineDrawNode*)pipelineNode)->pipelineId = wrapper.id;

        // TODO : find better design to tag and find pipeline node
        /*{
            uint32_t shaderStateId = pipelineCreateInfoList[i].statesToIdMap[PipelineStates::ShaderStage];
            
            for (auto info : passInfo)
            {
                std::vector<uint32_t> idList = renderPassToShaderStageIdMap[info.passTag];

                std::vector<uint32_t>::iterator it;
                it = std::find_if(idList.begin(), idList.end(), [&](uint32_t id) { return shaderStateId == id; });
                if (it != idList.end())
                {
                    pipelineNode->tag = info.passTag;
                    pipelineCreateInfoList[i].renderPassId = info.renderPassId;
                    pipelineCreateInfoList[i].subpassId = info.subpassId;
                    break;
                }
            }
        }*/

        {
            bool passTagMatch = false;

            for (auto info : passInfo)
            {
                // need to remove info.tag and tag should get derived from mask
                if (info.tagMask == pipelineCreateInfoList[i].tagMask)
                {
                    passTagMatch = true;
                    pipelineNode->tag = info.passTag;
                    pipelineCreateInfoList[i].renderPassId = info.renderPassId;
                    pipelineCreateInfoList[i].subpassId = info.subpassId;
                    break;
                }
            }

            ASSERT_MSG(passTagMatch == true, "Pass tag mismatch");
        }

        GraphNode<DrawGraphNode> * pipelinGraphNode = new GraphNode<DrawGraphNode>(pipelineNode);
        DrawGraphManager::GetInstance()->AddNode(pipelinGraphNode);
        pipelineDrawNodeList.push_back(pipelinGraphNode);
    }

    apiInterface->CreatePipeline(&pipelineCreateInfoList[0], numPipelines, ids);

    delete[] ids;
}

template<typename T>
inline void GraphicsPipelineManager<T>::GenerateAllPipelines(const uint32_t & renderPassId, const uint32_t & subpassId)
{
    // traversal
    this->renderPassID = renderPassId;
    this->subPassId = subpassId;

    // Get all the source states
    std::vector<GraphNode<StateWrapperBase> * > sourceNodeList = stateToNodeMap[PipelineStates::VertexInputState];
    
    // Get all the dest states
    std::vector<GraphNode<StateWrapperBase> * > destNodeList = stateToNodeMap[PipelineStates::DynamicState];

    // Execute Traversal, the above function TraversalEventHandler, gets triggered 
    // for every path traversal.
    for each(auto srcNode in sourceNodeList)
        for each (auto destNode in destNodeList)
        {
            pipelineGraph->FindAllPaths(srcNode->GetNodeId(), destNode->GetNodeId());
            // traversal event getting handled in TraversalEventHandler
        }

    // TraversalEventHandler gets triggered first

    uint32_t numPipelines = (uint32_t)pipelineCreateInfoList.size();
    uint32_t * ids = new uint32_t[numPipelines];

    for (uint32_t i = 0; i < numPipelines; i++)
    {
        // Generate pipeline
        PipelineWrapper wrapper = {};
        wrapper.id = GeneratePipelineId();
        wrapper.meshList = pipelineCreateInfoList[i].meshList;
        pipelineList.push_back(wrapper);

        ids[i] = wrapper.id;

        DrawGraphNode * pipelineNode = new PipelineDrawNode;
        pipelineNode->setWrapperList = pipelineCreateInfoList[i].setsPerPipeline;
        pipelineNode->meshList = pipelineCreateInfoList[i].meshList;
        ((PipelineDrawNode*)pipelineNode)->pipelineLayoutId = pipelineCreateInfoList[i].pipelineLayoutId;
        ((PipelineDrawNode*)pipelineNode)->pipelineId = wrapper.id;

        // TODO : find better design to tag and find pipeline node
        {
            uint32_t shaderStateId = pipelineCreateInfoList[i].statesToIdMap[PipelineStates::ShaderStage];
            std::vector<uint32_t> idList = renderPassToShaderStageIdMap[RenderPassTag::DepthPass];

            std::vector<uint32_t>::iterator it;
            it = std::find_if(idList.begin(), idList.end(), [&](uint32_t id) { return shaderStateId == id; });
            if (it != idList.end())
            {
                pipelineNode->tag = RenderPassTag::DepthPass;
            }
            else
            {
                pipelineNode->tag = RenderPassTag::ColorPass;
            }
        }

        GraphNode<DrawGraphNode> * pipelinGraphNode = new GraphNode<DrawGraphNode>(pipelineNode);
        DrawGraphManager::GetInstance()->AddNode(pipelinGraphNode);
        pipelineDrawNodeList.push_back(pipelinGraphNode);
    }

    apiInterface->CreatePipeline(&pipelineCreateInfoList[0], numPipelines, ids);

    delete[] ids;
}





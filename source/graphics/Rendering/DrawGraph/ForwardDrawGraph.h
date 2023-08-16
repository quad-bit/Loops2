#pragma once
#include "DrawGraphNode.h"
#include "Graph.h"

enum ResourceSets
{
    CAMERA = 0,
    PIPELINE = 1,
    LIGHT = 2,
    SURFACE = 3,
    TRANSFORM = 4
};

struct PerPipelineNodeData
{
    GraphNode<DrawGraphNode> * pipelineNode;
    std::vector<uint32_t> setValues;
    std::map<DrawNodeTypes, std::vector<GraphNode<DrawGraphNode> *>> typeToNodeMap;
    std::map<uint32_t, std::vector<GraphNode<DrawGraphNode> *>> setToNodesMap;
};

struct NodeSetPerPass
{
    RenderPassTag tag;
    std::vector<GraphNode<DrawGraphNode> *> pipelineNodeList;
    std::map<DrawNodeTypes, std::vector<GraphNode<DrawGraphNode> *>> typeToNodeMap;
    std::map<uint32_t, std::vector<GraphNode<DrawGraphNode> *>> setToNodesMap;
    std::vector<PerPipelineNodeData> pipelineNodeDataList;
};

template <typename T>
class ForwardGraph
{
private:
    Graph<DrawGraphNode> * drawGraph;
    /*std::vector<GraphNode<DrawGraphNode> *> pipelineNodeList;
    std::map<DrawNodeTypes, std::vector<GraphNode<DrawGraphNode> *>> typeToNodeMap;
    std::map<uint32_t, std::vector<GraphNode<DrawGraphNode> *>> setToNodesMap;
    std::vector<PerPipelineNodeData> pipelineNodeDataList;
*/
    std::map<RenderPassTag, NodeSetPerPass * > nodeDataPerPassMap;
    void CreateEdgeForPipelineNode(GraphNode<DrawGraphNode> * pipelineNode, PerPipelineNodeData * data, NodeSetPerPass * dataSetPerPass);
    void CreateEdge(GraphNode<DrawGraphNode> * newNode);
    PerPipelineNodeData * CreatePipelineData(GraphNode<DrawGraphNode> * pipelineNode, std::vector<PerPipelineNodeData> & dataList);
    bool CheckIfNodeBelongsToPipeline(GraphNode<DrawGraphNode> * pipelineNode, 
        GraphNode<DrawGraphNode> * node, std::vector<uint32_t> * pipelineNodeSetList);
    void AddNodeToPipelineData(PerPipelineNodeData * data, GraphNode<DrawGraphNode> * node);
    void CreateEdgesWithinPipeline(PerPipelineNodeData * data);

    bool CheckForCommonMeshId(GraphNode<DrawGraphNode> * src, GraphNode<DrawGraphNode> * dest);
    DrawGraphNode * dummyNode;
    GraphNode<DrawGraphNode> * dummyGraphNode;
    
    void CreateConnections(GraphNode<DrawGraphNode> * node, NodeSetPerPass * dataSetPerPass);

public:
    void Init(Graph<DrawGraphNode> * drawGraph);
    void DeInit();
    void Update(DrawCommandBuffer<T> * dcb, const RenderPassTag & tag);
    ~ForwardGraph();

    void AddNode(GraphNode<DrawGraphNode> * node);
};

template<typename T>
inline void ForwardGraph<T>::CreateEdgeForPipelineNode(GraphNode<DrawGraphNode> * pipelineNode, PerPipelineNodeData * data, NodeSetPerPass * dataSetPerPass)
{
    // for now camera is the first node in the graph as it is getting shared by each and every object.
    // Camera = 0
    // pipeline = 1
    // ...

    // Incoming node to pipeline node
    {
        // iterate through all the non pipeline nodes with set = 0
        std::vector<GraphNode<DrawGraphNode>*> * nodeList = &dataSetPerPass->setToNodesMap[ResourceSets::CAMERA];

        // find match for mesh id among the nodes
        for each(auto obj in *nodeList)
        {
            bool isNodeInPipeline = CheckIfNodeBelongsToPipeline(pipelineNode, obj, &data->setValues);

            if (isNodeInPipeline)
            {
                // if yes, add it to pipeline node data 
                AddNodeToPipelineData(data, obj);
                
                // if yes create an incoming edge.
                drawGraph->AttachDirectedEdge(obj, pipelineNode);
            }
        }
    }// Incoming node to pipeline node

    // out going edges from pipeline node
    {
        // iterate through all the non pipeline nodes with set present in pipeline node's setlist
        uint32_t size = (uint32_t)data->setValues.size();
        for(uint32_t i = 0; i < size; i++)
        {
            auto setValue = data->setValues[i];
            if (setValue != ResourceSets::CAMERA && setValue != ResourceSets::TRANSFORM)// camera already connected
            {
                std::vector<GraphNode<DrawGraphNode>*> * nodeList = &dataSetPerPass->setToNodesMap[setValue];
                for each(auto obj in *nodeList)
                {
                    bool isNodeInPipeline = CheckIfNodeBelongsToPipeline(pipelineNode, obj, &data->setValues);
                    if (isNodeInPipeline)
                    {
                        // if yes, add it to pipeline node data 
                        AddNodeToPipelineData(data, obj);

                        // if yes create an outgoing edge from pipeline node.
                        if(i == 1)
                            drawGraph->AttachDirectedEdge(pipelineNode, obj);
                    }
                }
            }
            // outgoing edge, special case
            // pipeline = 1
            // set 4 = 2 (in list) , TRANFORM
            else if (setValue == ResourceSets::TRANSFORM)
            {
                // connect to the mesh node
                std::vector<GraphNode<DrawGraphNode>*> * nodeList = &dataSetPerPass->typeToNodeMap[DrawNodeTypes::MESH];
                for each(auto obj in *nodeList)
                {
                    bool isNodeInPipeline = CheckIfNodeBelongsToPipeline(pipelineNode, obj, &data->setValues);
                    if (isNodeInPipeline)
                    {
                        // if yes, add it to pipeline node data 
                        AddNodeToPipelineData(data, obj);

                        // if yes create an outgoing edge from pipeline node.
                        if(i == 1)
                            drawGraph->AttachDirectedEdge(pipelineNode, obj);
                    }
                }

                std::vector<GraphNode<DrawGraphNode>*> * drawingNodeList = &dataSetPerPass->typeToNodeMap[DrawNodeTypes::DRAWING];
                for each(auto obj in *drawingNodeList)
                {
                    bool isNodeInPipeline = CheckIfNodeBelongsToPipeline(pipelineNode, obj, &data->setValues);
                    if (isNodeInPipeline)
                    {
                        // if yes, add it to pipeline node data 
                        AddNodeToPipelineData(data, obj);
                    }
                }
            }
        }

    }// out going edges from pipeline node

    // connect the remaining nodes.
    {
        CreateEdgesWithinPipeline(data);
    }
}

template<typename T>
inline void ForwardGraph<T>::CreateEdge(GraphNode<DrawGraphNode>* newNode)
{
    /*
    //create edges from all pipeline nodes
    for each(auto obj in pipelineNodeDataList)
    {
        // check if the setwrapper of the newNode is present in pipeline's setwrapper list
        std::vector<uint32_t>::iterator it;
        it = std::find_if(obj.setValues.begin(), obj.setValues.end(), [&](uint32_t e) {
            return e == newNode->node->setLevel; });

        if (it != obj.setValues.end())
        {
            // if the set is found in the expected list
            // check for mesh list
            bool nodeBelongsToPipeline = false;

            std::vector<uint32_t> * newNodeMeshList = &newNode->node->meshList;
            std::vector<uint32_t> * pipelineNodeMeshList = &obj.pipelineNode->node->meshList;

            for each (uint32_t var in *pipelineNodeMeshList)
            {
                std::vector<uint32_t>::iterator itt;
                itt = std::find_if(newNodeMeshList->begin(), newNodeMeshList->end(), [&](uint32_t e) {
                    return e == newNode->node->setLevel; });
                if (itt != newNodeMeshList->end())
                {
                    nodeBelongsToPipeline = true;
                    break;
                }
            }

            // if even a single mesh id is present, an edge will exist
            // outgoing and incoming edge ??
        }

        // if yes find the appropriate level for the connections based on the set number
    }
    */
}

template<typename T>
inline PerPipelineNodeData * ForwardGraph<T>::CreatePipelineData(GraphNode<DrawGraphNode>* pipelineNode, std::vector<PerPipelineNodeData> & dataList)
{
    std::vector<SetWrapper *> * setWrapperList = &pipelineNode->node->setWrapperList;
    uint32_t numSetsWrappers = (uint32_t)setWrapperList->size();
    
    PerPipelineNodeData data = {};
    data.pipelineNode = pipelineNode;

    // set values
    for (uint32_t i = 0; i < numSetsWrappers; i++)
    {
        data.setValues.push_back(setWrapperList->at(i)->setValue);
    }

    std::sort(data.setValues.begin(), data.setValues.end());

    // type to node map

    // set to node map

    dataList.push_back(data);
    return &dataList.back();
}

template<typename T>
inline bool ForwardGraph<T>::CheckIfNodeBelongsToPipeline(GraphNode<DrawGraphNode>* pipelineNode, GraphNode<DrawGraphNode>* node, std::vector<uint32_t> * pipelineNodeSetList)
{
    bool nodeBelongsToPipeline = false;
    
    // mesh nodes dont have setvalues
    if (node->node->drawNodeType != DrawNodeTypes::MESH && 
        node->node->drawNodeType != DrawNodeTypes::DRAWING)
    {
        // check if the setvalue of the node is present in setlist of the pipeline node
        std::vector<uint32_t>::iterator it;
        it = std::find_if(pipelineNodeSetList->begin(), pipelineNodeSetList->end(), [&](uint32_t e) {
            return e == node->node->setLevel; });

        if (it == pipelineNodeSetList->end())
        {
            return false;
        }
    }

    // now check for mesh list
    std::vector<uint32_t> * nodeMeshList = &node->node->meshList;
    std::vector<uint32_t> * pipelineNodeMeshList = &pipelineNode->node->meshList;

    for each (uint32_t var in *pipelineNodeMeshList)
    {
        std::vector<uint32_t>::iterator itt;
        itt = std::find_if(nodeMeshList->begin(), nodeMeshList->end(), [&](uint32_t e) {
            return e == var; });
        if (itt != nodeMeshList->end())
        {
            nodeBelongsToPipeline = true;
            break;
        }
    }

    return nodeBelongsToPipeline;
}

template<typename T>
inline void ForwardGraph<T>::AddNodeToPipelineData(PerPipelineNodeData * data, GraphNode<DrawGraphNode> * node)
{
    {
        auto it = data->typeToNodeMap.find(node->node->drawNodeType);
        if (it == data->typeToNodeMap.end())
        {
            // not found
            data->typeToNodeMap.insert(std::pair<DrawNodeTypes, std::vector<GraphNode<DrawGraphNode> *>>({
                node->node->drawNodeType, std::vector<GraphNode<DrawGraphNode> *>{ node } }));

        }
        else
        {
            // found
            it->second.push_back(node);
        }
    }

    if(node->node->drawNodeType != DrawNodeTypes::MESH && 
        node->node->drawNodeType != DrawNodeTypes::DRAWING)
    {
        auto it = data->setToNodesMap.find(node->node->setLevel);
        if (it == data->setToNodesMap.end())
        {
            // not found
            data->setToNodesMap.insert(std::pair<uint32_t, std::vector<GraphNode<DrawGraphNode> *>>({
                node->node->setLevel, std::vector<GraphNode<DrawGraphNode> *>{ node } }));
        }
        else
        {
            it->second.push_back(node);
        }
    }
}

template<typename T>
inline void ForwardGraph<T>::CreateEdgesWithinPipeline(PerPipelineNodeData * data)
{
    for (uint32_t i = 0; i < (uint32_t)data->setValues.size() - 1; i++)
    {
        uint32_t higherSetLevel = data->setValues[i];
        uint32_t lowerSetLevel = data->setValues[i+1];
        
        if (higherSetLevel == ResourceSets::CAMERA || (lowerSetLevel == ResourceSets::TRANSFORM && i == 0))
        {
            // already connected to pipeline node 
            continue;
        }

        //ASSERT_MSG_DEBUG(0, "Case yet to be handled");

        if (lowerSetLevel == ResourceSets::TRANSFORM)
        {
            // connect to the mesh node
            std::vector<GraphNode<DrawGraphNode>*> * highNodeList = &data->setToNodesMap[higherSetLevel];
            std::vector<GraphNode<DrawGraphNode>*> * lowNodeList = &data->typeToNodeMap[DrawNodeTypes::MESH];
            
            for each(auto highNode in *highNodeList)
            {
                for each(auto lowNode in *lowNodeList)
                {
                    bool shouldBeConnected = CheckForCommonMeshId(highNode, lowNode);
                    if (shouldBeConnected)
                        drawGraph->AttachDirectedEdge(highNode, lowNode);
                }
            }
        }
        else
        {
            std::vector<GraphNode<DrawGraphNode>*> * highNodeList = &data->setToNodesMap[higherSetLevel];
            std::vector<GraphNode<DrawGraphNode>*> * lowNodeList = &data->setToNodesMap[lowerSetLevel];
            // for i == 1 only outgoing edge is required as incoming edges is from pipeline node, already connected.

            // we dont need to create both incoming and outgoing for all nodes, only outgoing should do, i guess.
            {
                for each(auto highNode in *highNodeList)
                {
                    for each(auto lowNode in *lowNodeList)
                    {
                        bool shouldBeConnected = CheckForCommonMeshId(highNode, lowNode);
                        if (shouldBeConnected)
                            drawGraph->AttachDirectedEdge(highNode, lowNode);
                    }
                }
            }
        }
    }
}

template<typename T>
inline bool ForwardGraph<T>::CheckForCommonMeshId(GraphNode<DrawGraphNode>* src, GraphNode<DrawGraphNode>* dest)
{
    bool commonIdFound = false;
    std::vector<uint32_t>* srcMeshList = &src->node->meshList;
    std::vector<uint32_t>* destMeshList = &dest->node->meshList;

    for each(auto srcId in *srcMeshList)
    {
        std::vector<uint32_t>::iterator it;
        it = std::find_if(destMeshList->begin(), destMeshList->end(), [&](uint32_t e) {
            return e == srcId; });

        if (it != destMeshList->end())
        {
            return true;
        }
    }

    return false;
}

template<typename T>
inline void ForwardGraph<T>::CreateConnections(GraphNode<DrawGraphNode>* node, NodeSetPerPass * dataSetPerPass)
{
    DrawNodeTypes nodeType = node->node->drawNodeType;

    if (node->node->drawNodeType == DrawNodeTypes::PIPELINE)
    {
        PerPipelineNodeData * pipelineData = CreatePipelineData(node, dataSetPerPass->pipelineNodeDataList);
        CreateEdgeForPipelineNode(node, pipelineData, dataSetPerPass);
    }
    else
    {
        //if entry present then insert a new node
        auto it = dataSetPerPass->typeToNodeMap.find(nodeType);
        if (it != dataSetPerPass->typeToNodeMap.end())
        {
            it->second.push_back(node);
        }
        else
        {
            dataSetPerPass->typeToNodeMap.insert(std::pair<DrawNodeTypes, std::vector<GraphNode<DrawGraphNode> *>>({
                nodeType, std::vector<GraphNode<DrawGraphNode> *>{ node } }));
        }

        if (node->node->drawNodeType != DrawNodeTypes::MESH &&
            node->node->drawNodeType != DrawNodeTypes::DRAWING)
        {
            uint32_t setLevel = node->node->setLevel;
            auto itt = dataSetPerPass->setToNodesMap.find(setLevel);
            if (itt != dataSetPerPass->setToNodesMap.end())
            {
                itt->second.push_back(node);
            }
            else
            {
                dataSetPerPass->setToNodesMap.insert(std::pair<uint32_t, std::vector<GraphNode<DrawGraphNode> *>>({
                    setLevel, std::vector<GraphNode<DrawGraphNode> *>{ node } }));
            }
        }

        // Check with existing pipeline nodes if this node belongs to any of them
        for each(auto data in dataSetPerPass->pipelineNodeDataList)
        {
            ASSERT_MSG_DEBUG(0, "Case yet to be handled");

            bool isNodeInPipeline = CheckIfNodeBelongsToPipeline(data.pipelineNode, node, &data.setValues);
            // if yes add to the node list 
            if (isNodeInPipeline)
                AddNodeToPipelineData(&data, node);
            // create edge with existing
        }
    }
}

template<typename T>
inline void ForwardGraph<T>::Init(Graph<DrawGraphNode>* drawGraph)
{
    this->drawGraph = drawGraph;

    // Create a dummy node, which wont have any connections, hence all the nodes
    // will get traversed to find this dummy, Hopefully.
    NodeSetPerPass * nodeSetColorPass, * nodeSetDepthPass;
    nodeSetColorPass = new NodeSetPerPass;
    nodeSetColorPass->tag = RenderPassTag::ColorPass;
    
    nodeSetDepthPass = new NodeSetPerPass;
    nodeSetDepthPass->tag = RenderPassTag::DepthPass;

    nodeDataPerPassMap.clear();
    nodeDataPerPassMap.insert(std::pair<RenderPassTag, NodeSetPerPass*>({ RenderPassTag::ColorPass, nodeSetColorPass }));
    nodeDataPerPassMap.insert(std::pair<RenderPassTag, NodeSetPerPass*>({ RenderPassTag::DepthPass, nodeSetDepthPass }));

    dummyNode = new IndexedDrawNode;
    dummyNode->tag = RenderPassTag::ColorPass;
    dummyGraphNode = new GraphNode<DrawGraphNode>(dummyNode);
    AddNode(dummyGraphNode);
}

template<typename T>
inline void ForwardGraph<T>::DeInit()
{
    for each(auto obj in nodeDataPerPassMap)
    {
        delete obj.second;
    }

    delete dummyGraphNode;
    delete dummyNode;
}

template<typename T>
inline void ForwardGraph<T>::Update(DrawCommandBuffer<T>* dcb, const RenderPassTag & tag)
{
    NodeSetPerPass * dataSetPerPass = nodeDataPerPassMap[tag];

    std::vector<GraphNode<DrawGraphNode> *>* cameraNodeList = &dataSetPerPass->typeToNodeMap[DrawNodeTypes::CAMERA];
    std::vector<GraphNode<DrawGraphNode> *>* drawingNodeList = &dataSetPerPass->typeToNodeMap[DrawNodeTypes::DRAWING];
       
    DrawGraphUtil::setOffset = 0;
    DrawGraphUtil::descriptorIdList.clear();
    for each(auto cam in *cameraNodeList)
    {
        // searching for a disconnected dummy node, it will allow us iterate through all nodes
        drawGraph->DepthFirstTraversal(cam->GetNodeId(), dummyGraphNode->GetNodeId());
    }
}

template<typename T>
inline ForwardGraph<T>::~ForwardGraph()
{
}

template<typename T>
inline void ForwardGraph<T>::AddNode(GraphNode<DrawGraphNode> * node)
{
    drawGraph->Push(node);

    DrawNodeTypes nodeType = node->node->drawNodeType;
    RenderPassTag passTag = node->node->tag;

    NodeSetPerPass * dataSetPerPass;
    //TODO : remove this, its bad, replace with a switch
    if ((((uint32_t)passTag | (uint32_t)RenderPassTag::ColorPass) == (uint32_t)RenderPassTag::ColorPass) ||
        (((uint32_t)passTag | (uint32_t)RenderPassTag::DepthPass) == (uint32_t)RenderPassTag::DepthPass))
    {
        dataSetPerPass = nodeDataPerPassMap[passTag];
        CreateConnections(node, dataSetPerPass);
    }
    else if ((uint32_t)passTag == ((uint32_t)RenderPassTag::DepthPass | (uint32_t)RenderPassTag::ColorPass))
    {
        // perform the connections for both the passes
        dataSetPerPass = nodeDataPerPassMap[RenderPassTag::ColorPass];
        CreateConnections(node, dataSetPerPass);

        dataSetPerPass = nodeDataPerPassMap[RenderPassTag::DepthPass];
        CreateConnections(node, dataSetPerPass);
    }

    /*
    switch ((uint32_t)passTag)
    {
    case (uint32_t)RenderPassTag::ColorPass:
    case (uint32_t)RenderPassTag::DepthPass:
        // perform the normal connections
        dataSetPerPass = nodeDataPerPassMap[passTag];
        CreateConnections(node, dataSetPerPass);
        break;

    case ((uint32_t)(RenderPassTag::ColorPass | RenderPassTag::DepthPass)):
        // perform the connections for both the passes
        dataSetPerPass = nodeDataPerPassMap[RenderPassTag::ColorPass];
        CreateConnections(node, dataSetPerPass);

        dataSetPerPass = nodeDataPerPassMap[RenderPassTag::DepthPass];
        CreateConnections(node, dataSetPerPass);
        break;

    case ((uint32_t)RenderPassTag::None):
    default:
        ASSERT_MSG(0, "Invalid renderPass");
    }
    */

}

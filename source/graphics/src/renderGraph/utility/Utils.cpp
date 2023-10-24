#include "renderGraph/utility/Utils.h"
#include "renderGraph/Task.h"

void Renderer::RenderGraph::Utils::AddEdge(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
    const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
{
    auto fillAttrib = [](Renderer::RenderGraph::NodeDrawAttribs& attrib, RenderGraphNodeType type, std::string name)
    {
        if (type == RenderGraphNodeType::RESOURCE_NODE)
        {
            attrib.nodeColor = "red";
            attrib.nodeName = name;
            attrib.nodeShape = "oval";
        }
        else
        {
            attrib.nodeColor = "green";
            attrib.nodeName = name;
            attrib.nodeShape = "rectangle";
        }
    };

    Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
    fillAttrib(srcAttrib, srcNode->GetNodeData()->GetNodeType(), srcNode->GetNodeData()->GetNodeName());
    fillAttrib(dstAttrib, destNode->GetNodeData()->GetNodeType(), destNode->GetNodeData()->GetNodeName());

    std::string edgeLabel = "";
    if (usage != Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE &&
        srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        switch (usage)
        {
        case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY:
            edgeLabel = "R";
            break;
        case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE:
            edgeLabel = "RW";
            break;
        case Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY:
            edgeLabel = "W";
            break;
        default:
            ASSERT_MSG_DEBUG(0, "Invalid option");
        }
    }

    graph.AddToPrintLog(srcAttrib, dstAttrib, edgeLabel);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(destNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(srcNode->GetNodeData())->GetResource();

        task->AddInput(resource, usage, srcNode->GetNodeId());
    }

    graph.AttachDirectedEdge(srcNode, destNode);
}

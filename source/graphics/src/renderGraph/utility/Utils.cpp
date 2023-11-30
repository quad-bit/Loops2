#include "renderGraph/utility/Utils.h"
#include "renderGraph/Task.h"

namespace
{
    void CreatePrintGraphInfo(
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* srcNode,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* destNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage usage,
        Core::Enums::ImageLayout exepectedLayout,
        Core::Enums::ImageLayout previousLayout)
    {
        auto fillAttrib = [](Renderer::RenderGraph::NodeDrawAttribs& attrib,
            Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node
            )
        {
            Renderer::RenderGraph::Utils::RenderGraphNodeType type = node->GetNodeData()->GetNodeType();

            if (type == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
            {
                attrib.nodeColor = "red";
                attrib.nodeName = node->GetNodeData()->GetNodeName() + ":\n" + ((Renderer::RenderGraph::ResourceNode*)node->GetNodeData())->GetResource()[0]->GetResourceName();
                attrib.nodeShape = "oval";
            }
            else
            {
                std::string name = "";
                Renderer::RenderGraph::TaskType taskType = ((Renderer::RenderGraph::TaskNode*)node->GetNodeData())->GetTask()->GetTaskType();
                if (taskType == Renderer::RenderGraph::TaskType::RENDER_TASK)
                {
                    attrib.nodeColor = "green";
                    name = "Graphics:\n";
                }
                else if (taskType == Renderer::RenderGraph::TaskType::COMPUTE_TASK)
                {
                    attrib.nodeColor = "blue";
                    name = "Compute:\n";

                }
                else if (taskType == Renderer::RenderGraph::TaskType::TRANSFER_TASK)
                {
                    attrib.nodeColor = "purple";
                    name = "Transfer:\n";

                }
                else if (taskType == Renderer::RenderGraph::TaskType::DOWNLOAD_TASK)
                {
                    attrib.nodeColor = "pink";
                    name = "Download:\n";
                }
                attrib.nodeName = name + node->GetNodeData()->GetNodeName();
                attrib.nodeShape = "rectangle";
            }
        };

        Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
        fillAttrib(srcAttrib, srcNode);
        fillAttrib(dstAttrib, destNode);

        auto GetLayoutValue = [](const Core::Enums::ImageLayout layout) -> std::string
        {
            std::string value;
            switch (layout)
            {
            case Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                value = "LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
                value = "LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                value = "LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_GENERAL:
                value = "LAYOUT_GENERAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_PREINITIALIZED:
                value = "LAYOUT_PREINITIALIZED";
                break;
            case Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR:
                value = "LAYOUT_PRESENT_SRC_KHR";
                break;
            case Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                value = "LAYOUT_SHADER_READ_ONLY_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
                value = "LAYOUT_TRANSFER_DST_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL:
                value = "LAYOUT_TRANSFER_SRC_OPTIMAL";
                break;
            case Core::Enums::ImageLayout::LAYOUT_UNDEFINED:
                value = "LAYOUT_UNDEFINED";
                break;
            default:
                ASSERT_MSG_DEBUG(0, "invalid");
                break;
            }
            return value;
        };


        std::string edgeLabel = GetLayoutValue(previousLayout);

        if (usage != Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE &&
            srcNode->GetNodeData()->GetNodeType() == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
        {
            switch (usage)
            {
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY:
                edgeLabel += "\nREAD\n";
                break;
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE:
                edgeLabel += "\nREAD_WRITE\n";
                break;
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY:
                edgeLabel += "\nWRITE\n";
                break;
            default:
                ASSERT_MSG_DEBUG(0, "Invalid option");
            }
        }
        edgeLabel += GetLayoutValue(exepectedLayout);
        graph.AddToPrintLog(srcAttrib, dstAttrib, edgeLabel);
    }


    void CreatePrintGraphInfo(
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* srcNode,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* destNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage usage)
    {
        auto fillAttrib = [](Renderer::RenderGraph::NodeDrawAttribs& attrib,
            Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node
            )
        {
            Renderer::RenderGraph::Utils::RenderGraphNodeType type = node->GetNodeData()->GetNodeType();
            std::string name = node->GetNodeData()->GetNodeName();

            if (type == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
            {
                attrib.nodeColor = "red";
                attrib.nodeName = node->GetNodeData()->GetNodeName() + ":\n" + ((Renderer::RenderGraph::ResourceNode*)node->GetNodeData())->GetResource()[0]->GetResourceName();
                attrib.nodeShape = "oval";
            }
            else
            {
                std::string name = "";
                Renderer::RenderGraph::TaskType taskType = ((Renderer::RenderGraph::TaskNode*)node->GetNodeData())->GetTask()->GetTaskType();
                if (taskType == Renderer::RenderGraph::TaskType::RENDER_TASK)
                {
                    attrib.nodeColor = "green";
                    name = "Graphics:\n";
                }
                else if (taskType == Renderer::RenderGraph::TaskType::COMPUTE_TASK)
                {
                    attrib.nodeColor = "blue";
                    name = "Compute:\n";

                }
                else if (taskType == Renderer::RenderGraph::TaskType::TRANSFER_TASK)
                {
                    attrib.nodeColor = "purple";
                    name = "Transfer:\n";

                }
                else if (taskType == Renderer::RenderGraph::TaskType::DOWNLOAD_TASK)
                {
                    attrib.nodeColor = "pink";
                    name = "Download:\n";
                }
                attrib.nodeName = name + node->GetNodeData()->GetNodeName();
                attrib.nodeShape = "rectangle";
            }
        };

        Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
        fillAttrib(srcAttrib, srcNode);
        fillAttrib(dstAttrib, destNode);

        std::string edgeLabel = "";
        if (usage != Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE &&
            srcNode->GetNodeData()->GetNodeType() == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
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
    }
};

void Renderer::RenderGraph::Utils::AddEdge(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
    const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
{
    CreatePrintGraphInfo(graph, srcNode, destNode, usage);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(destNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(srcNode->GetNodeData())->GetResource();

        task->AddInput(ConnectionInfo{ usage, resource, srcNode->GetNodeId() });
    }
    else if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(srcNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(destNode->GetNodeData())->GetResource();

        task->AddOutput(ConnectionInfo{ usage, resource, destNode->GetNodeId() });
    }

    graph.AttachDirectedEdge(srcNode, destNode);
}

void Renderer::RenderGraph::Utils::AddEdge(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
    const Renderer::RenderGraph::Utils::ConnectionInfo& connectionInfo)
{
    CreatePrintGraphInfo(graph, srcNode, destNode, connectionInfo.m_usage);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(destNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(srcNode->GetNodeData())->GetResource();

        task->AddInput(connectionInfo);
    }
    else if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(srcNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(destNode->GetNodeData())->GetResource();

        task->AddOutput(connectionInfo);
    }

    graph.AttachDirectedEdge(srcNode, destNode);
}

void Renderer::RenderGraph::Utils::AddEdge(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
    const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage,
    const Core::Enums::ImageLayout expectedImageLayout,
    const Core::Enums::ImageLayout previousImageLayout)
{
    CreatePrintGraphInfo(graph, srcNode, destNode, usage, expectedImageLayout, previousImageLayout);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(destNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(srcNode->GetNodeData())->GetResource();

        ImageResourceConnectionInfo imageInfo{};
        imageInfo.m_prevImageLayout = previousImageLayout;
        imageInfo.m_expectedImageLayout = expectedImageLayout;

        task->AddInput(ConnectionInfo{ usage, resource, srcNode->GetNodeId(), &imageInfo});
    }
    else if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(srcNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(destNode->GetNodeData())->GetResource();

        task->AddOutput(ConnectionInfo{ usage, resource, destNode->GetNodeId() });
    }

    graph.AttachDirectedEdge(srcNode, destNode);
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> Renderer::RenderGraph::Utils::CreatePerFrameImageResource(
    const Core::Wrappers::ImageCreateInfo& createInfo,
    std::vector<std::string> names, uint32_t count)
{
    std::vector<uint32_t> imageIds;
    std::vector<uint32_t> memIds;

    for (uint32_t i = 0; i < count; i++)
    {
        imageIds.push_back(VulkanInterfaceAlias::CreateImage(createInfo, names[i]) );
    }

    //memory
    {
        // Get the image memory req
        Core::Wrappers::MemoryRequirementInfo req = VulkanInterfaceAlias::GetImageMemoryRequirement(imageIds[0]);

        // Allocate the memory
        size_t allocationSize = req.size * count;
        Core::Enums::MemoryType userReq[1]{ Core::Enums::MemoryType::DEVICE_LOCAL_BIT };
        auto memId = VulkanInterfaceAlias::AllocateMemory(&req, &userReq[0], allocationSize);
        memIds.push_back(memId);

        // Bind the memory to the image
        for (uint32_t i = 0; i < imageIds.size(); i++)
            VulkanInterfaceAlias::BindImageMemory(imageIds[i], memId, req.size * i);

    }

    //image view
    {
        auto aspect = Core::Enums::ImageAspectFlag::IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
        auto it = std::find_if(createInfo.m_usages.begin(), createInfo.m_usages.end(), [&](const Core::Enums::ImageUsage& usage) { return usage == Core::Enums::ImageUsage::USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; });
        if (it != createInfo.m_usages.end())
        {
            // color attachment
            aspect = Core::Enums::ImageAspectFlag::IMAGE_ASPECT_DEPTH_BIT;
        }
        else
        {
            // color attachment
            aspect = Core::Enums::ImageAspectFlag::IMAGE_ASPECT_COLOR_BIT;
        }

        ASSERT_MSG_DEBUG(aspect != Core::Enums::ImageAspectFlag::IMAGE_ASPECT_FLAG_BITS_MAX_ENUM, "wrong aspect");

        Core::Wrappers::ImageViewCreateInfo viewInfo = {};
        viewInfo.m_baseArrayLayer = 0;
        viewInfo.m_baseMipLevel = 0;
        viewInfo.m_components[0] = Core::Enums::ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.m_components[1] = Core::Enums::ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.m_components[2] = Core::Enums::ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.m_components[3] = Core::Enums::ComponentSwizzle::COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.m_format = createInfo.m_format;
        viewInfo.m_imageAspect = aspect;
        viewInfo.m_layerCount = 1;
        viewInfo.m_levelCount = 1;
        viewInfo.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;

        for (uint32_t i = 0; i < imageIds.size(); i++)
            VulkanInterfaceAlias::CreateImageView(viewInfo, imageIds[i]);
    }

    return std::make_pair(imageIds, memIds);
}

void Renderer::RenderGraph::Utils::AddSwapchainNode(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph, Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode, const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage, const Core::Enums::ImageLayout expectedImageLayout)
{

}

std::vector<uint32_t> Renderer::RenderGraph::Utils::GetSwapchainImages()
{
    std::vector<uint32_t> imageIds;
    for (uint32_t i = 0; i < RendererSettings::GetSwapBufferCount(); i++)
    {
        imageIds.push_back(VulkanInterfaceAlias::GetSwapchainImageId(i));
    }
    return imageIds;
}

void Renderer::RenderGraph::Utils::DestroyPerFrameImageResource(const std::vector<uint32_t>& imageIds, std::vector<uint32_t>& memIds)
{
    for (uint32_t i = 0; i < imageIds.size(); i++)
    {
        VulkanInterfaceAlias::DestroyImage(imageIds[i], false);
    }

    VulkanInterfaceAlias::FreeMemory(memIds.data(), memIds.size());
}

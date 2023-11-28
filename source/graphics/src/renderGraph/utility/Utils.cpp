#include "renderGraph/utility/Utils.h"
#include "renderGraph/Task.h"

void Renderer::RenderGraph::Utils::AddEdge(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
    const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage)
{
    auto fillAttrib = [](Renderer::RenderGraph::NodeDrawAttribs& attrib,
        Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* node
        )
    {
        RenderGraphNodeType type = node->GetNodeData()->GetNodeType();
        std::string name = node->GetNodeData()->GetNodeName();

        if (type == RenderGraphNodeType::RESOURCE_NODE)
        {
            attrib.nodeColor = "red";
            attrib.nodeName = name;
            attrib.nodeShape = "oval";
        }
        else
        {
            Renderer::RenderGraph::TaskType taskType = ((TaskNode*)node->GetNodeData())->GetTask()->GetTaskType();
            if (taskType == TaskType::RENDER_TASK)
                attrib.nodeColor = "green";
            else if (taskType == TaskType::COMPUTE_TASK)
                attrib.nodeColor = "blue";
            else if (taskType == TaskType::TRANSFER_TASK)
                attrib.nodeColor = "purple";
            else if (taskType == TaskType::DOWNLOAD_TASK)
                attrib.nodeColor = "pink";
            attrib.nodeName = name;
            attrib.nodeShape = "rectangle";
        }
    };

    Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
    fillAttrib(srcAttrib, srcNode);// ->GetNodeData()->GetNodeType(), srcNode->GetNodeData()->GetNodeName());
    fillAttrib(dstAttrib, destNode);//->GetNodeData()->GetNodeType(), destNode->GetNodeData()->GetNodeName());

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

void Renderer::RenderGraph::Utils::DestroyPerFrameImageResource(const std::vector<uint32_t>& imageIds, std::vector<uint32_t>& memIds)
{
    for (uint32_t i = 0; i < imageIds.size(); i++)
    {
        VulkanInterfaceAlias::DestroyImage(imageIds[i], false);
    }

    VulkanInterfaceAlias::FreeMemory(memIds.data(), memIds.size());
}

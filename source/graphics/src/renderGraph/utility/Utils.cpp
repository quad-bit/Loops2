#include "renderGraph/utility/Utils.h"
#include "renderGraph/Task.h"

namespace
{
    void FillAtribute(Renderer::RenderGraph::NodeDrawAttribs& attrib,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node
    )
    {
        Renderer::RenderGraph::Utils::RenderGraphNodeType type = node->GetNodeData()->GetNodeType();

        if (type == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
        {
            Renderer::ResourceManagement::ResourceType resourceType = ((Renderer::RenderGraph::ResourceNode*)node->GetNodeData())->GetResource()[0]->GetResourceType();
            if (resourceType == Renderer::ResourceManagement::ResourceType::IMAGE)
            {
                attrib.nodeColor = "red";
                attrib.nodeShape = "oval";
            }
            else
            {
                attrib.nodeColor = "orange";
                attrib.nodeShape = "circle";
            }

            attrib.nodeName = node->GetNodeData()->GetNodeName() + ":\n" + ((Renderer::RenderGraph::ResourceNode*)node->GetNodeData())->GetResource()[0]->GetResourceName();
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
    }

    
    std::string GetUsageValue(Renderer::RenderGraph::Utils::ResourceMemoryUsage usage,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node)
    {
        std::string value;
        if (usage != Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE &&
            node->GetNodeData()->GetNodeType() == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
        {
            switch (usage)
            {
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY:
                value = "\nREAD\n";
                break;
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE:
                value = "\nREAD_WRITE\n";
                break;
            case Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY:
                value = "\nWRITE\n";
                break;
            default:
                ASSERT_MSG_DEBUG(0, "Invalid option");
            }
        }
        return value;
    }

    void CreatePrintGraphInfo(
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* srcNode,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* destNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage usage,
        Core::Enums::ImageLayout exepectedLayout,
        Core::Enums::ImageLayout previousLayout)
    {
        Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
        FillAtribute(srcAttrib, srcNode);
        FillAtribute(dstAttrib, destNode);

        std::string edgeLabel = Core::Utility::ConvertImageLayoutToString(previousLayout);
        edgeLabel += GetUsageValue(usage, srcNode);

        //if (usage != Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE &&
        //    srcNode->GetNodeData()->GetNodeType() == Renderer::RenderGraph::Utils::RenderGraphNodeType::RESOURCE_NODE)
        //{
        //    switch (usage)
        //    {
        //    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY:
        //        edgeLabel += "\nREAD\n";
        //        break;
        //    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE:
        //        edgeLabel += "\nREAD_WRITE\n";
        //        break;
        //    case Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY:
        //        edgeLabel += "\nWRITE\n";
        //        break;
        //    default:
        //        ASSERT_MSG_DEBUG(0, "Invalid option");
        //    }
        //}

        edgeLabel += Core::Utility::ConvertImageLayoutToString(exepectedLayout);
        graph.AddToPrintLog(srcAttrib, dstAttrib, edgeLabel);
    }

    void CreatePrintGraphInfo(
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* srcNode,
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* destNode,
        Renderer::RenderGraph::Utils::ResourceMemoryUsage usage)
    {
        Renderer::RenderGraph::NodeDrawAttribs srcAttrib{}, dstAttrib{};
        FillAtribute(srcAttrib, srcNode);
        FillAtribute(dstAttrib, destNode);

        std::string edgeLabel = GetUsageValue(usage, srcNode);
        graph.AddToPrintLog(srcAttrib, dstAttrib, edgeLabel);
    }
};

void Renderer::RenderGraph::Utils::AddTaskOutput(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode
    )
{
    CreatePrintGraphInfo(graph, srcNode, destNode, Renderer::RenderGraph::Utils::ResourceMemoryUsage::NONE);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(srcNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(destNode->GetNodeData())->GetResource();

        task->AddOutput(ConnectionInfo{ ResourceMemoryUsage::NONE, resource, destNode->GetNodeId() });
    }
    else
    {
        ASSERT_MSG_DEBUG(0, "Meant for output node");
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
    ASSERT_MSG_DEBUG(expectedImageLayout != Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL, "Use dedicated function");
    ASSERT_MSG_DEBUG(expectedImageLayout != Core::Enums::ImageLayout::LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, "Use dedicated function");
    ASSERT_MSG_DEBUG(expectedImageLayout != Core::Enums::ImageLayout::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, "Use dedicated function");

    CreatePrintGraphInfo(graph, srcNode, destNode, usage, expectedImageLayout, previousImageLayout);

    if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(destNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(srcNode->GetNodeData())->GetResource();

        ImageResourceConnectionInfo imageInfo{};
        imageInfo.m_prevImageLayout = previousImageLayout;
        imageInfo.m_expectedImageLayout = expectedImageLayout;

        task->AddInput(ConnectionInfo{ usage, resource, srcNode->GetNodeId(), imageInfo});
    }
    else if (srcNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE &&
        destNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(srcNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(destNode->GetNodeData())->GetResource();

        ImageResourceConnectionInfo imageInfo{};
        task->AddOutput(ConnectionInfo{ usage, resource, destNode->GetNodeId(), imageInfo });
    }

    graph.AttachDirectedEdge(srcNode, destNode);
}

void Renderer::RenderGraph::Utils::AddInputAsColorAttachment(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* resourceNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* taskNode,
    const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage,
    const Core::Enums::ImageLayout previousImageLayout,
    uint32_t inputSlot)
{
    if (resourceNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        taskNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(taskNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(resourceNode->GetNodeData())->GetResource();

        auto taskType = task->GetTaskType();
        ASSERT_MSG_DEBUG(taskType == TaskType::RENDER_TASK, "invalid usage");

        ImageResourceConnectionInfo imageInfo{};
        imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imageInfo.m_prevImageLayout = previousImageLayout;
        imageInfo.m_colorAttachmentSlot = inputSlot;

        task->AddInput(ConnectionInfo{ usage, resource, resourceNode->GetNodeId(), imageInfo, std::nullopt });
    }
    else
        ASSERT_MSG_DEBUG(0, "Meant for only input");

    graph.AttachDirectedEdge(resourceNode, taskNode);
}

void Renderer::RenderGraph::Utils::AddInputAsDepthAttachment(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* resourceNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* taskNode
)
{
    if (resourceNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        taskNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(taskNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(resourceNode->GetNodeData())->GetResource();

        auto taskType = task->GetTaskType();
        ASSERT_MSG_DEBUG(taskType == TaskType::RENDER_TASK, "invalid usage");

        ImageResourceConnectionInfo imageInfo{};
        imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //task->AddInput(ConnectionInfo{ resource, imageInfo });
    }
    else
        ASSERT_MSG_DEBUG(0, "resource/task node mismatch");

    graph.AttachDirectedEdge(resourceNode, taskNode);
}

void Renderer::RenderGraph::Utils::AddInputAsShaderResource(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* resourceNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* taskNode
    )
{
    if (resourceNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        taskNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(taskNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(resourceNode->GetNodeData())->GetResource();

        auto taskType = task->GetTaskType();

        ImageResourceConnectionInfo imageInfo{};
        if (taskType == TaskType::RENDER_TASK)
        {
            imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else if (taskType == TaskType::COMPUTE_TASK)
        {
            imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_GENERAL;
        }
        else
        {
            ASSERT_MSG_DEBUG(0, "invalid usage");
        }

        //task->AddInput(ConnectionInfo{ resource, imageInfo });
    }
    else
        ASSERT_MSG_DEBUG(0, "resource/task node mismatch");

    graph.AttachDirectedEdge(resourceNode, taskNode);
}

void Renderer::RenderGraph::Utils::AddInputAsTransferData(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* resourceNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* taskNode,
    bool isTransferSource)
{
    if (resourceNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::RESOURCE_NODE &&
        taskNode->GetNodeData()->GetNodeType() == RenderGraphNodeType::TASK_NODE)
    {
        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(taskNode->GetNodeData())->GetTask();
        auto resource = static_cast<Renderer::RenderGraph::ResourceNode*>(resourceNode->GetNodeData())->GetResource();

        auto taskType = task->GetTaskType();
        ASSERT_MSG_DEBUG(taskType == TaskType::TRANSFER_TASK, "invalid usage");

        ImageResourceConnectionInfo imageInfo{};
        if (isTransferSource)
        {
            imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL;
        }
        else
        {
            imageInfo.m_expectedImageLayout = Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        //task->AddInput(ConnectionInfo{ resource, imageInfo });
    }
    else
        ASSERT_MSG_DEBUG(0, "resource/task node mismatch");

    graph.AttachDirectedEdge(resourceNode, taskNode);
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

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> Renderer::RenderGraph::Utils::CreatePerFrameBufferResource(
    const Core::Wrappers::BufferCreateInfo& createInfo,
    std::vector<std::string> names,
    uint32_t count)
{
    std::vector<uint32_t> bufferIds;
    for (uint32_t i = 0; i < count; i++)
    {
        auto value = VulkanInterfaceAlias::CreateBuffer(createInfo, false);
        bufferIds.push_back(value.first);
    }

    uint32_t memId = VulkanInterfaceAlias::AllocateBufferSharedMemory(bufferIds.data(), bufferIds.size());
    return std::pair(bufferIds, std::vector<uint32_t>{memId});
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

void Renderer::RenderGraph::Utils::DestroyPerFrameBufferResource(const std::vector<uint32_t>& bufferIds, std::vector<uint32_t>& memIds)
{
    VulkanInterfaceAlias::DestroyBuffer((uint32_t*)bufferIds.data(), bufferIds.size());
    VulkanInterfaceAlias::FreeMemory(memIds.data(), memIds.size());
}

void Renderer::RenderGraph::Utils::AddTaskBufferInput(
    Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* taskNode,
    Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* resourceNode,
    std::vector<Core::Enums::BufferType> bufferType)
{
}

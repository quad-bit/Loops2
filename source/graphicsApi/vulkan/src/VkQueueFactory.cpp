#include "VkQueueFactory.h"
#include "utility/VulkanUtility.h"
#include <CorePrecompiled.h>

GfxVk::Utility::VkQueueFactory* GfxVk::Utility::VkQueueFactory::instance = nullptr;

GfxVk::Utility::VkQueueWrapper * GfxVk::Utility::VkQueueFactory::MapQueueWrapper(const Core::Wrappers::QueueWrapper * wrapper)
{
    if (*wrapper->queueType == Core::Enums::PipelineType::GRAPHICS)
        for (uint32_t i = 0; i < graphicsQueueWrapperList.size(); i++)
        {
            if (wrapper->queueId == graphicsQueueWrapperList[i].queueId && wrapper->purpose == graphicsQueueWrapperList[i].purpose)
            {
                return &graphicsQueueWrapperList[i];
            }
        }

    if (*wrapper->queueType == Core::Enums::PipelineType::COMPUTE)
        for (uint32_t i = 0; i < computeQueueWrapperList.size(); i++)
        {
            if (wrapper->queueId == computeQueueWrapperList[i].queueId && wrapper->purpose == computeQueueWrapperList[i].purpose)
            {
                return &computeQueueWrapperList[i];
            }
        }

    if (*wrapper->queueType == Core::Enums::PipelineType::TRANSFER)
        for (uint32_t i = 0; i < transferQueueWrapperList.size(); i++)
        {
            if (wrapper->queueId == transferQueueWrapperList[i].queueId && wrapper->purpose == transferQueueWrapperList[i].purpose)
            {
                return &transferQueueWrapperList[i];
            }
        }

    ASSERT_MSG_DEBUG(0, "no queue found");
    return nullptr;
}

void GfxVk::Utility::VkQueueFactory::Init()
{
    PLOGD << "VkQueueFactory Init";

    uint32_t graphicsReq = VK_QUEUE_GRAPHICS_BIT;
    uint32_t computeReq = VK_QUEUE_COMPUTE_BIT;
    uint32_t transferReq = VK_QUEUE_TRANSFER_BIT;

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*GfxVk::Utility::CoreObjects::physicalDeviceObj, &qFamilyCount, nullptr);
    propertyList.resize(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(*GfxVk::Utility::CoreObjects::physicalDeviceObj, &qFamilyCount, propertyList.data());

    for (uint32_t j = 0; j < qFamilyCount; j++)
    {
        if (graphicsQueueFamilyIndex == -1 && ((propertyList[j].queueFlags & graphicsReq) == graphicsReq)
            && propertyList[j].queueCount >= minGraphicQueueRequired)
        {
            graphicsQueueFamilyIndex = j;
        }

        if (computeQueueFamilyIndex == -1 && ((propertyList[j].queueFlags & computeReq) == computeReq)
            && propertyList[j].queueCount >= minCopmuteQueueRequired)
        {
            computeQueueFamilyIndex = j;
        }

        if (transferQueueFamilyIndex == -1 && ((propertyList[j].queueFlags & transferReq) == transferReq)
            && propertyList[j].queueCount >= minTransferQueueRequired)
        {
            transferQueueFamilyIndex = j;
        }
    }

    ASSERT_MSG_DEBUG(graphicsQueueFamilyIndex != -1, "min 2 graphic queues required in a family");
    ASSERT_MSG_DEBUG(computeQueueFamilyIndex != -1, "min 2 compute queues required in a family");
    ASSERT_MSG_DEBUG(transferQueueFamilyIndex != -1, "min 1 transfer queues required in a family");
}

void GfxVk::Utility::VkQueueFactory::DeInit()
{
    PLOGD << "VkQueueFactory DeInit";

    delete[] graphicQueuePriority;

    if (computeQueuePriority != nullptr)
    delete[] computeQueuePriority;

    if(transferQueuePriority != nullptr)
    delete[] transferQueuePriority;

    for each(GfxVk::Utility::VkQueueWrapper obj in graphicsQueueWrapperList)
    {
        //if (obj.isQueueEnabled)
        {
            delete obj.queue;
        }
    }

    for each(GfxVk::Utility::VkQueueWrapper obj in computeQueueWrapperList)
    {
        //if (obj.isQueueEnabled)
        {
            delete obj.queue;
        }
    }

    for each(GfxVk::Utility::VkQueueWrapper obj in transferQueueWrapperList)
    {
        //if (obj.isQueueEnabled)
        {
            delete obj.queue;
        }
    }

    graphicsQueueWrapperList.clear();
    computeQueueWrapperList.clear();
    transferQueueWrapperList.clear();
}

void GfxVk::Utility::VkQueueFactory::Update()
{

}

GfxVk::Utility::VkQueueFactory * GfxVk::Utility::VkQueueFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Utility::VkQueueFactory();
    }
    return instance;
}

GfxVk::Utility::VkQueueFactory::~VkQueueFactory()
{
    
}

std::vector<VkDeviceQueueCreateInfo> GfxVk::Utility::VkQueueFactory::FindQueue()
{
    graphicQueuePriority = new float[minGraphicQueueRequired] {1.0f};

    std::vector<VkDeviceQueueCreateInfo> creatInfoList;

    VkDeviceQueueCreateInfo info = {};
    info.flags = 0;
    info.pNext = nullptr;
    info.pQueuePriorities = graphicQueuePriority;
    info.queueCount = minGraphicQueueRequired;
    info.queueFamilyIndex = graphicsQueueFamilyIndex;
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

    creatInfoList.push_back(info);

    graphicsQueueWrapperList.resize(minGraphicQueueRequired);

    uint32_t indexInGraphicFamily = 0;
    for (uint32_t i = 0; i < minGraphicQueueRequired; i++)
    {
        graphicsQueueWrapperList[i].queueFamilyIndex = graphicsQueueFamilyIndex;
        graphicsQueueWrapperList[i].queueType = Core::Enums::PipelineType::GRAPHICS;
        graphicsQueueWrapperList[i].indexInFamily = indexInGraphicFamily;
        graphicsQueueWrapperList[i].queueId = GetQueueId();
        graphicsQueueWrapperList[i].queue = new VkQueue;
        indexInGraphicFamily++;
    }

    uint32_t indexInComputeFamily = -1;

    if (graphicsQueueFamilyIndex == computeQueueFamilyIndex)
    {
        creatInfoList[0].queueCount = minGraphicQueueRequired + minCopmuteQueueRequired;
        indexInComputeFamily = indexInGraphicFamily;
        delete [] graphicQueuePriority;
        graphicQueuePriority = new float[minGraphicQueueRequired + minCopmuteQueueRequired]{ 1.0f };
        creatInfoList[0].pQueuePriorities = graphicQueuePriority;
        computeQueuePriority = nullptr;
    }
    else
    {
        computeQueuePriority = new float[minCopmuteQueueRequired] {1.0f};

        VkDeviceQueueCreateInfo info = {};
        info.flags = 0;
        info.pNext = nullptr;
        info.pQueuePriorities = computeQueuePriority;
        info.queueCount = minCopmuteQueueRequired;
        info.queueFamilyIndex = computeQueueFamilyIndex;
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        creatInfoList.push_back(info);

        indexInComputeFamily = 0;
    }

    computeQueueWrapperList.resize(minCopmuteQueueRequired);
    for (uint32_t i = 0; i < minCopmuteQueueRequired; i++)
    {
        computeQueueWrapperList[i].queueFamilyIndex = computeQueueFamilyIndex;
        computeQueueWrapperList[i].queueType = Core::Enums::PipelineType::COMPUTE;
        computeQueueWrapperList[i].indexInFamily = indexInComputeFamily;
        computeQueueWrapperList[i].queueId = GetQueueId();
        computeQueueWrapperList[i].queue = new VkQueue;
        indexInComputeFamily++;
    }

    uint32_t indexInTransferFamily = -1;

    if (graphicsQueueFamilyIndex == transferQueueFamilyIndex && 
        graphicsQueueFamilyIndex == computeQueueFamilyIndex)
    {
        creatInfoList[0].queueCount = minGraphicQueueRequired + minCopmuteQueueRequired + minTransferQueueRequired;
        indexInTransferFamily = indexInComputeFamily ;
        delete[] graphicQueuePriority;
        graphicQueuePriority = new float[minGraphicQueueRequired + minCopmuteQueueRequired + minTransferQueueRequired]{ 1.0f };
        creatInfoList[0].pQueuePriorities = graphicQueuePriority;
        transferQueuePriority = nullptr;
    }
    else if(transferQueueFamilyIndex == computeQueueFamilyIndex )
    {
        creatInfoList[1].queueCount = minCopmuteQueueRequired + minTransferQueueRequired;
        indexInTransferFamily = indexInComputeFamily;

        delete[] computeQueuePriority;
        computeQueuePriority = new float[minCopmuteQueueRequired + minTransferQueueRequired]{ 1.0f };
        creatInfoList[1].pQueuePriorities = computeQueuePriority;
        transferQueuePriority = nullptr;
    }
    else if (transferQueueFamilyIndex != computeQueueFamilyIndex &&
        transferQueueFamilyIndex != graphicsQueueFamilyIndex)
    {
        transferQueuePriority = new float[minTransferQueueRequired] {1.0f};
        VkDeviceQueueCreateInfo info = {};
        info.flags = 0;
        info.pNext = nullptr;
        info.pQueuePriorities = transferQueuePriority;
        info.queueCount = minTransferQueueRequired;
        info.queueFamilyIndex = transferQueueFamilyIndex;
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        creatInfoList.push_back(info);

        indexInTransferFamily = 0;
    }

    transferQueueWrapperList.resize(minTransferQueueRequired);
    for (uint32_t i = 0; i < minTransferQueueRequired; i++)
    {
        transferQueueWrapperList[i].queueFamilyIndex = transferQueueFamilyIndex;
        transferQueueWrapperList[i].queueType = Core::Enums::PipelineType::TRANSFER;
        transferQueueWrapperList[i].indexInFamily = indexInTransferFamily;
        transferQueueWrapperList[i].queueId = GetQueueId();
        transferQueueWrapperList[i].queue = new VkQueue;
        indexInTransferFamily++;
    }
    
    return creatInfoList; 
}

void GfxVk::Utility::VkQueueFactory::InitQueues()
{
    for (uint32_t i = 0; i < minGraphicQueueRequired; i++)
    {
        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, graphicsQueueWrapperList[i].queueFamilyIndex,
            graphicsQueueWrapperList[i].indexInFamily, graphicsQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*graphicsQueueWrapperList[i].queue != VK_NULL_HANDLE, "Graphics Queue not available ");
    }

    for (uint32_t i = 0; i < minCopmuteQueueRequired; i++)
    {
        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, computeQueueWrapperList[i].queueFamilyIndex,
            computeQueueWrapperList[i].indexInFamily, computeQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*computeQueueWrapperList[i].queue != VK_NULL_HANDLE, "Compute Queue not available ");
    }

    for (uint32_t i = 0; i < minTransferQueueRequired; i++)
    {
        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, transferQueueWrapperList[i].queueFamilyIndex, 
            transferQueueWrapperList[i].indexInFamily, transferQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*transferQueueWrapperList[i].queue != VK_NULL_HANDLE, "Transfer Queue not available ");
    }
}

VkQueue * GfxVk::Utility::VkQueueFactory::GetQueue(Core::Enums::PipelineType qType, const uint32_t & id)
{
    std::vector<GfxVk::Utility::VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case Core::Enums::PipelineType::GRAPHICS:
        {
            it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId== id; });
            ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    case Core::Enums::PipelineType::COMPUTE:
        {
            it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == id; });
            ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    case Core::Enums::PipelineType::TRANSFER:
        {
            it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == id; });
            ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }

    return nullptr;
}

VkQueue * GfxVk::Utility::VkQueueFactory::GetQueue(VkQueueFlagBits qType, const uint32_t & id)
{
    std::vector<GfxVk::Utility::VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
    return nullptr;
}

VkQueue * GfxVk::Utility::VkQueueFactory::GetQueue(const Core::Wrappers::QueueWrapper * req)
{
    GfxVk::Utility::VkQueueWrapper * vkWrapper = MapQueueWrapper(req);
    return vkWrapper->queue;
}

void GfxVk::Utility::VkQueueFactory::SetQueuePurpose(Core::Enums::QueueType* purpose, Core::Enums::PipelineType qType, const uint32_t & queueId)
{
    std::vector<GfxVk::Utility::VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case Core::Enums::PipelineType::GRAPHICS:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    case Core::Enums::PipelineType::COMPUTE:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    case Core::Enums::PipelineType::TRANSFER:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
}

uint32_t GfxVk::Utility::VkQueueFactory::GetQueueFamilyIndex(Core::Enums::PipelineType qType, uint32_t queueId)
{
    std::vector<GfxVk::Utility::VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case Core::Enums::PipelineType::GRAPHICS:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case Core::Enums::PipelineType::COMPUTE:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case Core::Enums::PipelineType::TRANSFER:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }

    return -1;
}

uint32_t GfxVk::Utility::VkQueueFactory::GetQueueFamilyIndex(VkQueueFlagBits qType, uint32_t queueId)
{
    std::vector<GfxVk::Utility::VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](GfxVk::Utility::VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
    return -1;
}

void GfxVk::Utility::VkQueueFactory::CreateGraphicsQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minGraphicQueueRequired, "Not enough graphics Queue");
    
    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(graphicQueueInitCounter <= minGraphicQueueRequired, "Graphics Queue exhausted ");

        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, graphicsQueueWrapperList[graphicQueueInitCounter].queueFamilyIndex,
            graphicsQueueWrapperList[graphicQueueInitCounter].indexInFamily, graphicsQueueWrapperList[graphicQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*graphicsQueueWrapperList[graphicQueueInitCounter].queue != VK_NULL_HANDLE, "Graphics Queue not available ");

        ids[i] = graphicsQueueWrapperList[graphicQueueInitCounter].queueId;
        graphicsQueueWrapperList[graphicQueueInitCounter].isQueueEnabled = true;
        graphicQueueInitCounter++;
    }
}

void GfxVk::Utility::VkQueueFactory::CreateComputeQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minCopmuteQueueRequired, "Not enough Compute Queue");
    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(computeQueueInitCounter <= minCopmuteQueueRequired, "Compute Queue exhausted ");

        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, computeQueueWrapperList[computeQueueInitCounter].queueFamilyIndex,
            computeQueueWrapperList[computeQueueInitCounter].indexInFamily, computeQueueWrapperList[computeQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*computeQueueWrapperList[computeQueueInitCounter].queue != VK_NULL_HANDLE, "Compute Queue not available ");

        ids[i] = computeQueueWrapperList[computeQueueInitCounter].queueId;
        computeQueueWrapperList[computeQueueInitCounter].isQueueEnabled = true;
        computeQueueInitCounter++;
    }
}

void GfxVk::Utility::VkQueueFactory::CreateTransferQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minTransferQueueRequired, "Not enough transfer Queue");

    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(transferQueueInitCounter <= minTransferQueueRequired, "Compute Queue exhausted ");

        vkGetDeviceQueue(*GfxVk::Utility::CoreObjects::logicalDeviceObj, transferQueueWrapperList[transferQueueInitCounter].queueFamilyIndex,
            transferQueueWrapperList[transferQueueInitCounter].indexInFamily, transferQueueWrapperList[transferQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*transferQueueWrapperList[transferQueueInitCounter].queue != VK_NULL_HANDLE, "Transfer Queue not available ");
        ids[i] = transferQueueWrapperList[transferQueueInitCounter].queueId;
        transferQueueWrapperList[transferQueueInitCounter].isQueueEnabled = true;
        transferQueueInitCounter++;
    }
}

void GfxVk::Utility::VkQueueFactory::SubmitQueue(const uint32_t & queueId, const Core::Enums::PipelineType* queueType, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
{
    ErrorCheck( vkQueueSubmit(*GetQueue(*queueType, queueId), submitCount, info, *fence));
}

void GfxVk::Utility::VkQueueFactory::SubmitQueue(const Core::Wrappers::QueueWrapper * req, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
{
    GfxVk::Utility::VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(req);
    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
}

//void GfxVk::Utility::VkQueueFactory::SubmitQueueForRendering(const Core::Wrappers::QueueWrapper * wrapper, const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
//{
//    GfxVk::Utility::VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(wrapper);
//    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
//}
//
//void GfxVk::Utility::VkQueueFactory::SubmitQueueForPresentation(const Core::Wrappers::QueueWrapper * wrapper, const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
//{
//    GfxVk::Utility::VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(wrapper);
//    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
//}

void GfxVk::Utility::VkQueueFactory::WaitForAllQueues()
{
    //TODO : Expose queue, DONE
    //vkQueueWaitIdle(*GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::renderQueueId));
    //vkQueueWaitIdle(*GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::presentationQueuedId));

    for each(GfxVk::Utility::VkQueueWrapper wrapper in transferQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

    for each(GfxVk::Utility::VkQueueWrapper wrapper in computeQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

    for each(GfxVk::Utility::VkQueueWrapper wrapper in graphicsQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

}


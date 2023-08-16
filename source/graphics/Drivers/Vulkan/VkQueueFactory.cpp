#include "VkQueueFactory.h"
#include "VulkanUtility.h"
#include <CorePrecompiled.h>

VkQueueFactory* VkQueueFactory::instance = nullptr;

VkQueueWrapper * VkQueueFactory::MapQueueWrapper(const QueueWrapper * wrapper)
{
    if (*wrapper->queueType == QueueType::GRAPHICS)
        for (uint32_t i = 0; i < graphicsQueueWrapperList.size(); i++)
        {
            if (wrapper->queueId == graphicsQueueWrapperList[i].queueId && wrapper->purpose == graphicsQueueWrapperList[i].purpose)
            {
                return &graphicsQueueWrapperList[i];
            }
        }

    if (*wrapper->queueType == QueueType::COMPUTE)
        for (uint32_t i = 0; i < computeQueueWrapperList.size(); i++)
        {
            if (wrapper->queueId == computeQueueWrapperList[i].queueId && wrapper->purpose == computeQueueWrapperList[i].purpose)
            {
                return &computeQueueWrapperList[i];
            }
        }

    if (*wrapper->queueType == QueueType::TRANSFER)
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

void VkQueueFactory::Init()
{
    PLOGD << "VkQueueFactory Init";

    uint32_t graphicsReq = VK_QUEUE_GRAPHICS_BIT;
    uint32_t computeReq = VK_QUEUE_COMPUTE_BIT;
    uint32_t transferReq = VK_QUEUE_TRANSFER_BIT;

    uint32_t qFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*CoreObjects::physicalDeviceObj, &qFamilyCount, nullptr);
    propertyList.resize(qFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(*CoreObjects::physicalDeviceObj, &qFamilyCount, propertyList.data());

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

void VkQueueFactory::DeInit()
{
    PLOGD << "VkQueueFactory DeInit";

    delete[] graphicQueuePriority;

    if (computeQueuePriority != nullptr)
    delete[] computeQueuePriority;

    if(transferQueuePriority != nullptr)
    delete[] transferQueuePriority;

    for each(VkQueueWrapper obj in graphicsQueueWrapperList)
    {
        //if (obj.isQueueEnabled)
        {
            delete obj.queue;
        }
    }

    for each(VkQueueWrapper obj in computeQueueWrapperList)
    {
        //if (obj.isQueueEnabled)
        {
            delete obj.queue;
        }
    }

    for each(VkQueueWrapper obj in transferQueueWrapperList)
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

void VkQueueFactory::Update()
{

}

VkQueueFactory * VkQueueFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkQueueFactory();
    }
    return instance;
}

VkQueueFactory::~VkQueueFactory()
{
    
}

std::vector<VkDeviceQueueCreateInfo> VkQueueFactory::FindQueue()
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
        graphicsQueueWrapperList[i].queueType = QueueType::GRAPHICS;
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
        computeQueueWrapperList[i].queueType = QueueType::COMPUTE;
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
        transferQueueWrapperList[i].queueType = QueueType::TRANSFER;
        transferQueueWrapperList[i].indexInFamily = indexInTransferFamily;
        transferQueueWrapperList[i].queueId = GetQueueId();
        transferQueueWrapperList[i].queue = new VkQueue;
        indexInTransferFamily++;
    }
    
    return creatInfoList; 
}

void VkQueueFactory::InitQueues()
{
    for (uint32_t i = 0; i < minGraphicQueueRequired; i++)
    {
        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, graphicsQueueWrapperList[i].queueFamilyIndex,
            graphicsQueueWrapperList[i].indexInFamily, graphicsQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*graphicsQueueWrapperList[i].queue != VK_NULL_HANDLE, "Graphics Queue not available ");
    }

    for (uint32_t i = 0; i < minCopmuteQueueRequired; i++)
    {
        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, computeQueueWrapperList[i].queueFamilyIndex,
            computeQueueWrapperList[i].indexInFamily, computeQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*computeQueueWrapperList[i].queue != VK_NULL_HANDLE, "Compute Queue not available ");
    }

    for (uint32_t i = 0; i < minTransferQueueRequired; i++)
    {
        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, transferQueueWrapperList[i].queueFamilyIndex, 
            transferQueueWrapperList[i].indexInFamily, transferQueueWrapperList[i].queue);
        ASSERT_MSG_DEBUG(*transferQueueWrapperList[i].queue != VK_NULL_HANDLE, "Transfer Queue not available ");
    }
}

VkQueue * VkQueueFactory::GetQueue(QueueType qType, const uint32_t & id)
{
    std::vector<VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case QueueType::GRAPHICS:
        {
            it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId== id; });
            ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    case QueueType::COMPUTE:
        {
            it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == id; });
            ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    case QueueType::TRANSFER:
        {
            it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == id; });
            ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

            return it->queue;
        }
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }

    return nullptr;
}

VkQueue * VkQueueFactory::GetQueue(VkQueueFlagBits qType, const uint32_t & id)
{
    std::vector<VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == id; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queue;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
    return nullptr;
}

VkQueue * VkQueueFactory::GetQueue(const QueueWrapper * req)
{
    VkQueueWrapper * vkWrapper = MapQueueWrapper(req);
    return vkWrapper->queue;
}

void VkQueueFactory::SetQueuePurpose(QueuePurpose * purpose, QueueType qType, const uint32_t & queueId)
{
    std::vector<VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case QueueType::GRAPHICS:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    case QueueType::COMPUTE:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    case QueueType::TRANSFER:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        it->purpose = purpose;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
}

uint32_t VkQueueFactory::GetQueueFamilyIndex(QueueType qType, uint32_t queueId)
{
    std::vector<VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case QueueType::GRAPHICS:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case QueueType::COMPUTE:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case QueueType::TRANSFER:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }

    return -1;
}

uint32_t VkQueueFactory::GetQueueFamilyIndex(VkQueueFlagBits qType, uint32_t queueId)
{
    std::vector<VkQueueWrapper>::iterator it;

    switch (qType)
    {
    case VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT:
    {
        it = std::find_if(graphicsQueueWrapperList.begin(), graphicsQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != graphicsQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT:
    {
        it = std::find_if(computeQueueWrapperList.begin(), computeQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != computeQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    case VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT:
    {
        it = std::find_if(transferQueueWrapperList.begin(), transferQueueWrapperList.end(), [&](VkQueueWrapper e) { return e.queueId == queueId; });
        ASSERT_MSG_DEBUG(it != transferQueueWrapperList.end(), "Queue family index not found");

        return it->queueFamilyIndex;
    }
    break;

    default:
        ASSERT_MSG_DEBUG(0, "Queue not found");
    }
    return -1;
}

void VkQueueFactory::CreateGraphicsQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minGraphicQueueRequired, "Not enough graphics Queue");
    
    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(graphicQueueInitCounter <= minGraphicQueueRequired, "Graphics Queue exhausted ");

        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, graphicsQueueWrapperList[graphicQueueInitCounter].queueFamilyIndex,
            graphicsQueueWrapperList[graphicQueueInitCounter].indexInFamily, graphicsQueueWrapperList[graphicQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*graphicsQueueWrapperList[graphicQueueInitCounter].queue != VK_NULL_HANDLE, "Graphics Queue not available ");

        ids[i] = graphicsQueueWrapperList[graphicQueueInitCounter].queueId;
        graphicsQueueWrapperList[graphicQueueInitCounter].isQueueEnabled = true;
        graphicQueueInitCounter++;
    }
}

void VkQueueFactory::CreateComputeQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minCopmuteQueueRequired, "Not enough Compute Queue");
    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(computeQueueInitCounter <= minCopmuteQueueRequired, "Compute Queue exhausted ");

        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, computeQueueWrapperList[computeQueueInitCounter].queueFamilyIndex,
            computeQueueWrapperList[computeQueueInitCounter].indexInFamily, computeQueueWrapperList[computeQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*computeQueueWrapperList[computeQueueInitCounter].queue != VK_NULL_HANDLE, "Compute Queue not available ");

        ids[i] = computeQueueWrapperList[computeQueueInitCounter].queueId;
        computeQueueWrapperList[computeQueueInitCounter].isQueueEnabled = true;
        computeQueueInitCounter++;
    }
}

void VkQueueFactory::CreateTransferQueues(uint32_t * ids, const uint32_t & count)
{
    ASSERT_MSG_DEBUG(count <= minTransferQueueRequired, "Not enough transfer Queue");

    for (uint32_t i = 0; i < count; i++)
    {
        ASSERT_MSG_DEBUG(transferQueueInitCounter <= minTransferQueueRequired, "Compute Queue exhausted ");

        vkGetDeviceQueue(*CoreObjects::logicalDeviceObj, transferQueueWrapperList[transferQueueInitCounter].queueFamilyIndex,
            transferQueueWrapperList[transferQueueInitCounter].indexInFamily, transferQueueWrapperList[transferQueueInitCounter].queue);
        ASSERT_MSG_DEBUG(*transferQueueWrapperList[transferQueueInitCounter].queue != VK_NULL_HANDLE, "Transfer Queue not available ");
        ids[i] = transferQueueWrapperList[transferQueueInitCounter].queueId;
        transferQueueWrapperList[transferQueueInitCounter].isQueueEnabled = true;
        transferQueueInitCounter++;
    }
}

void VkQueueFactory::SubmitQueue(const uint32_t & queueId, const QueueType * queueType, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
{
    ErrorCheck( vkQueueSubmit(*GetQueue(*queueType, queueId), submitCount, info, *fence));
}

void VkQueueFactory::SubmitQueue(const QueueWrapper * req, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
{
    VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(req);
    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
}

//void VkQueueFactory::SubmitQueueForRendering(const QueueWrapper * wrapper, const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
//{
//    VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(wrapper);
//    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
//}
//
//void VkQueueFactory::SubmitQueueForPresentation(const QueueWrapper * wrapper, const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence)
//{
//    VkQueueWrapper * vkQueueWrapperObj = MapQueueWrapper(wrapper);
//    ErrorCheck(vkQueueSubmit(*vkQueueWrapperObj->queue, submitCount, info, *fence));
//}

void VkQueueFactory::WaitForAllQueues()
{
    //TODO : Expose queue, DONE
    //vkQueueWaitIdle(*GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::renderQueueId));
    //vkQueueWaitIdle(*GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, CoreObjects::presentationQueuedId));

    for each(VkQueueWrapper wrapper in transferQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

    for each(VkQueueWrapper wrapper in computeQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

    for each(VkQueueWrapper wrapper in graphicsQueueWrapperList)
    {
        if (wrapper.isQueueEnabled)
        vkQueueWaitIdle(*wrapper.queue);
    }

}


#pragma once
#include <vulkan\vulkan.h>
#include <stdint.h>
#include <vector>
#include <RenderingWrapper.h>


enum class FamilyPermutatation
{
    //SAME_FAMILY,
    //SAME_QUEUE,
    SAME_FAMILY_SAME_QUEUE,
    SAME_FAMILY_UNIQUE_QUEUE,
    UNIQUE_QUEUES

};

typedef PipelineType QueueType;

class VkQueueWrapper
{
public:
    uint32_t queueFamilyIndex;
    uint32_t queueId;
    uint32_t indexInFamily;
    VkQueue * queue = NULL;
    QueueType queueType;
    QueuePurpose * purpose;
    bool isQueueEnabled = false;
};


class VkQueueFactory
{
private:
    VkQueueFactory(){}
    VkQueueFactory(VkQueueFactory const &) {}
    VkQueueFactory const & operator= (VkQueueFactory const &) {}

    static VkQueueFactory* instance;

    //std::vector<QueueWrapper> queueWrapperList;
    std::vector<VkQueueWrapper> graphicsQueueWrapperList, computeQueueWrapperList, transferQueueWrapperList;
    std::vector<VkQueueFamilyProperties> propertyList;
    //std::vector<uint32_t> uniqueIndiciesInFamily;
    const float* graphicQueuePriority, *computeQueuePriority, *transferQueuePriority;
    //uint32_t familyIndex = -1;

    uint32_t minGraphicQueueRequired = 2, minCopmuteQueueRequired = 2, minTransferQueueRequired = 1;
    uint32_t graphicsQueueFamilyIndex = -1, computeQueueFamilyIndex = -1, transferQueueFamilyIndex = -1;
    uint32_t graphicQueueInitCounter = 0, computeQueueInitCounter = 0, transferQueueInitCounter = 0;

    uint32_t queueIdCounter = 0;
    uint32_t GetQueueId() { return queueIdCounter++; }
    VkQueueWrapper * MapQueueWrapper(const QueueWrapper * wrapper);

public:
    void Init();
    void DeInit();
    void Update();
    static VkQueueFactory* GetInstance();
    ~VkQueueFactory();

    //void FindQueue(VkQueueFlagBits * queueFlag, uint32_t count, FamilyPermutatation perm, VkDeviceQueueCreateInfo * creatInfo, uint32_t * indexInFamily);
    //void InitQueues(uint32_t * indexInFamily, uint32_t count);
    std::vector<VkDeviceQueueCreateInfo> FindQueue();
    void InitQueues();

    VkQueue * GetQueue(QueueType qType, const uint32_t & id);
    VkQueue * GetQueue(VkQueueFlagBits qType, const uint32_t & id);
    VkQueue * GetQueue(const QueueWrapper * req);

    void SetQueuePurpose(QueuePurpose * purpose, QueueType qType, const uint32_t & id);

    uint32_t GetQueueFamilyIndex(QueueType qType, uint32_t queueId);
    //uint32_t GetQueueFamilyIndex(PipelineType qType, uint32_t queueId);
    uint32_t GetQueueFamilyIndex(VkQueueFlagBits qType, uint32_t queueId);
    uint32_t GetGraphicsQueueFamilyIndex() { return graphicsQueueFamilyIndex; }

    void CreateGraphicsQueues(uint32_t * ids, const uint32_t & count);
    void CreateComputeQueues(uint32_t * ids, const uint32_t & count);
    void CreateTransferQueues(uint32_t * ids, const uint32_t & count);

    void SubmitQueue(const uint32_t & queueId, const QueueType * queueType, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence);
    void SubmitQueue(const QueueWrapper * req, VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence);
    //void SubmitQueueForRendering(const QueueWrapper * req, const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence);
    //void SubmitQueueForPresentation(const QueueWrapper * req,const VkSubmitInfo * info, const uint32_t & submitCount, VkFence * fence);
    //void DestroyQueue(QueueType qType, uint32_t queueId);

    void WaitForAllQueues();
};

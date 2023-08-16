#include "VkDescriptorPoolFactory.h"
#include "VulkanUtility.h"
#include <algorithm>
#include <Assertion.h>

VkDescriptorPoolFactory* VkDescriptorPoolFactory::instance = nullptr;

uint32_t VkDescriptorPoolFactory::GetId()
{
    return idCounter++;
}

void VkDescriptorPoolFactory::Init()
{
    CreateDescritorPool();
}

void VkDescriptorPoolFactory::DeInit()
{
    for each(auto obj in poolWrapperList)
    {
        if (obj.poolSizeCount > 1)
            delete[] obj.poolSize;
        else if (obj.poolSizeCount == 1)
            delete obj.poolSize;

        vkDestroyDescriptorPool(*CoreObjects::logicalDeviceObj, *obj.pool, CoreObjects::pAllocator);
        delete obj.pool;
    }

    poolWrapperList.clear();

    for each(auto obj in descriptorSetList)
        delete obj;
    descriptorSetList.clear();
}

void VkDescriptorPoolFactory::Update()
{

}

VkDescriptorPoolFactory * VkDescriptorPoolFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkDescriptorPoolFactory();
    }
    return instance;
}

VkDescriptorPoolFactory::~VkDescriptorPoolFactory()
{
}

const uint32_t VkDescriptorPoolFactory::CreateDescritorPool()
{
    PoolWrapper poolWrapper = {};
    poolWrapper.poolId = GetId();

    poolWrapper.poolSizeCount = 6;

    VkDescriptorPoolSize * poolSize = new VkDescriptorPoolSize[poolWrapper.poolSizeCount];
    poolSize[0].descriptorCount = 50;
    poolSize[0].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    poolSize[1].descriptorCount = 100;
    poolSize[1].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    poolSize[2].descriptorCount = 50;
    poolSize[2].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

    poolSize[3].descriptorCount = 50;
    poolSize[3].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

    poolSize[4].descriptorCount = 50;
    poolSize[4].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

    poolSize[5].descriptorCount = 50;
    poolSize[5].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

    VkDescriptorPoolCreateInfo createInfo = {};
    //createInfo.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = MAX_SETS_PER_POOL;
    createInfo.pNext = nullptr;
    createInfo.poolSizeCount = 6;
    createInfo.pPoolSizes = poolSize;
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    poolWrapper.poolSize = poolSize;
    poolWrapper.pool = new VkDescriptorPool;

    ErrorCheck(vkCreateDescriptorPool(*CoreObjects::logicalDeviceObj, &createInfo, CoreObjects::pAllocator, poolWrapper.pool));
    
    poolWrapperList.push_back(poolWrapper);

    return poolWrapper.poolId;
}

VkDescriptorPool * VkDescriptorPoolFactory::GetDescriptorPool(const uint32_t & id)
{
    std::vector<PoolWrapper>::iterator it;

    it = std::find_if(poolWrapperList.begin(), poolWrapperList.end(), [&](PoolWrapper e) { return e.poolId == id; });

    ASSERT_MSG_DEBUG(it != poolWrapperList.end(), "Pool id not found");
    return it->pool;
}

VkDescriptorPool * VkDescriptorPoolFactory::GetDescriptorPool()
{
    return poolWrapperList[0].pool;
}

void VkDescriptorPoolFactory::StoreDescriptorSet(VkDescriptorSet * set)
{
    descriptorSetList.push_back(set);
}

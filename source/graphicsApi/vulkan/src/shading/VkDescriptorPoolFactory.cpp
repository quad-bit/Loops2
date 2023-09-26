#include "shading/VkDescriptorPoolFactory.h"
#include "utility/VulkanUtility.h"
#include <algorithm>
#include <Platform/Assertion.h>
#include <CorePrecompiled.h>

GfxVk::Shading::VkDescriptorPoolFactory* GfxVk::Shading::VkDescriptorPoolFactory::instance = nullptr;

uint32_t GfxVk::Shading::VkDescriptorPoolFactory::GetId()
{
    return idCounter++;
}

void GfxVk::Shading::VkDescriptorPoolFactory::Init()
{
    PLOGD << "VkDescriptorPoolFactory init";

    CreateDescritorPool();
}

void GfxVk::Shading::VkDescriptorPoolFactory::DeInit()
{
    PLOGD << "VkDescriptorPoolFactory DeInit";

    for each(auto obj in poolWrapperList)
    {
        if (obj.poolSizeCount > 1)
            delete[] obj.poolSize;
        else if (obj.poolSizeCount == 1)
            delete obj.poolSize;

        vkDestroyDescriptorPool(DeviceInfo::GetLogicalDevice(), *obj.pool, DeviceInfo::GetAllocationCallback());
        delete obj.pool;
    }

    poolWrapperList.clear();

    for each(auto obj in descriptorSetList)
        delete obj;
    descriptorSetList.clear();
}

void GfxVk::Shading::VkDescriptorPoolFactory::Update()
{

}

GfxVk::Shading::VkDescriptorPoolFactory * GfxVk::Shading::VkDescriptorPoolFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Shading::VkDescriptorPoolFactory();
    }
    return instance;
}

GfxVk::Shading::VkDescriptorPoolFactory::~VkDescriptorPoolFactory()
{
}

const uint32_t GfxVk::Shading::VkDescriptorPoolFactory::CreateDescritorPool()
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

    GfxVk::Utility::ErrorCheck(vkCreateDescriptorPool(DeviceInfo::GetLogicalDevice(), &createInfo, DeviceInfo::GetAllocationCallback(), poolWrapper.pool));
    
    poolWrapperList.push_back(poolWrapper);

    return poolWrapper.poolId;
}

VkDescriptorPool * GfxVk::Shading::VkDescriptorPoolFactory::GetDescriptorPool(const uint32_t & id)
{
    std::vector<PoolWrapper>::iterator it;

    it = std::find_if(poolWrapperList.begin(), poolWrapperList.end(), [&](PoolWrapper e) { return e.poolId == id; });

    ASSERT_MSG_DEBUG(it != poolWrapperList.end(), "Pool id not found");
    return it->pool;
}

VkDescriptorPool * GfxVk::Shading::VkDescriptorPoolFactory::GetDescriptorPool()
{
    return poolWrapperList[0].pool;
}

void GfxVk::Shading::VkDescriptorPoolFactory::StoreDescriptorSet(VkDescriptorSet * set)
{
    descriptorSetList.push_back(set);
}

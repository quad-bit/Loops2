#include "VkSamplerFactory.h"
#include "VkRenderingUnwrapper.h"
#include "VulkanUtility.h"
#include "VulkanSettings.h"

VkSamplerFactory* VkSamplerFactory::instance = nullptr;

uint32_t VkSamplerFactory::GenerateId()
{
    return idCounter++;
}

void VkSamplerFactory::Init()
{
    idCounter = 0;
}

void VkSamplerFactory::DeInit()
{
    for each(auto obj in idToSamplerMap)
    {
        vkDestroySampler(*CoreObjects::logicalDeviceObj, obj.second, CoreObjects::pAllocator);
    }

    idToSamplerMap.clear();
}

void VkSamplerFactory::Update()
{

}

VkSamplerFactory * VkSamplerFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkSamplerFactory();
    }
    return instance;
}

VkSamplerFactory::~VkSamplerFactory()
{
}

uint32_t VkSamplerFactory::CreateSampler(const SamplerCreateInfo & info)
{
    VkSamplerCreateInfo vkInfo = VulkanUnwrap::UnwrapSamplerCreateInfo(info);
    return CreateSampler(vkInfo);
}

uint32_t VkSamplerFactory::CreateSampler(const VkSamplerCreateInfo & info)
{
    uint32_t id = GenerateId();
    VkSampler sampler;
    ErrorCheck(vkCreateSampler(*CoreObjects::logicalDeviceObj, &info, CoreObjects::pAllocator, &sampler));

    idToSamplerMap.insert(std::pair<uint32_t, VkSampler>({id, sampler}));

    return id;
}

VkSampler * VkSamplerFactory::GetSampler(const uint32_t & id)
{
    auto it = idToSamplerMap.find(id);

    if (it == idToSamplerMap.end())
    {
        ASSERT_MSG_DEBUG(0, "sampler not found");
    }
    
    return &it->second;
}

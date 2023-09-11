#include "VkSamplerFactory.h"
#include "utility/VkRenderingUnwrapper.h"
#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"

GfxVk::Shading::VkSamplerFactory* GfxVk::Shading::VkSamplerFactory::instance = nullptr;

uint32_t GfxVk::Shading::VkSamplerFactory::GenerateId()
{
    return idCounter++;
}

void GfxVk::Shading::VkSamplerFactory::Init()
{
    idCounter = 0;
}

void GfxVk::Shading::VkSamplerFactory::DeInit()
{
    for each(auto obj in idToSamplerMap)
    {
        vkDestroySampler(DeviceInfo::GetLogicalDevice(), obj.second, DeviceInfo::GetAllocationCallback());
    }

    idToSamplerMap.clear();
}

void GfxVk::Shading::VkSamplerFactory::Update()
{

}

GfxVk::Shading::VkSamplerFactory * GfxVk::Shading::VkSamplerFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Shading::VkSamplerFactory();
    }
    return instance;
}

GfxVk::Shading::VkSamplerFactory::~VkSamplerFactory()
{
}

uint32_t GfxVk::Shading::VkSamplerFactory::CreateSampler(const Core::Wrappers::SamplerCreateInfo & info)
{
    VkSamplerCreateInfo vkInfo = Unwrap::UnwrapSamplerCreateInfo(info);
    return CreateSampler(vkInfo);
}

uint32_t GfxVk::Shading::VkSamplerFactory::CreateSampler(const VkSamplerCreateInfo & info)
{
    uint32_t id = GenerateId();
    VkSampler sampler;
    GfxVk::Utility::ErrorCheck(vkCreateSampler(DeviceInfo::GetLogicalDevice(), &info, DeviceInfo::GetAllocationCallback(), &sampler));

    idToSamplerMap.insert(std::pair<uint32_t, VkSampler>({id, sampler}));

    return id;
}

VkSampler * GfxVk::Shading::VkSamplerFactory::GetSampler(const uint32_t & id)
{
    auto it = idToSamplerMap.find(id);

    if (it == idToSamplerMap.end())
    {
        ASSERT_MSG_DEBUG(0, "sampler not found");
    }
    
    return &it->second;
}

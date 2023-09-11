#include "shading/VkShaderResourceAllocator.h"
#include "utility/VulkanUtility.h"
#include "shading/VkDescriptorPoolFactory.h" 
/*
void VkShaderResourceAllocator::CreateResourceSetting()
{
    std::vector<uint32_t> perSceneSets{ 0, 1 };    //"View", "Lights"
    std::vector<uint32_t> perMaterialSets{ 2, 3 };   //"Sampler", "DiffuseTexture"
    std::vector<uint32_t> perMaterialInstanceSets{ 4, 5 };   //"Transform"

    resourceSetClassifyingMap.insert(std::pair < ShaderResourceClassification, std::vector<uint32_t>>{
        ShaderResourceClassification::PER_SCENE, perSceneSets
    });

    resourceSetClassifyingMap.insert(std::pair < ShaderResourceClassification, std::vector<uint32_t>>{
        ShaderResourceClassification::PER_MATERIAL, perMaterialSets
    });

    resourceSetClassifyingMap.insert(std::pair < ShaderResourceClassification, std::vector<uint32_t>>{
        ShaderResourceClassification::PER_MATERIAL_INSTANCE, perMaterialInstanceSets
    });
}

VkShaderResourceAllocator::VkShaderResourceAllocator()
{
}

VkShaderResourceAllocator::~VkShaderResourceAllocator()
{
}

VkDescriptorSet * VkShaderResourceAllocator::AllocateDescriptor(SetWrapper * setWrapper, VkDescriptorSetLayout * layout)
{
    VkDescriptorSet * set = new VkDescriptorSet;

    VkDescriptorSetAllocateInfo info = {};
    info.descriptorPool = *VkDescriptorPoolFactory::GetInstance()->GetDescriptorPool();
    info.descriptorSetCount = 1;
    info.pNext = nullptr;
    info.pSetLayouts = layout;
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    ErrorCheck(vkAllocateDescriptorSets(DeviceInfo::GetLogicalDevice(), &info, set));
    VkDescriptorPoolFactory::GetInstance()->StoreDescriptorSet(set);
    
    return set;
}

VkDescriptorSet ** VkShaderResourceAllocator::AllocateDescriptor(SetWrapper ** setWrapper, VkDescriptorSetLayout ** layout)
{
    return nullptr;
}
*/
#include "resourceManagement/UniformFactory.h"
#include <algorithm>
#include <Platform/Assertion.h>
#include "VulkanInterface.h"

Renderer::ResourceManagement::UniformFactory* Renderer::ResourceManagement::UniformFactory::instance = nullptr;
void Renderer::ResourceManagement::UniformFactory::HandleUniformBuffer(Core::Utility::BufferBindingInfo& bufferInfo)
{
    Core::Enums::MemoryType mem[2]{ Core::Enums::MemoryType::HOST_VISIBLE_BIT, Core::Enums::MemoryType::HOST_COHERENT_BIT };
    Core::Wrappers::BufferCreateInfo info{};
    info.size = bufferInfo.info.totalSize;
    info.usage.push_back(Core::Enums::BufferUsage::BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    uint32_t id = VulkanInterfaceAlias::CreateBuffer(info, "UniformBuffer");
    bufferInfo.bufferIdList.push_back(id);
    bufferIds.push_back(id);

    uint32_t memId = VulkanInterfaceAlias::AllocateAndBindBufferMemory(id, mem, 2, false, std::nullopt);
    bufferInfo.bufferMemoryId.push_back(memId);
    memoryIds.push_back(memId);
}

void Renderer::ResourceManagement::UniformFactory::Init()
{
    setWrapperList = VulkanInterfaceAlias::GetSetWrapperList();
    m_perSetMap = VulkanInterfaceAlias::GetPerSetSetwrapperList();
}

void Renderer::ResourceManagement::UniformFactory::DeInit()
{
    setConfig.clear();

    VulkanInterfaceAlias::DestroyBuffer(bufferIds.data(),(uint32_t)bufferIds.size(), false);
    VulkanInterfaceAlias::FreeMemory(memoryIds.data(), (uint32_t)memoryIds.size());
}

void Renderer::ResourceManagement::UniformFactory::Update()
{

}

Renderer::ResourceManagement::UniformFactory * Renderer::ResourceManagement::UniformFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ResourceManagement::UniformFactory();
    }
    return instance;
}

Renderer::ResourceManagement::UniformFactory::~UniformFactory()
{
}
void Renderer::ResourceManagement::UniformFactory::UploadDataToBuffers(const uint32_t & bufId, const size_t & dataSize, const size_t & memAlignedSize, void * data, const size_t & memoryOffset, bool keepMemoryMounted)
{
    VulkanInterfaceAlias::CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

void Renderer::ResourceManagement::UniformFactory::AllocateDescriptorSet(Core::Wrappers::SetWrapper * wrapper, Core::Utility::ShaderBindingDescription * desc, const uint32_t & numBindings, const uint32_t & numDescriptorsPerBinding)
{
    //// Allocate descriptor sets
    //uint32_t * descriptorIds = VulkanInterfaceAlias::AllocateDescriptorsSet(wrapper, numDescriptorsPerBinding);

    //for (uint32_t i = 0; i < numBindings; i++)
    //{
    //    for (uint32_t k = 0; k < numDescriptorsPerBinding; k++)
    //        desc[i].descriptorSetIds.push_back(descriptorIds[k]);

    //}
    //// update the descriptors
    //VulkanInterfaceAlias::LinkSetBindingToResources(desc, numBindings);
}

Core::Wrappers::SetWrapper* Renderer::ResourceManagement::UniformFactory::AllocateSetResources(const Core::Utility::DescriptorSetInfo& setDescription)
{
    Core::Wrappers::SetWrapper* setWrapper = GetSetWrapper(setDescription);

    for (uint32_t i = 0; i < setDescription.m_numBindings; i++)
    {
        Core::Enums::DescriptorType resourceType = setDescription.m_setBindings[i].m_resourceType;
        {
            switch (resourceType)
            {
            case Core::Enums::DescriptorType::SAMPLER:
                break;
            case Core::Enums::DescriptorType::COMBINED_IMAGE_SAMPLER:
                break;
            case Core::Enums::DescriptorType::SAMPLED_IMAGE:
                break;
            case Core::Enums::DescriptorType::STORAGE_IMAGE:
                break;
            case Core::Enums::DescriptorType::UNIFORM_TEXEL_BUFFER:
                break;
            case Core::Enums::DescriptorType::STORAGE_TEXEL_BUFFER:
                break;
            case Core::Enums::DescriptorType::UNIFORM_BUFFER:
            {
                HandleUniformBuffer((Core::Utility::BufferBindingInfo&)std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[i].m_bindingInfo));
                break;
            }
            case Core::Enums::DescriptorType::STORAGE_BUFFER:
                break;
            case Core::Enums::DescriptorType::UNIFORM_BUFFER_DYNAMIC:
                break;
            case Core::Enums::DescriptorType::STORAGE_BUFFER_DYNAMIC:
                break;
            case Core::Enums::DescriptorType::INPUT_ATTACHMENT:
                break;
            }
        }
    }

    return setWrapper;
}

void Renderer::ResourceManagement::UniformFactory::AllocateDescriptorSets(Core::Wrappers::SetWrapper* wrapper, Core::Utility::DescriptorSetInfo& setDescription, const uint32_t& numDescriptorSets)
{
    // Allocate descriptor sets
    uint32_t* descriptorIds = VulkanInterfaceAlias::AllocateDescriptorsSet(wrapper, numDescriptorSets);

    for (uint32_t i = 0; i < numDescriptorSets; i++)
    {
        setDescription.m_descriptorSetIds.push_back(descriptorIds[i]);
    }

    // update the descriptors
    VulkanInterfaceAlias::LinkSetBindingToResources(setDescription, numDescriptorSets);
}

Core::Wrappers::SetWrapper* Renderer::ResourceManagement::UniformFactory::GetSetWrapper(const Core::Utility::DescriptorSetInfo& setDescription)
{
    Core::Wrappers::SetWrapper* wrapper = nullptr;

    auto IterateWrappers = [&](const std::vector<Core::Wrappers::SetWrapper*>& wrapperList) -> Core::Wrappers::SetWrapper*
        {
            //std::vector<Core::Wrappers::SetWrapper*>::iterator it;
            const auto it = std::find_if(wrapperList.begin(), wrapperList.end(), [&](Core::Wrappers::SetWrapper* e)
                {
                    // Check if the set value and num of bindings are same
                    if (e->setValue == setDescription.m_setNumber && setDescription.m_numBindings == e->bindingWrapperList.size())
                    {
                        for (uint32_t i = 0; i < setDescription.m_numBindings; i++)
                        {
                            if (e->bindingWrapperList[i].bindingObj.descriptorType != setDescription.m_setBindings[i].m_resourceType)
                                break;

                            if (e->bindingWrapperList[i].bindingObj.binding != setDescription.m_setBindings[i].m_bindingNumber)
                                break;

                            if (e->bindingWrapperList[i].memberList.size() != setDescription.m_setBindings[i].m_numElements)
                                break;

                            if (e->bindingWrapperList[i].bindingName != setDescription.m_setBindings[i].m_bindingName)
                                break;

                            return true;
                        }
                    }
                    return false;
                });

            ASSERT_MSG_DEBUG(it != wrapperList.end(), "Set not found");
            return *it;
        };

    auto FindSetWrapper = [&setDescription, this, &IterateWrappers]() -> Core::Wrappers::SetWrapper*
        {
            Core::Wrappers::SetWrapper* setWrapper = nullptr;
            const auto it = m_perSetMap->find(setDescription.m_setNumber);
            if (it != m_perSetMap->end())
            {
                setWrapper = IterateWrappers(it->second);
            }
            else
            {
                ASSERT_MSG_DEBUG(0, "Set not found");
            }
            return setWrapper;
        };

    return FindSetWrapper();
}

std::vector<uint32_t> Renderer::ResourceManagement::UniformFactory::AcquireMeshList(Core::Wrappers::SetWrapper * wrapper)
{
    ASSERT_MSG(0, "Not implemented");
    return std::vector<uint32_t>();
}

size_t Renderer::ResourceManagement::UniformFactory::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
}

uint32_t Renderer::ResourceManagement::UniformFactory::CreateSampler(const Core::Wrappers::SamplerCreateInfo & info)
{
    return VulkanInterfaceAlias::CreateSampler(info);
}

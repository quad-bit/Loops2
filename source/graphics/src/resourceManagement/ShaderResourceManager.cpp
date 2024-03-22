#include "resourceManagement/ShaderResourceManager.h"
#include <algorithm>
#include <Platform/Assertion.h>
#include "VulkanInterface.h"

Renderer::ResourceManagement::ShaderResourceManager* Renderer::ResourceManagement::ShaderResourceManager::instance = nullptr;
void Renderer::ResourceManagement::ShaderResourceManager::HandleUniformBuffer(Core::Utility::BufferBindingInfo& bufferInfo)
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

void Renderer::ResourceManagement::ShaderResourceManager::Init()
{
    setWrapperList = VulkanInterfaceAlias::GetSetWrapperList();
    m_perSetMap = VulkanInterfaceAlias::GetPerSetSetwrapperList();
}

void Renderer::ResourceManagement::ShaderResourceManager::DeInit()
{
    setConfig.clear();

    VulkanInterfaceAlias::DestroyBuffer(bufferIds.data(),(uint32_t)bufferIds.size(), false);
    VulkanInterfaceAlias::FreeMemory(memoryIds.data(), (uint32_t)memoryIds.size());
}

void Renderer::ResourceManagement::ShaderResourceManager::Update()
{

}

Renderer::ResourceManagement::ShaderResourceManager * Renderer::ResourceManagement::ShaderResourceManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ResourceManagement::ShaderResourceManager();
    }
    return instance;
}

Renderer::ResourceManagement::ShaderResourceManager::~ShaderResourceManager()
{
}
void Renderer::ResourceManagement::ShaderResourceManager::UploadDataToBuffers(const uint32_t & bufId, const size_t & dataSize, const size_t & memAlignedSize, void * data, const size_t & memoryOffset, bool keepMemoryMounted)
{
    VulkanInterfaceAlias::CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

Core::Wrappers::SetWrapper* Renderer::ResourceManagement::ShaderResourceManager::AllocateSetResources(const Core::Utility::DescriptorSetInfo& setDescription)
{
    Core::Wrappers::SetWrapper* setWrapper = GetSetWrapper(setDescription);

    for (uint32_t i = 0; i < setDescription.m_numBindings; i++)
    {
        Core::Enums::DescriptorType resourceType = setDescription.m_setBindings[i].m_resourceType;
        {
            switch (resourceType)
            {
            case Core::Enums::DescriptorType::SAMPLER:
            case Core::Enums::DescriptorType::COMBINED_IMAGE_SAMPLER:
            case Core::Enums::DescriptorType::SAMPLED_IMAGE:
            case Core::Enums::DescriptorType::STORAGE_IMAGE:
            case Core::Enums::DescriptorType::UNIFORM_TEXEL_BUFFER:
            case Core::Enums::DescriptorType::STORAGE_TEXEL_BUFFER:
            case Core::Enums::DescriptorType::STORAGE_BUFFER:
            case Core::Enums::DescriptorType::UNIFORM_BUFFER_DYNAMIC:
            case Core::Enums::DescriptorType::STORAGE_BUFFER_DYNAMIC:
            case Core::Enums::DescriptorType::INPUT_ATTACHMENT:
                ASSERT_MSG_DEBUG(0, "Not yet implemented");
                break;
            case Core::Enums::DescriptorType::UNIFORM_BUFFER:
                HandleUniformBuffer((Core::Utility::BufferBindingInfo&)std::get<Core::Utility::BufferBindingInfo>(setDescription.m_setBindings[i].m_bindingInfo));
                break;
            }
        }
    }

    return setWrapper;
}

void Renderer::ResourceManagement::ShaderResourceManager::AllocateDescriptorSets(Core::Wrappers::SetWrapper* wrapper, Core::Utility::DescriptorSetInfo& setDescription, const uint32_t& numDescriptorSets)
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

Core::Wrappers::SetWrapper* Renderer::ResourceManagement::ShaderResourceManager::GetSetWrapper(const Core::Utility::DescriptorSetInfo& setDescription)
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
                        uint32_t counter = 0;
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

                            counter++;
                        }
                        if (counter == setDescription.m_numBindings)
                            return true;
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

size_t Renderer::ResourceManagement::ShaderResourceManager::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
}

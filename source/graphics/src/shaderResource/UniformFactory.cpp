#include "shaderResource/UniformFactory.h"
#include <algorithm>
#include <Platform/Assertion.h>
#include "VulkanInterface.h"

Renderer::ShaderResource::UniformFactory* Renderer::ShaderResource::UniformFactory::instance = nullptr;


void Renderer::ShaderResource::UniformFactory::HandleUniformBuffer(Core::Utility::ShaderBindingDescription * desc, Core::Wrappers::SetWrapper * setWrapper, Core::Wrappers::BindingWrapper bindingObj, size_t allocationSize)
{
    Core::Enums::BufferType type { Core::Enums::BufferType::UNIFORM_BUFFER_BIT };
    Core::Enums::MemoryType mem[2] { Core::Enums::MemoryType::HOST_VISIBLE_BIT, Core::Enums::MemoryType::HOST_COHERENT_BIT};

    // TODO : remove the hardcoded values and automate it.

    // Create a new buffer
    Core::Wrappers::BufferInfo info = {};
    info.dataSize = allocationSize;
    info.bufType = &type;
    info.memType = mem;
    info.memTypeCount = 2;

    // num bufs should be deduced from alloc info

    const uint32_t numBuffers = 1;
    uint32_t * id =  VulkanInterfaceAlias::CreateBuffers(&info, numBuffers);
    //desc->resourceId = *id;
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        desc->bufferBindingInfo.bufferIdList[i] = id[i];
        bufferIds.push_back(id[i]);
    }

    // allocate buffer memory
    // num mem should be deduced from alloc info
    // TODO : should be deprecated,this is horrible
    uint32_t * memId = VulkanInterfaceAlias::AllocateBufferMemory(id, numBuffers);
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        desc->bufferBindingInfo.bufferMemoryId[i] = memId[i];
        memoryIds.push_back(memId[i]);
    }

    delete[] id;
    delete[] memId;
}

void Renderer::ShaderResource::UniformFactory::HandleUniformBuffer(Core::Utility::BufferBindingInfo& bufferInfo)
{
    Core::Enums::BufferType type{ Core::Enums::BufferType::UNIFORM_BUFFER_BIT };
    Core::Enums::MemoryType mem[2]{ Core::Enums::MemoryType::HOST_VISIBLE_BIT, Core::Enums::MemoryType::HOST_COHERENT_BIT };

    // Create a new buffer
    Core::Wrappers::BufferInfo info = {};
    info.dataSize = bufferInfo.info.totalSize;
    info.bufType = &type;
    info.memType = mem;
    info.memTypeCount = 2;

    const uint32_t numBuffers = 1;
    uint32_t* id = VulkanInterfaceAlias::CreateBuffers(&info, numBuffers);

    for (uint32_t i = 0; i < numBuffers; i++)
    {
        bufferInfo.bufferIdList.push_back(id[i]);
        bufferIds.push_back(id[i]);
    }

    // allocate buffer memory
    // num mem should be deduced from alloc info
    // TODO : should be deprecated,this is horrible
    uint32_t* memId = VulkanInterfaceAlias::AllocateBufferMemory(id, numBuffers);
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        bufferInfo.bufferMemoryId.push_back(memId[i]);
        memoryIds.push_back(memId[i]);
    }

    delete[] id;
    delete[] memId;
}

void Renderer::ShaderResource::UniformFactory::Init()
{
    setWrapperList = VulkanInterfaceAlias::GetSetWrapperList();
    m_perSetMap = VulkanInterfaceAlias::GetPerSetSetwrapperList();
}

void Renderer::ShaderResource::UniformFactory::DeInit()
{
    setConfig.clear();

    VulkanInterfaceAlias::DestroyBuffer(bufferIds.data(),(uint32_t) bufferIds.size());
    VulkanInterfaceAlias::FreeMemory(memoryIds.data(), (uint32_t)memoryIds.size());
}

void Renderer::ShaderResource::UniformFactory::Update()
{

}

Renderer::ShaderResource::UniformFactory * Renderer::ShaderResource::UniformFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ShaderResource::UniformFactory();
    }
    return instance;
}

Renderer::ShaderResource::UniformFactory::~UniformFactory()
{
}

// allocates resources for entire set, all bindings.
//Core::Wrappers::SetWrapper * Renderer::ShaderResource::UniformFactory::AllocateSetResource(Core::Utility::ShaderBindingDescription * desc, size_t * allocationSize, const uint32_t & numBindings, Core::Utility::AllocationMethod allocation)
//{
//    Core::Wrappers::SetWrapper * setWrapper = GetSetWrapper(desc, numBindings);
//
//    for (uint32_t i = 0; i < numBindings; i++)
//    {
//        Core::Wrappers::BindingWrapper binding = setWrapper->bindingWrapperList[i];
//        if (binding.bindingName == desc[i].resourceName && binding.bindingObj.binding == desc[i].binding &&
//            binding.memberList.size() == desc[i].numElements)
//        {
//            switch (binding.bindingObj.descriptorType)
//            {
//            case Core::Enums::DescriptorType::SAMPLER:
//                break;
//            case Core::Enums::DescriptorType::COMBINED_IMAGE_SAMPLER:
//                break;
//            case Core::Enums::DescriptorType::SAMPLED_IMAGE:
//                break;
//            case Core::Enums::DescriptorType::STORAGE_IMAGE:
//                break;
//            case Core::Enums::DescriptorType::UNIFORM_TEXEL_BUFFER:
//                break;
//            case Core::Enums::DescriptorType::STORAGE_TEXEL_BUFFER:
//                break;
//            case Core::Enums::DescriptorType::UNIFORM_BUFFER:
//                HandleUniformBuffer(&desc[i], setWrapper, binding, allocationSize[i]);
//                break;
//            case Core::Enums::DescriptorType::STORAGE_BUFFER:
//                break;
//            case Core::Enums::DescriptorType::UNIFORM_BUFFER_DYNAMIC:
//                break;
//            case Core::Enums::DescriptorType::STORAGE_BUFFER_DYNAMIC:
//                break;
//            case Core::Enums::DescriptorType::INPUT_ATTACHMENT:
//                break;
//            }
//        }
//        else
//        {
//            ASSERT_MSG_DEBUG(0, "Binding not found");
//        }
//    }
//
//    //AllocateDescriptors(*it, desc, numBindings);
//    return setWrapper;
//}

void Renderer::ShaderResource::UniformFactory::AllocateBindingResources(Core::Wrappers::SetWrapper * setWrapper, Core::Utility::ShaderBindingDescription * desc, size_t allocationSize, const uint32_t & bindingIndexInSetwrapper, Core::Utility::AllocationMethod allocation)
{
    //for (uint32_t i = 0; i < numBindings; i++)
    {
        Core::Wrappers::BindingWrapper binding = setWrapper->bindingWrapperList[bindingIndexInSetwrapper];
        //if (binding.bindingName == desc[i].resourceName && binding.bindingObj.binding == desc[i].binding &&
            //binding.memberList.size() == desc[i].numElements)
        {
            switch (binding.bindingObj.descriptorType)
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
                HandleUniformBuffer(desc, setWrapper, binding, allocationSize);
                break;
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
        /*else
        {
            ASSERT_MSG_DEBUG(0, "Binding not found");
        }*/
    }
}

void Renderer::ShaderResource::UniformFactory::UploadDataToBuffers(const uint32_t & bufId, const size_t & dataSize, const size_t & memAlignedSize, void * data, const size_t & memoryOffset, bool keepMemoryMounted)
{
    VulkanInterfaceAlias::CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

void Renderer::ShaderResource::UniformFactory::AllocateDescriptorSet(Core::Wrappers::SetWrapper * wrapper, Core::Utility::ShaderBindingDescription * desc, const uint32_t & numBindings, const uint32_t & numDescriptorsPerBinding)
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

Core::Wrappers::SetWrapper* Renderer::ShaderResource::UniformFactory::AllocateSetResources(const Core::Utility::DescriptorSetDescription& setDescription)
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

void Renderer::ShaderResource::UniformFactory::AllocateDescriptorSets(Core::Wrappers::SetWrapper* wrapper, Core::Utility::DescriptorSetDescription& setDescription, const uint32_t& numDescriptorSets)
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

Core::Wrappers::SetWrapper* Renderer::ShaderResource::UniformFactory::GetSetWrapper(const Core::Utility::DescriptorSetDescription& setDescription)
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
                                return false;

                            if (e->bindingWrapperList[i].bindingObj.binding != setDescription.m_setBindings[i].m_bindingNumber)
                                return false;

                            if (e->bindingWrapperList[i].memberList.size() != setDescription.m_setBindings[i].m_numElements)
                                return false;

                            if (e->bindingWrapperList[i].bindingName != setDescription.m_setBindings[i].m_bindingName)
                                return false;
                        }
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

std::vector<uint32_t> Renderer::ShaderResource::UniformFactory::AcquireMeshList(Core::Wrappers::SetWrapper * wrapper)
{
    ASSERT_MSG(0, "Not implemented");
    return std::vector<uint32_t>();
}

size_t Renderer::ShaderResource::UniformFactory::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return VulkanInterfaceAlias::GetMemoryAlignedDataSizeForBuffer(dataSize);
}

void Renderer::ShaderResource::UniformFactory::AllocateUniformBuffer(Core::Wrappers::BufferCreateInfo * info, const uint32_t & numBuffers, uint32_t * out_buffIds, size_t * out_bufferMemRequirements)
{
    VulkanInterfaceAlias::CreateBuffers(info, numBuffers, out_buffIds, out_bufferMemRequirements);
}

void Renderer::ShaderResource::UniformFactory::CreateSetLayout(Core::Utility::ShaderBindingDescription * desc, const uint32_t & numBindings)
{
    // Create set Layout bindings

}

uint32_t Renderer::ShaderResource::UniformFactory::CreateSampler(const Core::Wrappers::SamplerCreateInfo & info)
{
    return VulkanInterfaceAlias::CreateSampler(info);
}

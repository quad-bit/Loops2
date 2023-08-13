#include "UniformFactory.h"
#include <algorithm>
#include <Assertion.h>
#include "RenderingWrapper.h"

#if (RENDERING_API == VULKAN)
#include <VulkanInterface.h>
#elif (RENDERING_API == DX)
#include <DxInterface.h>
#endif

UniformFactory* UniformFactory::instance = nullptr;


void UniformFactory::HandleUniformBuffer(ShaderBindingDescription * desc, SetWrapper * setWrapper, BindingWrapper bindingObj, size_t allocationSize)
{
    BufferType type { BufferType::UNIFORM_BUFFER_BIT };
    MemoryType mem[2] { MemoryType::HOST_VISIBLE_BIT, MemoryType::HOST_COHERENT_BIT};

    // TODO : remove the hardcoded values and automate it.

    // Create a new buffer
    BufferInfo info = {};
    info.dataSize = allocationSize;
    info.bufType = &type;
    info.memType = mem;
    info.memTypeCount = 2;

    // num bufs should be deduced from alloc info

    const uint32_t numBuffers = 1;
    uint32_t * id = apiInterface->CreateBuffers(&info, numBuffers);
    //desc->resourceId = *id;
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        desc->bufferBindingInfo.bufferIdList[i] = id[i];
        bufferIds.push_back(id[i]);
    }

    // allocate buffer memory
    // num mem should be deduced from alloc info
    // TODO : should be deprecated,this is horrible
    uint32_t * memId = apiInterface->AllocateBufferMemory(id, numBuffers);
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        desc->bufferBindingInfo.bufferMemoryId[i] = memId[i];
        memoryIds.push_back(memId[i]);
    }

    delete[] id;
    delete[] memId;
}

void UniformFactory::Init(ApiInterface * api)
{
    this->apiInterface = api;
    setWrapperList = this->apiInterface->GetSetWrapperList();
}

void UniformFactory::DeInit()
{
    setConfig.clear();

    {
        apiInterface->DestroyBuffer(bufferIds.data(),(uint32_t) bufferIds.size());
    }

    {
        apiInterface->FreeMemory(memoryIds.data(), (uint32_t)memoryIds.size());
    }
}

void UniformFactory::Update()
{

}

UniformFactory * UniformFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new UniformFactory();
    }
    return instance;
}

UniformFactory::~UniformFactory()
{
}

// allocates resources for entire set, all bindings.
SetWrapper * UniformFactory::AllocateSetResource(ShaderBindingDescription * desc, size_t * allocationSize, const uint32_t & numBindings, AllocationMethod allocation)
{
    SetWrapper * setWrapper = GetSetWrapper(desc, numBindings);

    for (uint32_t i = 0; i < numBindings; i++)
    {
        BindingWrapper binding = setWrapper->bindingWrapperList[i];
        if (binding.bindingName == desc[i].resourceName && binding.bindingObj.binding == desc[i].binding &&
            binding.memberList.size() == desc[i].numElements)
        {
            switch (binding.bindingObj.descriptorType)
            {
            case DescriptorType::SAMPLER:
                break;
            case DescriptorType::COMBINED_IMAGE_SAMPLER:
                break;
            case DescriptorType::SAMPLED_IMAGE:
                break;
            case DescriptorType::STORAGE_IMAGE:
                break;
            case DescriptorType::UNIFORM_TEXEL_BUFFER:
                break;
            case DescriptorType::STORAGE_TEXEL_BUFFER:
                break;
            case DescriptorType::UNIFORM_BUFFER:
                HandleUniformBuffer(&desc[i], setWrapper, binding, allocationSize[i]);
                break;
            case DescriptorType::STORAGE_BUFFER:
                break;
            case DescriptorType::UNIFORM_BUFFER_DYNAMIC:
                break;
            case DescriptorType::STORAGE_BUFFER_DYNAMIC:
                break;
            case DescriptorType::INPUT_ATTACHMENT:
                break;
            }
        }
        else
        {
            ASSERT_MSG_DEBUG(0, "Binding not found");
        }
    }

    //AllocateDescriptors(*it, desc, numBindings);
    return setWrapper;
}

void UniformFactory::AllocateBindingResources(SetWrapper * setWrapper, ShaderBindingDescription * desc, size_t allocationSize, const uint32_t & bindingIndexInSetwrapper, AllocationMethod allocation)
{
    //for (uint32_t i = 0; i < numBindings; i++)
    {
        BindingWrapper binding = setWrapper->bindingWrapperList[bindingIndexInSetwrapper];
        //if (binding.bindingName == desc[i].resourceName && binding.bindingObj.binding == desc[i].binding &&
            //binding.memberList.size() == desc[i].numElements)
        {
            switch (binding.bindingObj.descriptorType)
            {
            case DescriptorType::SAMPLER:
                break;
            case DescriptorType::COMBINED_IMAGE_SAMPLER:
                break;
            case DescriptorType::SAMPLED_IMAGE:
                break;
            case DescriptorType::STORAGE_IMAGE:
                break;
            case DescriptorType::UNIFORM_TEXEL_BUFFER:
                break;
            case DescriptorType::STORAGE_TEXEL_BUFFER:
                break;
            case DescriptorType::UNIFORM_BUFFER:
                HandleUniformBuffer(desc, setWrapper, binding, allocationSize);
                break;
            case DescriptorType::STORAGE_BUFFER:
                break;
            case DescriptorType::UNIFORM_BUFFER_DYNAMIC:
                break;
            case DescriptorType::STORAGE_BUFFER_DYNAMIC:
                break;
            case DescriptorType::INPUT_ATTACHMENT:
                break;
            }
        }
        /*else
        {
            ASSERT_MSG_DEBUG(0, "Binding not found");
        }*/
    }
}

void UniformFactory::UploadDataToBuffers(const uint32_t & bufId, const size_t & dataSize, const size_t & memAlignedSize, void * data, const size_t & memoryOffset, bool keepMemoryMounted)
{
    apiInterface->CopyBufferDataToMemory(bufId, dataSize, memAlignedSize, data, memoryOffset, keepMemoryMounted);
}

void UniformFactory::AllocateDescriptorSet(SetWrapper * wrapper, ShaderBindingDescription * desc, const uint32_t & numBindings, const uint32_t & numDescriptorsPerBinding)
{
    // Allocate descriptor sets
    uint32_t * descriptorIds = apiInterface->AllocateDescriptorsSet(wrapper, numDescriptorsPerBinding);

    for (uint32_t i = 0; i < numBindings; i++)
    {
        for (uint32_t k = 0; k < numDescriptorsPerBinding; k++)
            desc[i].descriptorSetIds.push_back(descriptorIds[k]);

    }
    // update the descriptors
    apiInterface->LinkSetBindingToResources(desc, numBindings);
}

SetWrapper * UniformFactory::GetSetWrapper(ShaderBindingDescription * desc, const uint32_t & numBindings)
{
    uint32_t foundCount = 0;

    SetWrapper * wrapper;

    {
        std::vector<SetWrapper*>::iterator it;
        it = std::find_if(setWrapperList->begin(), setWrapperList->end(), [&](SetWrapper * e)
        {
            // Check if the set value and num of bindings are same
            if(e->setValue == desc[0].set && numBindings == e->bindingWrapperList.size())
            for (uint32_t i = 0; i < numBindings; i++)
            {
                // check if the number of elements in each binding is same
                if (e->bindingWrapperList[i].bindingObj.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER ||
                    e->bindingWrapperList[i].bindingObj.descriptorType == DescriptorType::SAMPLER)
                {
                    if (e->bindingWrapperList[i].bindingName == desc[i].resourceName)
                        foundCount++;
                }
                else if (e->bindingWrapperList[i].memberList.size() == desc[i].numElements)
                {
                    foundCount++;
                }
            }

            return (foundCount == numBindings);
            //return (e->setValue == desc[i].set && numBindings == e->bindingWrapperList.size());
        });

        ASSERT_MSG_DEBUG(it != setWrapperList->end(), "Set not found");
       
        wrapper = *it;
    }

    return wrapper;
}

std::vector<uint32_t> UniformFactory::AcquireMeshList(SetWrapper * wrapper)
{
    ASSERT_MSG(0, "Not implemented");
    return std::vector<uint32_t>();
}

size_t UniformFactory::GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize)
{
    return apiInterface->GetMemoryAlignedDataSizeForBuffer(dataSize);
}

void UniformFactory::AllocateUniformBuffer(BufferCreateInfo * info, const uint32_t & numBuffers, uint32_t * out_buffIds, size_t * out_bufferMemRequirements)
{
    apiInterface->CreateBuffers(info, numBuffers, out_buffIds, out_bufferMemRequirements);
}

void UniformFactory::CreateSetLayout(ShaderBindingDescription * desc, const uint32_t & numBindings)
{
    // Create set Layout bindings

}

uint32_t UniformFactory::CreateSampler(const SamplerCreateInfo & info)
{
    return apiInterface->CreateSampler(info);
}

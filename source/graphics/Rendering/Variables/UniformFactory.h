#pragma once

#include "Settings.h"
#include "ShaderResourceDescription.h"
#include <vector>
#include <map>
#include <RenderingWrapper.h>

#if (RENDERING_API == VULKAN)
class VulkanInterface;
typedef VulkanInterface ApiInterface;
#elif (RENDERING_API == DX)
class DxInterface;
typedef DxInterface ApiInterface;
#endif

//struct UniformWrapper
//{
//    SetWrapper * setWrapper;
//    BindingWrapper bindingWrapper;
//    uint32_t numUniformsAllocated;
//};

class UniformFactory
{
private:
    UniformFactory(){}
    UniformFactory(UniformFactory const &) {}
    UniformFactory const & operator=(UniformFactory const &) {}

    static UniformFactory* instance;

    std::vector<SetWrapper*> * setWrapperList;
    ApiInterface * apiInterface;

    std::map<uint32_t, GlobalResourceAllocationConfig> setConfig;
    //std::vector<UniformWrapper> uniformWrapperList;
    void HandleUniformBuffer(ShaderBindingDescription * desc, SetWrapper * setWrapper, BindingWrapper bindingObj, size_t allocationSize);
    //void HandleSampler()
    std::vector<uint32_t> bufferIds;
    std::vector<uint32_t> memoryIds;

public:
    void Init(ApiInterface * apiInterface);
    void DeInit();
    void Update();
    static UniformFactory* GetInstance();
    ~UniformFactory();
    
    // Create buffers/images
    SetWrapper * AllocateSetResource(ShaderBindingDescription * desc, size_t * allocationSize, const uint32_t & numBindings, AllocationMethod allocation);
    // meant for a single binding of a set
    void AllocateBindingResources(SetWrapper * setWrapper, ShaderBindingDescription * desc, size_t allocationSize, const uint32_t & bindingIndexInSetwrapper, AllocationMethod allocation);
    void AllocateDescriptorSet(SetWrapper * wrapper, ShaderBindingDescription * desc, const uint32_t & numBindings, const uint32_t & numDescriptorsPerBinding);

    SetWrapper * GetSetWrapper(ShaderBindingDescription * desc, const uint32_t & numBindings);
    std::vector<uint32_t> AcquireMeshList(SetWrapper * wrapper);

    void UploadDataToBuffers(const uint32_t & bufId, const size_t & dataSize, const size_t & memAlignedSize, void * data, const size_t & memoryOffset, bool keepMemoryMounted);
    size_t GetMemoryAlignedDataSizeForBuffer(const size_t & dataSize);
    void AllocateUniformBuffer(BufferCreateInfo * info, const uint32_t & numBuffers, uint32_t * out_buffIds, size_t * out_bufferMemRequirements);
    void CreateSetLayout(ShaderBindingDescription * desc, const uint32_t & numBindings);

    // should not be in Uniform Factory, need to create a sampler factory
    uint32_t CreateSampler(const SamplerCreateInfo & info);
};

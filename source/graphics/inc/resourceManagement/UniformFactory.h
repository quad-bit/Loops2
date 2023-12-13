#pragma once

#include "Settings.h"
#include "Utility/ShaderResourceDescription.h"
#include <vector>
#include <map>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

namespace Renderer
{
    namespace ResourceManagement
    {
        class UniformFactory
        {
        private:
            UniformFactory() {}
            UniformFactory(UniformFactory const&) {}
            UniformFactory const& operator=(UniformFactory const&) {}

            static UniformFactory* instance;

            std::vector<Core::Wrappers::SetWrapper*>* setWrapperList;
            std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>>* m_perSetMap;
            std::map<uint32_t, Core::Utility::GlobalResourceAllocationConfig> setConfig;
            
            // Deprecated
            void HandleUniformBuffer(Core::Utility::ShaderBindingDescription* desc, Core::Wrappers::SetWrapper* setWrapper, Core::Wrappers::BindingWrapper bindingObj, size_t allocationSize);
            
            void HandleUniformBuffer(Core::Utility::BufferBindingInfo& bufferInfo);
            //void HandleSampler()
            std::vector<uint32_t> bufferIds;
            std::vector<uint32_t> memoryIds;

        public:
            void Init();
            void DeInit();
            void Update();
            static UniformFactory* GetInstance();
            ~UniformFactory();

            ///////// OLD ================

            // Create buffers/images
            Core::Wrappers::SetWrapper* AllocateSetResource(Core::Utility::ShaderBindingDescription* desc, size_t* allocationSize, const uint32_t& numBindings, Core::Utility::AllocationMethod allocation);
            // meant for a single binding of a set
            void AllocateBindingResources(Core::Wrappers::SetWrapper* setWrapper, Core::Utility::ShaderBindingDescription* desc, size_t allocationSize, const uint32_t& bindingIndexInSetwrapper, Core::Utility::AllocationMethod allocation);
            void AllocateDescriptorSet(Core::Wrappers::SetWrapper* wrapper, Core::Utility::ShaderBindingDescription* desc, const uint32_t& numBindings, const uint32_t& numDescriptorsPerBinding);

            ///////// OLD ================

            Core::Wrappers::SetWrapper* AllocateSetResources(const Core::Utility::DescriptorSetInfo& setDescription);
            void AllocateDescriptorSets(Core::Wrappers::SetWrapper* wrapper, Core::Utility::DescriptorSetInfo& setDescription, const uint32_t& numDescriptorSets);

            Core::Wrappers::SetWrapper* GetSetWrapper(const Core::Utility::DescriptorSetInfo& setDescription);
            std::vector<uint32_t> AcquireMeshList(Core::Wrappers::SetWrapper* wrapper);

            void UploadDataToBuffers(const uint32_t& bufId, const size_t& dataSize, const size_t& memAlignedSize, void* data, const size_t& memoryOffset, bool keepMemoryMounted);
            size_t GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize);

            //Not getting used, BufferCreateInfo
            void AllocateUniformBuffer(Core::Wrappers::BufferCreateInfo* info, const uint32_t& numBuffers, uint32_t* out_buffIds, size_t* out_bufferMemRequirements);

            void CreateSetLayout(Core::Utility::ShaderBindingDescription* desc, const uint32_t& numBindings);

            // should not be in Uniform Factory, need to create a sampler factory
            uint32_t CreateSampler(const Core::Wrappers::SamplerCreateInfo& info);
        };
    }
}

typedef Renderer::ResourceManagement::UniformFactory UniFactAlias;

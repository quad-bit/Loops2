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
        class ShaderResourceManager
        {
        private:
            ShaderResourceManager() {}
            ShaderResourceManager(ShaderResourceManager const&) {}
            ShaderResourceManager const& operator=(ShaderResourceManager const&) {}

            static ShaderResourceManager* instance;

            std::vector<Core::Wrappers::SetWrapper*>* setWrapperList;
            std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>>* m_perSetMap;
            std::map<uint32_t, Core::Utility::GlobalResourceAllocationConfig> setConfig;

            void HandleUniformBuffer(Core::Utility::BufferBindingInfo& bufferInfo);

            std::vector<uint32_t> bufferIds;
            std::vector<uint32_t> memoryIds;
        public:
            void Init();
            void DeInit();
            void Update();
            static ShaderResourceManager* GetInstance();
            ~ShaderResourceManager();

            // Create shader resources on the go
            Core::Wrappers::SetWrapper* AllocateSetResources(const Core::Utility::DescriptorSetInfo& setDescription);
            // Create shader resources prior to calling the below
            void AllocateDescriptorSets(Core::Wrappers::SetWrapper* wrapper, Core::Utility::DescriptorSetInfo& setDescription, const uint32_t& numDescriptorSets);

            Core::Wrappers::SetWrapper* GetSetWrapper(const Core::Utility::DescriptorSetInfo& setDescription);

            void UploadDataToBuffers(const uint32_t& bufId, const size_t& dataSize, const size_t& memAlignedSize, void* data, const size_t& memoryOffset, bool keepMemoryMounted);
            size_t GetMemoryAlignedDataSizeForBuffer(const size_t& dataSize);
        };
    }
}

typedef Renderer::ResourceManagement::ShaderResourceManager ShdrResMgrAlias;

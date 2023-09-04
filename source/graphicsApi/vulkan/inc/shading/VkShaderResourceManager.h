#pragma once

#include <vulkan/vulkan.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <rapidjson/document.h> 
#include <vector>
#include <map>
#include <tuple>
#include <Utility/ShaderResourceDescription.h>

namespace GfxVk
{
    namespace Shading
    {
        struct ShaderResources
        {
            std::string shaderName;
            std::vector<Core::Wrappers::SetWrapper*> setWrappers;
        };

        struct PipelineLayoutWrapper
        {
            uint32_t id;
            std::vector<VkDescriptorSetLayout> setLayoutList;
            std::vector<int> setValuesInPipelineLayout;
            VkPipelineLayout* pipelineLayout;
        };

        //class VkShaderResourceAllocator;

        class VkShaderResourceManager
        {
        private:
            VkShaderResourceManager() {}
            VkShaderResourceManager(VkShaderResourceManager const&) {}
            VkShaderResourceManager const& operator= (VkShaderResourceManager const&) {}

            static VkShaderResourceManager* instance;
            //VkShaderResourceAllocator * resourceAllocator;

            uint32_t bindingIdCounter = 0;
            uint32_t setWrapperIdCounter = 0;
            uint32_t setLayoutWrapperIdCounter = 0;
            uint32_t pipelineLayoutIdCounter = 0;
            uint32_t vkdescriptorSetIdCounter = 0;
            uint32_t GetBindingID();
            uint32_t GetSetLayoutID();
            uint32_t GetPipelineLayoutID();
            uint32_t GetVkDescriptorSetLayoutWrapperID();
            uint32_t GetVkDescriptorSetID();

            void DecrementSetLayoutId() { setWrapperIdCounter--; }

            std::vector<Core::Wrappers::SetWrapper*> setWrapperList;
            std::map< uint32_t, VkDescriptorSetLayout*> idToSetLayoutMap;
            std::map< uint32_t, VkDescriptorSet> idToSetMap;
            std::map< uint32_t, uint32_t> descIdToSetValueMap;
            std::vector<ShaderResources> perShaderResourceList;
            std::vector<PipelineLayoutWrapper> pipelineLayoutWrapperList;
            std::vector<VkDescriptorSetLayout> fillerSetLayouts;

            VkDescriptorSetLayout fillerSetLayout = VK_NULL_HANDLE;
            VkDescriptorSet fillerSet = VK_NULL_HANDLE;

            std::map< uint32_t, std::vector<int>> pipelineLayoutIdToSetValuesMap;

            void CreateUniqueSetLayoutWrapper(std::vector<Core::Wrappers::BindingWrapper>& bindingList, std::string shaderName, uint32_t set);
            void CreateUniqueSetLayoutWrapper(const std::vector<Core::Wrappers::BindingWrapper>& bindingList, const std::vector<std::string>& shaderName, uint32_t set);
            void AccumulateSetLayoutPerShader(std::string shaderName, Core::Wrappers::SetWrapper* setWrapper);

            VkDescriptorSetLayout* CreateSetLayouts(Core::Wrappers::SetWrapper* setWrapper);
            VkDescriptorType UnwrapDescriptorType(const Core::Enums::DescriptorType& descType);

            Core::Enums::ShaderType GetTypeFromName(const std::string& shaderName);
            std::string GetShaderNameFromRefl(const std::string& reflName);

            VkDescriptorSet GetDescriptorSet(const uint32_t& id);

            // will add fller setlayouts in case intermediate sets are missing.
            void GetSetLayouts(Core::Wrappers::SetWrapper** setWrapperList, const uint32_t& numSets, std::vector<VkDescriptorSetLayout>& layoutList,
                std::vector<int>& setValueList);

        public:
            void Init();
            void Init(const std::string& pipelineFilePath);
            void DeInit();
            void Update();
            static VkShaderResourceManager* GetInstance();
            ~VkShaderResourceManager();

            std::vector<Core::Wrappers::SetWrapper*> GetSetsForShaders(const std::vector<std::string>& shaderNames);
            uint32_t CreatePipelineLayout(Core::Wrappers::SetWrapper** setWrapperList, const size_t& numSets);
            VkPipelineLayout* GetPipelineLayout(const uint32_t& id);
            std::vector<Core::Wrappers::SetWrapper*>* GetSetWrapperList();

            std::vector<VkDescriptorSet> GetDescriptors(uint32_t* ids, const uint32_t& count, const uint32_t& pipelineLayoutId);
            std::tuple<std::vector<VkDescriptorSet>, uint32_t> GetDescriptors(uint32_t* ids, const uint32_t& count, const uint32_t& pipelineLayoutId, const uint32_t& firstSet);
            uint32_t* AllocateDescriptorSets(Core::Wrappers::SetWrapper* set, const uint32_t& numDescriptors);
            //void LinkSetBindingToResources(ShaderBindingDescription * desc);
            void LinkSetBindingToResources(Core::Utility::ShaderBindingDescription* desc, const uint32_t& numBindings);
            const std::vector<int>* GetSetValuesInPipelineLayout(const uint32_t& pipelineLayoutId);
        };
    }
}
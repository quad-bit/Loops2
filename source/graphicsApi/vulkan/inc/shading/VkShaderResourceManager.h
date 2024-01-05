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

        enum class BindingType
        {
            POSITION,
            COLOR,
            NORMAL,
            UV_0,
            UV_1
        };

        // The buffer type bound at a binding
        struct VertexBindingTypeInfo
        {
            BindingType m_bindingType;
            // Binding index represents the position at which the respective
            // vertex buffer should be place while calling vkCmdBindVertexBuffers
            uint32_t m_index;
        };

        struct TaskWrapper
        {
            std::string m_name;
            std::vector<std::string> m_shaderNames;
            uint32_t m_shaderModuleStateId;
            std::vector<Core::Wrappers::SetWrapper*> m_setWrappers;
            uint32_t m_pipelineLayoutId;
            std::vector<VertexBindingTypeInfo> m_bindingTypeInfo;
            std::optional<uint32_t> m_vertexInputStateId;
        };

        struct TechniqueWrapper
        {
            std::string techniqueName;
            uint32_t lodMin, lodMax;
            std::vector<TaskWrapper> m_taskList;

            /*std::vector<std::string> m_shaderNames;
            std::vector<Core::Wrappers::SetWrapper*> m_setWrappers;
            uint32_t m_pipelineLayoutId;
            std::vector<VertexBindingTypeInfo> m_bindingTypeInfo;
            uint32_t m_vertexInputStateId;
            uint32_t m_shaderModuleStateId;*/
        };

        struct VertexInputStateInfoWrapper
        {
            std::vector<VkVertexInputAttributeDescription> m_attributeList;
            std::vector<VkVertexInputBindingDescription> m_bindingList;
            VkPipelineVertexInputStateCreateInfo m_createInfo;
            uint32_t m_id;
        };

        struct ShaderStageInfoWrapper
        {
            uint32_t m_id;
            std::vector<VkPipelineShaderStageCreateInfo> m_infoList;
        };

        struct EffectResources
        {
            std::string m_effectName;
            std::vector<TechniqueWrapper> techniqueList;
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

            uint32_t m_bindingIdCounter = 0;
            uint32_t m_setWrapperIdCounter = 0;
            uint32_t m_setLayoutWrapperIdCounter = 0;
            uint32_t m_pipelineLayoutIdCounter = 0;
            uint32_t m_vkdescriptorSetIdCounter = 0;
            uint32_t m_vertexInputWrapperIdCounter = 0;
            uint32_t m_shaderStateWrapperIdCounter = 0;

            uint32_t GetBindingID();
            uint32_t GetSetLayoutID();
            uint32_t GetPipelineLayoutID();
            uint32_t GetVkDescriptorSetLayoutWrapperID();
            uint32_t GetVkDescriptorSetID();
            uint32_t GetVertexInputID();
            uint32_t GetShaderStateID();

            void DecrementSetLayoutId() { m_setWrapperIdCounter--; }

            std::vector<Core::Wrappers::SetWrapper*> m_setWrapperList;
            //setwrapper id to descriptorSetLayout
            std::map< uint32_t, VkDescriptorSetLayout*> m_idToSetLayoutMap;
            std::map< uint32_t, VkDescriptorSet> m_idToSetMap;
            std::map< uint32_t, uint32_t> m_descIdToSetValueMap;
            std::vector<ShaderResources> m_perShaderResourceList;
            std::vector<PipelineLayoutWrapper> m_pipelineLayoutWrapperList;
            std::vector<VkDescriptorSetLayout> m_fillerSetLayouts;
            //std::vector<EffectResources> m_effectResources;
            std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>> m_perSetMap;
            std::map<uint32_t, VertexInputStateInfoWrapper> m_vertexInputWrapperMap;
            std::map<uint32_t, ShaderStageInfoWrapper> m_shaderStageWrapperMap;

            std::vector<EffectResources> m_perEffectResources;

            VkDescriptorSetLayout m_fillerSetLayout = VK_NULL_HANDLE;
            VkDescriptorSet m_fillerSet = VK_NULL_HANDLE;

            std::map< uint32_t, std::vector<int>> m_pipelineLayoutIdToSetValuesMap;

            void CreateUniqueSetLayoutWrapper(std::vector<Core::Wrappers::BindingWrapper>& bindingList, std::string shaderName, uint32_t set);
            Core::Wrappers::SetWrapper* CreateUniqueSetLayoutWrapper(const std::vector<Core::Wrappers::BindingWrapper>& bindingList, const std::string& effectName, const std::vector<std::string>& shaderName, uint32_t set);
            void AccumulateSetLayoutPerShader(std::string shaderName, Core::Wrappers::SetWrapper* setWrapper);

            VkDescriptorSetLayout* CreateSetLayouts(Core::Wrappers::SetWrapper* setWrapper);
            VkDescriptorType UnwrapDescriptorType(const Core::Enums::DescriptorType& descType);

            Core::Enums::ShaderType GetTypeFromName(const std::string& shaderName);
            std::string GetShaderNameFromRefl(const std::string& reflName);

            VkDescriptorSet GetDescriptorSet(const uint32_t& id);

            // will add fller setlayouts in case intermediate sets are missing.
            void GetSetLayouts(Core::Wrappers::SetWrapper** setWrapperList, const uint32_t& numSets, std::vector<VkDescriptorSetLayout>& layoutList,
                std::vector<int>& setValueList);

            void CreateVertexInfoForTask(const rapidjson::Value& taskDoc, GfxVk::Shading::TaskWrapper& taskWapper);
            void CreateShaderInfoForTask(const rapidjson::Value& taskDoc, GfxVk::Shading::TaskWrapper& taskWapper);

            const char* shaderEntryName = "main";

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
            VkPipelineShaderStageCreateInfo* GetShaderStageCreateInfo(uint32_t id);

            std::vector<Core::Wrappers::SetWrapper*>* GetSetWrapperList();

            std::vector<VkDescriptorSet> GetDescriptors(uint32_t* ids, const uint32_t& count, const uint32_t& pipelineLayoutId);
            std::tuple<std::vector<VkDescriptorSet>, uint32_t> GetDescriptors(uint32_t* ids, const uint32_t& count, const uint32_t& pipelineLayoutId, const uint32_t& firstSet);
            uint32_t* AllocateDescriptorSets(Core::Wrappers::SetWrapper* set, const uint32_t& numDescriptors);
            //void LinkSetBindingToResources(ShaderBindingDescription * desc);
            void LinkSetBindingToResources(Core::Utility::ShaderBindingDescription* desc, const uint32_t& numBindings);
            void LinkSetBindingToResources(const Core::Utility::DescriptorSetInfo desc, const uint32_t& numDescriptorSets);
            const std::vector<int>* GetSetValuesInPipelineLayout(const uint32_t& pipelineLayoutId);
            
            std::map<uint32_t, std::vector<Core::Wrappers::SetWrapper*>>* GetPerSetSetwrapperMap();

            uint32_t GetVertexInputStateId(const std::string& effectName, const std::string& techniqueName, const std::string& taskName);
            uint32_t GetPipelineLayoutId(const std::string& effectName, const std::string& techniqueName, const std::string& taskName);
            uint32_t GetShaderStateId(const std::string& effectName, const std::string& techniqueName, const std::string& taskName);

        };
    }
}
#pragma once

#include <vulkan\vulkan.h>
#include <RenderingWrapper.h>
#include <rapidjson\document.h> 
#include <vector>
#include <map>
#include <tuple>
#include <ShaderResourceDescription.h>

struct ShaderResources
{
    std::string shaderName;
    std::vector<SetWrapper*> setWrappers;
};

struct PipelineLayoutWrapper
{
    uint32_t id;
    std::vector<VkDescriptorSetLayout> setLayoutList;
    std::vector<int> setValuesInPipelineLayout;
    VkPipelineLayout * pipelineLayout;
};

//class VkShaderResourceAllocator;

class VkShaderResourceManager
{
private:
    VkShaderResourceManager(){}
    VkShaderResourceManager(VkShaderResourceManager const &) {}
    VkShaderResourceManager const & operator= (VkShaderResourceManager const &) {}

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

    std::vector<SetWrapper*> setWrapperList;
    std::map< uint32_t, VkDescriptorSetLayout *> idToSetLayoutMap;
    std::map< uint32_t, VkDescriptorSet> idToSetMap;
    std::map< uint32_t, uint32_t> descIdToSetValueMap;
    std::vector<ShaderResources> perShaderResourceList;
    std::vector<PipelineLayoutWrapper> pipelineLayoutWrapperList;
    std::vector<VkDescriptorSetLayout> fillerSetLayouts;

    VkDescriptorSetLayout fillerSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet fillerSet = VK_NULL_HANDLE;
    
    std::map< uint32_t, std::vector<int>> pipelineLayoutIdToSetValuesMap;

    void CreateUniqueSetLayoutWrapper(std::vector<BindingWrapper>& bindingList, std::string shaderName, uint32_t set);
    void CreateUniqueSetLayoutWrapper(const std::vector<BindingWrapper> & bindingList, const std::vector<std::string>& shaderName, uint32_t set);
    void AccumulateSetLayoutPerShader(std::string shaderName, SetWrapper * setWrapper);

    VkDescriptorSetLayout * CreateSetLayouts(SetWrapper * setWrapper);
    VkDescriptorType UnwrapDescriptorType(const DescriptorType & descType);

    ShaderType GetTypeFromName(const std::string & shaderName);
    std::string GetShaderNameFromRefl(const std::string & reflName);

    VkDescriptorSet GetDescriptorSet(const uint32_t & id);
    
    // will add fller setlayouts in case intermediate sets are missing.
    void GetSetLayouts(SetWrapper ** setWrapperList, const uint32_t & numSets, std::vector<VkDescriptorSetLayout> & layoutList,
        std::vector<int> & setValueList);

public:
    void Init();
    void Init(const std::string& pipelineFilePath);
    void DeInit();
    void Update();
    static VkShaderResourceManager* GetInstance();
    ~VkShaderResourceManager();

    std::vector<SetWrapper*> GetSetsForShaders(const std::vector<std::string> & shaderNames);
    uint32_t CreatePipelineLayout(SetWrapper ** setWrapperList, const size_t & numSets);
    VkPipelineLayout * GetPipelineLayout(const uint32_t & id);
    std::vector<SetWrapper*> * GetSetWrapperList();
    
    std::vector<VkDescriptorSet> GetDescriptors(uint32_t * ids, const uint32_t & count, const uint32_t & pipelineLayoutId);
    std::tuple<std::vector<VkDescriptorSet>, uint32_t> GetDescriptors(uint32_t * ids, const uint32_t & count, const uint32_t & pipelineLayoutId, const uint32_t & firstSet);
    uint32_t * AllocateDescriptorSets(SetWrapper * set, const uint32_t & numDescriptors);
    //void LinkSetBindingToResources(ShaderBindingDescription * desc);
    void LinkSetBindingToResources(ShaderBindingDescription * desc, const uint32_t & numBindings);
    const std::vector<int> * GetSetValuesInPipelineLayout(const uint32_t & pipelineLayoutId);
};

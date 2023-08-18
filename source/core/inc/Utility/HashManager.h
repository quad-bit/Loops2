#pragma once

#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include <map>
#include <vector>

class Shader;

//struct ShaderStateWrapper;
//struct SetWrapper;
//enum class PipelineStates;

struct PipelineObjectHashWrapper
{
    uint32_t objectId;
    Core::Enums::PipelineStates state;
    std::size_t hash;
};

struct DescriptorSetLayoutHashWrapper
{
    uint32_t objectId;
    std::size_t hash = 0UL;
};

//combination of sets
struct SetLayoutCombinationWrapper
{
    uint32_t layoutId;
    std::vector<Core::Wrappers::SetWrapper*> setList;
    std::size_t hash;
};

struct MaterialHashWrapper
{
    uint32_t materialId;
    std::size_t hash;
};

template < typename T>
struct PipelineStateHashWrapper
{
    uint32_t objectId;
    std::size_t hash;
    Core::Enums::PipelineStates state;
};

class HashManager
{
private:
    HashManager(){}
    HashManager(HashManager const &) {}
    HashManager const & operator= (HashManager const &) {}

    static HashManager* instance;
    
    std::map<Core::Enums::PipelineStates, std::vector<std::size_t>> stateToHashMap;
    std::vector<PipelineObjectHashWrapper> pipelineHashList;
    std::vector<DescriptorSetLayoutHashWrapper> descriptorSetLayoutWrapperHashList;
    std::vector<SetLayoutCombinationWrapper> setCombinationWrapperHashList;
    std::vector<MaterialHashWrapper> materialWrapperHashList;

    std::vector<PipelineStateHashWrapper<Core::Wrappers::TessellationState>> tessellationStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::ViewportState>> viewportStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::RasterizationState>> rasterizationStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::MultiSampleState>> multisampleStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::DepthStencilState>> depthStencilStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::ColorBlendState>> ColorblendStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<Core::Wrappers::DynamicStateList>> dynamicStateHashWrapperList;
    
    uint32_t CheckForPipeLineObjectHash(const std::size_t & hash, const uint32_t & stateId, const Core::Enums::PipelineStates & state);
    
    template < typename T> 
    int CheckForHashExistence(std::vector<PipelineStateHashWrapper<T>> & list, const size_t & hash,
        const uint32_t & stateId);


public:
    void Init();
    void DeInit();
    void Update();
    static HashManager* GetInstance();
    ~HashManager();

    int FindVertexInputStateHash(Core::Wrappers::VertexInputState * inputInfo, uint32_t stateId);
    int FindInputAssemblyStateHash(Core::Wrappers::InputAssemblyState * inputInfo, uint32_t stateId);
    int FindShaderStateHash(Shader * inputInfo, const uint32_t & shaderCount,uint32_t stateId, Core::Enums::PipelineStates * state);
    int FindTessellationStateHash(Core::Wrappers::TessellationState * inputInfo, uint32_t stateId);
    int FindViewportStateHash(Core::Wrappers::ViewportState * inputInfo, uint32_t stateId);
    int FindRasterizationHash(Core::Wrappers::RasterizationState * inputInfo, uint32_t stateId);
    int FindMultiSampleHash(Core::Wrappers::MultiSampleState * inputInfo, uint32_t stateId);
    int FindDepthStencilHash(Core::Wrappers::DepthStencilState * inputInfo, uint32_t stateId);
    int FindColorBlendHash(Core::Wrappers::ColorBlendState * inputInfo, uint32_t stateId);
    int FindDynamicStateHash(Core::Wrappers::DynamicStateList * inputInfo, uint32_t stateId);

    int FindDescriptorSetHash(Core::Wrappers::SetWrapper * obj, uint32_t id);
    int FindResourceLayoutHash(Core::Wrappers::SetWrapper ** obj, const uint32_t & setCount, uint32_t id);
    int FindMaterialHash(const std::vector<std::string> * shaderNames, const uint32_t & matId);
};

template<typename T>
inline int HashManager::CheckForHashExistence( std::vector<PipelineStateHashWrapper<T>>& list, const size_t & hash, const uint32_t & stateId)
{
    std::vector<PipelineStateHashWrapper<T>>::iterator it;
    it = std::find_if(list.begin(), list.end(), [&](PipelineStateHashWrapper<T> e) { return e.hash == hash; });

    if (it != list.end())
    {
        return it->objectId;
    }
    else
    {
        PipelineStateHashWrapper<T> obj = {};
        obj.hash = hash;
        obj.objectId = stateId;

        list.push_back(obj);
        return -1;
    }

    return 0;
}

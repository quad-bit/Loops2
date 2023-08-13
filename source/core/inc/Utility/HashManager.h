#pragma once

#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include <map>
#include <vector>

class Shader;
struct ShaderStateWrapper;
struct SetWrapper;
enum class PipelineStates;

struct PipelineObjectHashWrapper
{
    uint32_t objectId;
    PipelineStates state;
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
    std::vector<SetWrapper*> setList;
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
    PipelineStates state;
};

class HashManager
{
private:
    HashManager(){}
    HashManager(HashManager const &) {}
    HashManager const & operator= (HashManager const &) {}

    static HashManager* instance;
    
    std::map<PipelineStates, std::vector<std::size_t>> stateToHashMap;
    std::vector<PipelineObjectHashWrapper> pipelineHashList;
    std::vector<DescriptorSetLayoutHashWrapper> descriptorSetLayoutWrapperHashList;
    std::vector<SetLayoutCombinationWrapper> setCombinationWrapperHashList;
    std::vector<MaterialHashWrapper> materialWrapperHashList;

    std::vector<PipelineStateHashWrapper<TessellationState>> tessellationStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<ViewportState>> viewportStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<RasterizationState>> rasterizationStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<MultiSampleState>> multisampleStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<DepthStencilState>> depthStencilStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<ColorBlendState>> ColorblendStateHashWrapperList;
    std::vector<PipelineStateHashWrapper<DynamicStateList>> dynamicStateHashWrapperList;
    
    uint32_t CheckForPipeLineObjectHash(const std::size_t & hash, const uint32_t & stateId, const PipelineStates & state);
    
    template < typename T> 
    int CheckForHashExistence(std::vector<PipelineStateHashWrapper<T>> & list, const size_t & hash,
        const uint32_t & stateId);


public:
    void Init();
    void DeInit();
    void Update();
    static HashManager* GetInstance();
    ~HashManager();

    int FindVertexInputStateHash(VertexInputState * inputInfo, uint32_t stateId);
    int FindInputAssemblyStateHash(InputAssemblyState * inputInfo, uint32_t stateId);
    int FindShaderStateHash(Shader * inputInfo, const uint32_t & shaderCount, 
        uint32_t stateId, PipelineStates * state);
    int FindTessellationStateHash(TessellationState * inputInfo, uint32_t stateId);
    int FindViewportStateHash(ViewportState * inputInfo, uint32_t stateId);
    int FindRasterizationHash(RasterizationState * inputInfo, uint32_t stateId);
    int FindMultiSampleHash(MultiSampleState * inputInfo, uint32_t stateId);
    int FindDepthStencilHash(DepthStencilState * inputInfo, uint32_t stateId);
    int FindColorBlendHash(ColorBlendState * inputInfo, uint32_t stateId);
    int FindDynamicStateHash(DynamicStateList * inputInfo, uint32_t stateId);

    int FindDescriptorSetHash(SetWrapper * obj, uint32_t id);
    int FindResourceLayoutHash(SetWrapper ** obj, const uint32_t & setCount, uint32_t id);
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

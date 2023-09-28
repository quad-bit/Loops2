#include "Utility/HashManager.h"
#include "Utility/HashHelper.h"
#include "Platform/Assertion.h"
//#include "Utility/RenderingWrappers/RenderingWrapper.h"

HashManager* HashManager::instance = nullptr;

uint32_t HashManager::CheckForPipeLineObjectHash(const std::size_t & hash, const uint32_t & stateId, const Core::Enums::PipelineStates & state)
{
    std::vector<PipelineObjectHashWrapper>::iterator it;
    it = std::find_if(pipelineHashList.begin(), pipelineHashList.end(), [&](PipelineObjectHashWrapper e) { return e.hash == hash; });

    if (it != pipelineHashList.end())
    {
        return it->objectId;
    }
    else
    {
        PipelineObjectHashWrapper obj;
        obj.hash = hash;
        obj.objectId = stateId;
        obj.state = state;

        pipelineHashList.push_back(obj);
        return -1;
    }
}

void HashManager::Init()
{
    
}

void HashManager::DeInit()
{
    pipelineHashList.clear();
    pipelineHashList.resize(0);

    descriptorSetLayoutWrapperHashList.clear();
}

void HashManager::Update()
{

}

HashManager * HashManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new HashManager();
    }
    return instance;
}

HashManager::~HashManager()
{
}

int HashManager::FindVertexInputStateHash(Core::Wrappers::VertexInputState * inputInfo, uint32_t stateId)
{
    std::size_t hash = 0UL;
    size_t hashAttrib, hashBinding, hashPipelineState;

    for (uint32_t i = 0; i < inputInfo->attribCount; i++)
    {
        hashAttrib = std::hash<Core::Wrappers::VertexInputAttributeInfo>{}(inputInfo->attribInfo[i]);
        HashCombine(hash, hashAttrib);
    }

    for (uint32_t i = 0; i < inputInfo->bindingCount; i++)
    {
        hashBinding = std::hash<Core::Wrappers::VertexInputBindingInfo>{}(inputInfo->bindingInfo[i]);
        HashCombine(hash, hashBinding);
    }

    hashPipelineState = std::hash<uint32_t>{}((uint32_t)inputInfo->state);
    HashCombine(hash, hashPipelineState);

    return CheckForPipeLineObjectHash(hash, stateId, inputInfo->state);
}

int HashManager::FindInputAssemblyStateHash(Core::Wrappers::InputAssemblyState * inputInfo, uint32_t stateId)
{
    std::size_t hash = 0UL;
    size_t hashPrimitive, hashRestart, hashState;

    hashPrimitive = std::hash<uint32_t>{}((uint32_t)*inputInfo->primitiveType);
    hashRestart = std::hash<bool>{}(inputInfo->isPrimtiveRestartEnabled);
    hashState = std::hash<uint32_t>{}((uint32_t)inputInfo->state);

    HashCombine(hash, hashPrimitive, hashRestart, hashState);

    return CheckForPipeLineObjectHash(hash, stateId, inputInfo->state);
}

int HashManager::FindShaderStateHash(Core::Wrappers::ShaderWrapper* shaders, const uint32_t & shaderCount, uint32_t stateId, Core::Enums::PipelineStates * state)
{
    // TODO : needs correction
    std::size_t hash = 0UL;
    size_t shaderCountHash;
    size_t vertexShaderHash = 0UL, fragmentShaderHash = 0UL;
    ASSERT_MSG_DEBUG(shaderCount <= 2, "More than two shaders, yet to be handled");

    shaderCountHash = std::hash<uint32_t>{}(shaderCount);

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        size_t shaderTypeHash = std::hash<uint32_t>{}((uint32_t)shaders[i].shaderType);
        size_t shaderNameHash = std::hash<std::string>{}(shaders[i].shaderName);
        size_t shaderId = std::hash<uint32_t>{}(shaders[i].shaderId);

        if (shaders[i].shaderType == Core::Enums::ShaderType::VERTEX)
        {
            HashCombine(vertexShaderHash, shaderTypeHash, shaderNameHash, shaderId);
        }
        else if (shaders[i].shaderType == Core::Enums::ShaderType::FRAGMENT)
        {
            HashCombine(fragmentShaderHash, shaderTypeHash, shaderNameHash, shaderId);
        }
    }

    HashCombine(hash, shaderCountHash, vertexShaderHash, fragmentShaderHash);

    return CheckForPipeLineObjectHash(hash, stateId, *state);
}

int HashManager::FindTessellationStateHash(Core::Wrappers::TessellationState * inputInfo, uint32_t stateId)
{
    std::size_t hash = 0UL;
    hash = std::hash<uint32_t>{}(inputInfo->patchControlPoints);

    int result = CheckForHashExistence<Core::Wrappers::TessellationState>(tessellationStateHashWrapperList, hash, stateId);
    return result;

    //// check if the hash exist in the list 
    //std::vector<PipelineStateHashWrapper<TessellationState>>::iterator it;
    //it = std::find_if(tessellationStateHashWrapperList.begin(), tessellationStateHashWrapperList.end(), [&](PipelineStateHashWrapper<TessellationState> e) { return e.hash == hash; });
    //
    //uint32_t id;

    //if (it != tessellationStateHashWrapperList.end())
    //{
    //    // if yes return the id
    //    id = it->objectId;
    //}
    //else
    //{
    //    // if not add it
    //    PipelineStateHashWrapper<TessellationState> obj = {};
    //    obj.hash = hash;
    //    obj.objectId = stateId;
    //    obj.state = inputInfo->state;

    //    tessellationStateHashWrapperList.push_back(obj);

    //    return -1;
    //}

    //return id;
}

int HashManager::FindViewportStateHash(Core::Wrappers::ViewportState * inputInfo, uint32_t stateId)
{
    std::size_t hash = 0UL;
    std::size_t viewportCountHash = 0UL, scissorCountHash = 0UL;
    std::size_t viewportHashes = 0UL, scissorHashes = 0UL;

    viewportCountHash = std::hash<uint32_t>{}(inputInfo->viewportCount);
    scissorCountHash = std::hash<uint32_t>{}(inputInfo->scissorCount);
    
    /*
    float    x;
    float    y;
    float    width;
    float    height;
    float    minDepth;
    float    maxDepth;
    */
    
    if (inputInfo->viewportCount > 1) // Needs to be investigated as one dynamic state is involved
    {
        viewportHashes = 0UL;
        for (uint32_t i = 0; i < inputInfo->viewportCount - 1; i++)
        {
            size_t x = std::hash<float>{}(inputInfo->pViewports[i].x);
            size_t y = std::hash<float>{}(inputInfo->pViewports[i].y);
            size_t width = std::hash<float>{}(inputInfo->pViewports[i].width);
            size_t height = std::hash<float>{}(inputInfo->pViewports[i].height);
            size_t minDepth = std::hash<float>{}(inputInfo->pViewports[i].minDepth);
            size_t maxDepth = std::hash<float>{}(inputInfo->pViewports[i].maxDepth);

            HashCombine(viewportHashes, x, y, width, height, minDepth, maxDepth);
        }
    }

    /*
    float offsetX;
    float offsetY;
    float lengthX;
    float lengthY;
    */
    if (inputInfo->scissorCount > 1) // Needs to be investigated as one dynamic state is involved
    {
        scissorHashes = 0UL;
        for (uint32_t i = 0; i < inputInfo->scissorCount - 1; i++)
        {
            size_t offsetX = std::hash<float>{}(inputInfo->pViewports[i].x);
            size_t offsetY = std::hash<float>{}(inputInfo->pViewports[i].y);
            size_t lengthX = std::hash<float>{}(inputInfo->pViewports[i].width);
            size_t lengthY = std::hash<float>{}(inputInfo->pViewports[i].height);

            HashCombine(scissorHashes, offsetX, offsetY, lengthX, lengthY);
        }
    }

    HashCombine(hash, viewportCountHash, scissorCountHash, viewportHashes, scissorHashes);

    int result = CheckForHashExistence<Core::Wrappers::ViewportState>(viewportStateHashWrapperList, hash, stateId);
    return result;
    
    //// check if the hash exist in the list 
    //std::vector<PipelineStateHashWrapper<ViewportState>>::iterator it;
    //it = std::find_if(viewportStateHashWrapperList.begin(), viewportStateHashWrapperList.end(), [&](PipelineStateHashWrapper<ViewportState> e) { return e.hash == hash; });

    //if (it != viewportStateHashWrapperList.end())
    //{
    //    return it->objectId;
    //}
    //else
    //{
    //    PipelineStateHashWrapper<ViewportState> obj = {};
    //    obj.hash = hash;
    //    obj.objectId = stateId;

    //    viewportStateHashWrapperList.push_back(obj);
    //    return -1;
    //}

    //return -1;
}

int HashManager::FindRasterizationHash(Core::Wrappers::RasterizationState * inputInfo, uint32_t stateId)
{
    /*
        bool depthClampEnable;
        bool rasterizerDiscardEnable;
        PolygonMode polygonMode;
        CullMode cullMode;
        FrontFace frontFace;
        bool depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
        float lineWidth;
    */

    size_t hash = 0UL;

    size_t depthClampEnableHash = std::hash<bool>{}(inputInfo->depthClampEnable);
    size_t rasterizeHash = std::hash<bool>{}(inputInfo->rasterizerDiscardEnable);
    size_t polygonHash = std::hash<uint32_t>{}((uint32_t)inputInfo->polygonMode);
    size_t cullHash = std::hash<uint32_t>{}((uint32_t)inputInfo->cullMode);
    size_t frontHash = std::hash<uint32_t>{}((uint32_t)inputInfo->frontFace);
    size_t depthBiasEnableHash = std::hash<bool>{}(inputInfo->depthBiasEnable);
    size_t depthConstHash = std::hash<float>{}(inputInfo->depthBiasConstantFactor);
    size_t depthClampHash = std::hash<float>{}(inputInfo->depthBiasClamp);
    size_t depthbiasSlopeHash = std::hash<float>{}(inputInfo->depthBiasSlopeFactor);
    size_t lineWidthHash = std::hash<float>{}(inputInfo->lineWidth);

    HashCombine(hash, depthClampEnableHash, rasterizeHash, polygonHash, cullHash, frontHash,
        depthBiasEnableHash, depthConstHash, depthClampHash, depthbiasSlopeHash, lineWidthHash);

    int result = CheckForHashExistence<Core::Wrappers::RasterizationState>(rasterizationStateHashWrapperList, hash, stateId);
    return result;
}

int HashManager::FindMultiSampleHash(Core::Wrappers::MultiSampleState * inputInfo, uint32_t stateId)
{
    size_t hash = 0UL;
    size_t sampleShadingEnableHash = std::hash<bool>{}(inputInfo->sampleShadingEnable);
    size_t minSampleShadingHash = std::hash<float>{}(inputInfo->minSampleShading);
    size_t alphaToCoverageEnableHash = std::hash<bool>{}(inputInfo->alphaToCoverageEnable);
    size_t alphaToOneEnableHash = std::hash<bool>{}(inputInfo->alphaToOneEnable);
    size_t sampleCountHash = std::hash<uint32_t>{}((uint32_t)inputInfo->sampleCount);
    
    HashCombine(hash, sampleShadingEnableHash, minSampleShadingHash, alphaToCoverageEnableHash,
        alphaToOneEnableHash, sampleCountHash);
    
    /*
    bool sampleShadingEnable;
    float minSampleShading;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
    Samples sampleCount;
    */

    int result = CheckForHashExistence<Core::Wrappers::MultiSampleState>(multisampleStateHashWrapperList, hash, stateId);
    return result;
}

int HashManager::FindDepthStencilHash(Core::Wrappers::DepthStencilState * inputInfo, uint32_t stateId)
{
    /*
    bool depthTestEnable;
    bool depthWriteEnable;
    bool depthBoundsTestEnable;
    bool stencilTestEnable;
    CompareOp depthCompareOp;
    StencilOpState front;
    StencilOpState back;
    float minDepthBounds;
    float maxDepthBounds;
    */
    
    size_t hash = 0UL;

    size_t h1 = std::hash<bool>{}(inputInfo->depthTestEnable);
    size_t h2 = std::hash<bool>{}(inputInfo->depthWriteEnable);
    size_t h3 = std::hash<bool>{}(inputInfo->depthBoundsTestEnable);
    size_t h4 = std::hash<bool>{}(inputInfo->stencilTestEnable);
    size_t h5 = std::hash<uint32_t>{}((uint32_t)inputInfo->depthCompareOp);
    size_t h6 = std::hash<Core::Wrappers::StencilOpState>{}(inputInfo->front);
    size_t h7 = std::hash<Core::Wrappers::StencilOpState>{}(inputInfo->back);
    size_t h8 = std::hash<float>{}(inputInfo->minDepthBounds);
    size_t h9 = std::hash<float>{}(inputInfo->maxDepthBounds);

    HashCombine(hash, h1, h2, h3, h4, h5, h6, h7, h8, h9);

    int result = CheckForHashExistence<Core::Wrappers::DepthStencilState>(depthStencilStateHashWrapperList, hash, stateId);
    return result;
}

int HashManager::FindColorBlendHash(Core::Wrappers::ColorBlendState * inputInfo, uint32_t stateId)
{
    /*
        bool logicOpEnable;
        LogicOp logicOp;
        uint32_t attachmentCount;
        PipelineColorBlendAttachmentState* pAttachments;
        float blendConstants[4];
    */

    size_t hash = 0UL;

    size_t h1 = std::hash<bool>{}(inputInfo->logicOpEnable);
    size_t h2 = std::hash<uint32_t>{}((uint32_t)inputInfo->logicOp);
    size_t h3 = std::hash<uint32_t>{}(inputInfo->attachmentCount);
    size_t h4 = std::hash<float>{}(inputInfo->blendConstants[0]);
    size_t h5 = std::hash<float>{}(inputInfo->blendConstants[1]);
    size_t h6 = std::hash<float>{}(inputInfo->blendConstants[2]);
    size_t h7 = std::hash<float>{}(inputInfo->blendConstants[3]);

    size_t hattachment = 0UL;
    for (uint32_t i = 0; i < inputInfo->attachmentCount; i++)
    {
        size_t h1 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].alphaBlendOp);
        size_t h2 = std::hash<bool>{}(inputInfo->pAttachments[i].blendEnable);
        size_t h3 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].colorBlendOp);
        size_t h4 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].colorWriteMask);
        size_t h5 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].dstAlphaBlendFactor);
        size_t h6 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].dstColorBlendFactor);
        size_t h7 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].srcAlphaBlendFactor);
        size_t h8 = std::hash<uint32_t>{}((uint32_t)inputInfo->pAttachments[i].srcColorBlendFactor);

        HashCombine(hattachment, h1, h2, h3, h4, h5, h6, h7, h8);
    }
    
    HashCombine(hash, h1, h2, h3, h4, h5, h6, h7, hattachment);

    int result = CheckForHashExistence<Core::Wrappers::ColorBlendState>(ColorblendStateHashWrapperList, hash, stateId);
    return result;
}

int HashManager::FindDynamicStateHash(Core::Wrappers::DynamicStateList * inputInfo, uint32_t stateId)
{
    /*
        uint32_t                           dynamicStateCount;
        const DynamicState*                pDynamicStates;
    */

    size_t hash = 0UL;
    size_t hCount = std::hash<uint32_t>{}(inputInfo->dynamicStateCount);

    size_t h0 = 0UL;
    for (uint32_t i = 0; i < inputInfo->dynamicStateCount; i++)
    {
        size_t h1 = std::hash<uint32_t>{}((uint32_t)inputInfo->pDynamicStates[i]);
        HashCombine(h0, h1);
    }

    HashCombine(hash, hCount, h0);

    int result = CheckForHashExistence<Core::Wrappers::DynamicStateList>(dynamicStateHashWrapperList, hash, stateId);
    return result;
}

int HashManager::FindResourceLayoutHash(Core::Wrappers::SetWrapper ** wrapperList, const uint32_t & setCount, uint32_t id)
{
    std::size_t accumulatedHash = 0UL;
    for (uint32_t i = 0; i < setCount; i++)
    {
        Core::Wrappers::SetWrapper obj = *wrapperList[i];
        size_t hash = std::hash<Core::Wrappers::SetWrapper>{}(obj);
        accumulatedHash += hash;
    }
    
    std::vector<SetLayoutCombinationWrapper>::iterator it;
    it = std::find_if(setCombinationWrapperHashList.begin(), setCombinationWrapperHashList.end(), [&](SetLayoutCombinationWrapper e) { return e.hash == accumulatedHash; });

    if (it != setCombinationWrapperHashList.end())
    {
        return it->layoutId;
    }
    else
    {
        SetLayoutCombinationWrapper wrapper = {};
        wrapper.hash = accumulatedHash;
        wrapper.layoutId = id;
 
        for (uint32_t i = 0; i < setCount; i++)
            wrapper.setList.push_back(wrapperList[i]);

        setCombinationWrapperHashList.push_back(wrapper);
        return -1;
    }
}

int HashManager::FindMaterialHash(const std::vector<std::string>* shaderNames, const uint32_t & matId)
{
    size_t hash = 0UL, shaderNameHash = 0UL;
    uint32_t numShaders = (uint32_t)shaderNames->size();
    for (uint32_t i = 0; i < numShaders; i++)
    {
        shaderNameHash += std::hash<std::string>{}(shaderNames->at(i));
    }
    HashCombine(hash, shaderNameHash);

    std::vector<MaterialHashWrapper>::iterator it; 
    it = std::find_if(materialWrapperHashList.begin(), materialWrapperHashList.end(), [&](MaterialHashWrapper e) { return e.hash == hash; });
    
    if (it != materialWrapperHashList.end())
    {
        return it->materialId;
    }
    else
    {
        MaterialHashWrapper wrapper = {};
        wrapper.hash = hash;
        wrapper.materialId = matId;
        materialWrapperHashList.push_back(wrapper);
        return -1;
    }

    return 0;
}

int HashManager::FindDescriptorSetHash(Core::Wrappers::SetWrapper * bindingObj, uint32_t id)
{
    size_t hash = std::hash<Core::Wrappers::SetWrapper>{}(*bindingObj);

    std::vector<DescriptorSetLayoutHashWrapper>::iterator it;
    it = std::find_if(descriptorSetLayoutWrapperHashList.begin(), descriptorSetLayoutWrapperHashList.end(), [&](DescriptorSetLayoutHashWrapper e) { return e.hash == hash; });

    if (it != descriptorSetLayoutWrapperHashList.end())
    {
        return it->objectId;
    }
    else
    {
        DescriptorSetLayoutHashWrapper wrapper = {};
        wrapper.hash = hash;
        wrapper.objectId = id;
        descriptorSetLayoutWrapperHashList.push_back(wrapper);
        return -1;
    }

    return 0;
}

#pragma once
#include <vector>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vulkan/vulkan.h>

///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////


enum class PipelineStateLevel
{
    Global = 0,
    Local = 1,
    None
};

class VulkanGraphicsPipeline
{
private:
    uint32_t objectId;

    VkPipelineCache *                                     pipelineCacheObj;
    VkPipelineShaderStageCreateInfo *                     pipelineShaderStageCreateInfo;
    VkPipelineVertexInputStateCreateInfo *                pipelineVertexInputStateCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo *              pipelineInputAssemblyStateCreateInfo;
    VkPipelineTessellationStateCreateInfo *               pipelineTessellationStateCreateInfo;
    VkPipelineViewportStateCreateInfo *                   pipelineViewportStateCreateInfo;
    VkPipelineRasterizationStateCreateInfo *              pipelineRasterizationStateCreateInfo;
    VkPipelineMultisampleStateCreateInfo *                pipelineMultisampleStateCreateInfo;
    VkPipelineDepthStencilStateCreateInfo *               pipelineDepthStencilStateCreateInfo;
    VkPipelineColorBlendStateCreateInfo *                 pipelineColorBlendStateCreateInfo;
    VkPipelineDynamicStateCreateInfo *                    pipelineDynamicStateCreateInfo;
    VkGraphicsPipelineCreateInfo *                        vulkanGraphicsPipelineCreateInfo;

    std::vector<PipelineStateLevel> stateLevelList;

public:
    void SetPipelineShaderStage(VkPipelineShaderStageCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineTessellationState(VkPipelineTessellationStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineViewportState(VkPipelineViewportStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineRasterizationState(VkPipelineRasterizationStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineMultiSampleState(VkPipelineMultisampleStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineDepthStencilState(VkPipelineDepthStencilStateCreateInfo * info, PipelineStateLevel * level);
    void SetPipelineColorBlendState(VkPipelineColorBlendStateCreateInfo * info, PipelineStateLevel * level);
    void SetDynamicState(VkPipelineDynamicStateCreateInfo * info, PipelineStateLevel * level);
    
    void CreatePipeline();
  
    const uint32_t & GetObjectId()
    {
        return objectId;
    }

    VulkanGraphicsPipeline(const uint32_t & id);
    ~VulkanGraphicsPipeline();
};

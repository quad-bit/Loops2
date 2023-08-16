#include "VulkanGraphicsPipeline.h"
#include "RenderingWrapper.h"

///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////
///////////////////////////////// NOT GETTING USED ///////////////////////////////

void VulkanGraphicsPipeline::SetPipelineShaderStage(VkPipelineShaderStageCreateInfo * info, PipelineStateLevel * level)
{
    pipelineShaderStageCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::ShaderStage] = *level;
}

void VulkanGraphicsPipeline::SetPipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineVertexInputStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::VertexInputState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineInputAssemblyStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::InputAssemblyState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineTessellationState(VkPipelineTessellationStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineTessellationStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::TessellationState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineViewportState(VkPipelineViewportStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineViewportStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::ViewportState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineRasterizationState(VkPipelineRasterizationStateCreateInfo * info, PipelineStateLevel * level)
{ 
    pipelineRasterizationStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::RasterizationState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineMultiSampleState(VkPipelineMultisampleStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineMultisampleStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::MultisampleState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineDepthStencilState(VkPipelineDepthStencilStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineDepthStencilStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::DepthStencilState] = *level;
}

void VulkanGraphicsPipeline::SetPipelineColorBlendState(VkPipelineColorBlendStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineColorBlendStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::ColorBlendState] = *level;
}

void VulkanGraphicsPipeline::SetDynamicState(VkPipelineDynamicStateCreateInfo * info, PipelineStateLevel * level)
{
    pipelineDynamicStateCreateInfo = info;
    stateLevelList[(uint32_t)PipelineStates::DynamicState] = *level;
}

void VulkanGraphicsPipeline::CreatePipeline()
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const uint32_t & id)
{
    objectId = id;
    stateLevelList.resize((uint32_t)PipelineStates::NumStates);
    for each (PipelineStateLevel var in stateLevelList)
    {
        var = PipelineStateLevel::None;
    }
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    delete[] pipelineVertexInputStateCreateInfo->pVertexAttributeDescriptions;
    delete[] pipelineVertexInputStateCreateInfo->pVertexBindingDescriptions;
    delete pipelineVertexInputStateCreateInfo;

    //TODO : Delete states on heap
}

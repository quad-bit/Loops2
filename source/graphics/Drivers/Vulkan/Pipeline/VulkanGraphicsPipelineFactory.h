#pragma once
#include <vulkan\vulkan.h>
#include <vector>
#include <map>
#include <RenderingWrapper.h>
#include <PipelineStateWrappers.h>

class VulkanGraphicsPipeline;
class Shader;

struct VkShaderStageWrapper
{
    uint32_t id;
    VkPipelineShaderStageCreateInfo * shaderStageCreateInfo;
    uint32_t shaderCount;
};

class VulkanGraphicsPipelineFactory
{

private:
    VulkanGraphicsPipelineFactory(){}
    VulkanGraphicsPipelineFactory(VulkanGraphicsPipelineFactory const &) {}
    VulkanGraphicsPipelineFactory const & operator= (VulkanGraphicsPipelineFactory const &) {}

    static VulkanGraphicsPipelineFactory* instance;
/*
    VkPipelineCache                                     pipelineCacheObj;
    VkPipelineShaderStageCreateInfo                     pipelineShaderStageCreateInfo = {};
    VkPipelineVertexInputStateCreateInfo                pipelineVertexInputStateCreateInfo = {};
    VkPipelineInputAssemblyStateCreateInfo              pipelineInputAssemblyStateCreateInfo = {};
    VkPipelineTessellationStateCreateInfo               pipelineTessellationStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo                   pipelineViewportStateCreateInfoObj = {};
    VkPipelineRasterizationStateCreateInfo              pipelineRasterizationStateCreateInfoObj = {};
    VkPipelineMultisampleStateCreateInfo                pipelineMultisampleStateCreateInfo = {};
    VkPipelineDepthStencilStateCreateInfo               pipelineDepthStencilStateCreateInfo = {};
    VkPipelineColorBlendStateCreateInfo                 pipelineColorBlendStateCreateInfoObj = {};
    VkPipelineColorBlendAttachmentState                 attachments[1];
    VkPipelineDynamicStateCreateInfo                    pipelineDynamicStateCreateInfoObj = {};
    VkGraphicsPipelineCreateInfo                        vulkanGraphicsPipelineCreateInfoObj = {};

    void InitPipelineCache();
    void CreatePipelineShaderStage();
    void CreatePipelineVertexInputState();
    void CreatePipelineInputAssemblyState();
    void CreatePipelineTessellationState();
    void CreatePipelineViewportState();
    void CreatePipelineRasterizationState();
    void CreatePipelineMultiSampleState();
    void CreatePipelineDepthStencilState();
    void CreatePipelineColorBlendState();
    void CreateDynamicState();
*/

    std::vector<VulkanGraphicsPipeline *> pipelineList;
    std::map<uint32_t, VkPipelineVertexInputStateCreateInfo *> idToVertexInputMap;
    std::map<uint32_t, VkPipelineInputAssemblyStateCreateInfo *> idToInputAssemblyMap;
    std::vector<VkShaderStageWrapper *> shaderStageWrapperList;
    std::map<uint32_t, VkPipelineTessellationStateCreateInfo *> idToTessellationMap;
    std::map<uint32_t, VkPipelineViewportStateCreateInfo *> idToViewportMap;
    std::map<uint32_t, VkPipelineRasterizationStateCreateInfo *> idToRasterizationMap;
    std::map<uint32_t, VkPipelineMultisampleStateCreateInfo *> idToMultiSampleMap;
    std::map<uint32_t, VkPipelineDepthStencilStateCreateInfo *> idToDepthStencilMap;
    std::map<uint32_t, VkPipelineColorBlendStateCreateInfo *> idToColorBlendMap;
    std::map<uint32_t, VkPipelineDynamicStateCreateInfo *> idToDynamicMap;
    std::map<uint32_t, VkGraphicsPipelineCreateInfo *> idToPipelineInfoMap;
    std::map<uint32_t, VkPipeline *> idToPipelineMap;

    std::vector<VkPipeline *> tempVectorToBeDeleted;

    //deprecated
    VkVertexInputAttributeDescription UnwrapVertexInputAttributeInfo(VertexInputAttributeInfo * info);
    //deprecated
    VkVertexInputBindingDescription UnwrapVertexInputBindingInfo(VertexInputBindingInfo * info);
    
    VkVertexInputAttributeDescription * UnwrapVertexInputAttributeInfo(VertexInputAttributeInfo * info, const uint32_t & count);
    VkVertexInputBindingDescription * UnwrapVertexInputBindingInfo(VertexInputBindingInfo * info, const uint32_t & count);
    VkPrimitiveTopology UnwrapPrimitiveInfo(PrimtiveType * primitive);
    VkCullModeFlags UnwrapCullMode(CullMode mode);
    VkFrontFace UnwrapFrontFace(FrontFace face);
    VkPolygonMode UnwrapPolygonMode(PolygonMode mode);
    VkStencilOpState UnwrapStencilOpState(StencilOpState state);
    VkStencilOp UnwrapStencilOp(StencilOp op);
    VkPipelineColorBlendAttachmentState * UnwrapColorBlendAttachment(PipelineColorBlendAttachmentState * state, const uint32_t & count);
    VkLogicOp UnwrapLogicOp(LogicOp op);
    VkBlendOp UwrapBlendOp(BlendOp op);
    VkColorComponentFlags UnwrapColorComponents(ColorComponentFlagBits bits);
    VkBlendFactor UnwrapBlendFactor(BlendFactor blend);
    VkDynamicState * UnwrapDynamicList(const DynamicState * pDynamicStates, const uint32_t & dynamicStateCount);

    void DestroyVertexInputState();
    void DestroyInputAssemblyState();
    void DestroyShaderState();
    void DestroyTessellationState();
    void DestroyViewportState();
    void DestroyRasterisationState();
    void DestroyDepthStencilState();
    void DestroyColorBlendState();
    void DestroyMultiSampleState();
    void DestroyDynamicState();
    void DestroyPipelines();

public:
    void Init();
    void DeInit();
    void Update();
    static VulkanGraphicsPipelineFactory* GetInstance();
    ~VulkanGraphicsPipelineFactory();

    std::vector<VkDynamicState> dynamicStateList;

    void CreateVertexInputState(const VertexInputWrapper * vertexInputWrapper);
    void CreateInputAssemblyState(const InputAssemblyWrapper * InputAssemblyWrapper);
    void CreateShaderState(const ShaderStateWrapper * shaderStateWrapper);
    void CreateTessellationState(const TessellationStateWrapper * wrapper);
    void CreateViewportState(const ViewPortStateWrapper * wrapper);
    void CreateRasterisationState(const RasterizationStateWrapper * wrapper);
    void CreateDepthStencilState(const DepthStencilStateWrapper * wrapper);
    void CreateColorBlendState(const ColorBlendStateWrapper * wrapper);
    void CreateMultiSampleState(const MultiSampleStateWrapper * wrapper);
    void CreateDynamicState(const DynamicStateWrapper * wrapper);

    void CreatePipeline(PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineId);

    VkPipeline * GetPipeline(const uint32_t & id) ;
};

#include "VulkanGraphicsPipelineFactory.h"
#include "VulkanGraphicsPipeline.h"
#include "RenderingWrapper.h"
#include "VkRenderingUnwrapper.h"
#include <CorePrecompiled.h>
#include "VkShaderFactory.h"
#include "VulkanUtility.h"
#include "VkRenderPassFactory.h"
#include "VkShaderResourceManager.h"

VulkanGraphicsPipelineFactory* VulkanGraphicsPipelineFactory::instance = nullptr;

VkPrimitiveTopology VulkanGraphicsPipelineFactory::UnwrapPrimitiveInfo(PrimtiveType * primitive)
{
    switch (*primitive)
    {
    case PrimtiveType::TOPOLOGY_POINT_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    case PrimtiveType::TOPOLOGY_LINE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    case PrimtiveType::TOPOLOGY_LINE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    case PrimtiveType::TOPOLOGY_TRIANGLE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    case PrimtiveType::TOPOLOGY_TRIANGLE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    case PrimtiveType::TOPOLOGY_TRIANGLE_FAN:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

    case PrimtiveType::TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

    case PrimtiveType::TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

    case PrimtiveType::TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

    case PrimtiveType::TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

    case PrimtiveType::TOPOLOGY_PATCH_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

    case PrimtiveType::TOPOLOGY_MAX_ENUM:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    }

    ASSERT_MSG_DEBUG(0, "invalid primitive");
    return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkCullModeFlags VulkanGraphicsPipelineFactory::UnwrapCullMode(CullMode mode)
{
    VkCullModeFlags vkmode;

    switch (mode)
    {
    case CullMode::CULL_MODE_BACK_BIT:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        break;

    case CullMode::CULL_MODE_FLAG_BITS_MAX_ENUM:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        break;

    case CullMode::CULL_MODE_FRONT_AND_BACK:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
        break;

    case CullMode::CULL_MODE_FRONT_BIT:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
        break;

    case CullMode::CULL_MODE_NONE:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        break;

    default : 
        ASSERT_MSG_DEBUG(0, "Cull mode not found");
    }

    return vkmode;
}
VkFrontFace VulkanGraphicsPipelineFactory::UnwrapFrontFace(FrontFace face)
{
    VkFrontFace vkFace;

    switch (face)
    {
    case FrontFace::FRONT_FACE_CLOCKWISE:
        vkFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        break;
    
    case FrontFace::FRONT_FACE_COUNTER_CLOCKWISE:
        vkFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;

    case FrontFace::FRONT_FACE_MAX_ENUM:
        vkFace = VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
        break;

    default: ASSERT_MSG_DEBUG(0, "Face not found");
    }

    return vkFace;
}

VkPolygonMode VulkanGraphicsPipelineFactory::UnwrapPolygonMode(PolygonMode mode)
{
    VkPolygonMode vkMode;

    switch (mode)
    {
    case PolygonMode::POLYGON_MODE_FILL:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        break;
    case PolygonMode::POLYGON_MODE_FILL_RECTANGLE_NV:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_FILL_RECTANGLE_NV;
        break;
    case PolygonMode::POLYGON_MODE_LINE:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
        break;
    case PolygonMode::POLYGON_MODE_MAX_ENUM:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_MAX_ENUM;
        break;
    case PolygonMode::POLYGON_MODE_POINT:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_POINT;
        break;
    
    default: ASSERT_MSG_DEBUG(0, "Mode not found");

    }

    return vkMode;
}

VkStencilOpState VulkanGraphicsPipelineFactory::UnwrapStencilOpState(StencilOpState state)
{
    VkStencilOpState vkState;
    vkState.compareMask = state.compareMask;
    vkState.compareOp = VulkanUnwrap::UnwrapCompareOp(state.compareOp);
    vkState.depthFailOp = UnwrapStencilOp(state.depthFailOp);
    vkState.failOp = UnwrapStencilOp(state.failOp);
    vkState.passOp = UnwrapStencilOp(state.passOp);
    vkState.reference = state.reference;
    vkState.writeMask = state.writeMask;

    return vkState;
}


VkStencilOp VulkanGraphicsPipelineFactory::UnwrapStencilOp(StencilOp op)
{
    VkStencilOp vkOp;

    switch (op)
    {
    case StencilOp::STENCIL_OP_DECREMENT_AND_CLAMP:
        vkOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        break;

    case StencilOp::STENCIL_OP_DECREMENT_AND_WRAP:
        vkOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
        break;

    case StencilOp::STENCIL_OP_INCREMENT_AND_CLAMP:
        vkOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        break;

    case StencilOp::STENCIL_OP_INCREMENT_AND_WRAP:
        vkOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
        break;

    case StencilOp::STENCIL_OP_INVERT:
        vkOp = VkStencilOp::VK_STENCIL_OP_INVERT;
        break;

    case StencilOp::STENCIL_OP_KEEP:
        vkOp = VkStencilOp::VK_STENCIL_OP_KEEP;
        break;

    case StencilOp::STENCIL_OP_MAX_ENUM:
        vkOp = VkStencilOp::VK_STENCIL_OP_MAX_ENUM;
        break;

    case StencilOp::STENCIL_OP_REPLACE:
        vkOp = VkStencilOp::VK_STENCIL_OP_REPLACE;
        break;

    case StencilOp::STENCIL_OP_ZERO:
        vkOp = VkStencilOp::VK_STENCIL_OP_ZERO;
        break;

    default: ASSERT_MSG_DEBUG(0, "Mode not found");
    }

    return vkOp;
}

VkPipelineColorBlendAttachmentState * VulkanGraphicsPipelineFactory::UnwrapColorBlendAttachment(PipelineColorBlendAttachmentState * state, const uint32_t & count)
{
    VkPipelineColorBlendAttachmentState * vkstates = new VkPipelineColorBlendAttachmentState[count];
    
    for (uint32_t i = 0; i < count; i++)
    {
        vkstates[i].alphaBlendOp = UwrapBlendOp(state[i].alphaBlendOp);
        vkstates[i].blendEnable = state[i].blendEnable;
        vkstates[i].colorBlendOp = UwrapBlendOp(state[i].colorBlendOp);
        vkstates[i].colorWriteMask = UnwrapColorComponents (state[i].colorWriteMask);
        vkstates[i].dstAlphaBlendFactor = UnwrapBlendFactor(state[i].dstAlphaBlendFactor);
        vkstates[i].dstColorBlendFactor = UnwrapBlendFactor(state[i].dstColorBlendFactor);
        vkstates[i].srcAlphaBlendFactor = UnwrapBlendFactor(state[i].srcAlphaBlendFactor);
        vkstates[i].srcColorBlendFactor = UnwrapBlendFactor(state[i].srcColorBlendFactor);
    }

    return vkstates;
}

VkLogicOp VulkanGraphicsPipelineFactory::UnwrapLogicOp(LogicOp op)
{
    VkLogicOp vkOp;

    switch (op)
    {
    case LogicOp::LOGIC_OP_AND:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND;
        break;

    case LogicOp::LOGIC_OP_AND_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND_INVERTED;
        break;

    case LogicOp::LOGIC_OP_AND_REVERSE:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND_REVERSE;
        break;

    case LogicOp::LOGIC_OP_CLEAR:
        vkOp = VkLogicOp::VK_LOGIC_OP_CLEAR;
        break;

    case LogicOp::LOGIC_OP_COPY:
        vkOp = VkLogicOp::VK_LOGIC_OP_COPY;
        break;

    case LogicOp::LOGIC_OP_COPY_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_COPY_INVERTED;
        break;

    case LogicOp::LOGIC_OP_EQUIVALENT:
        vkOp = VkLogicOp::VK_LOGIC_OP_EQUIVALENT;
        break;

    case LogicOp::LOGIC_OP_INVERT:
        vkOp = VkLogicOp::VK_LOGIC_OP_INVERT;
        break;

    case LogicOp::LOGIC_OP_MAX_ENUM:
        vkOp = VkLogicOp::VK_LOGIC_OP_MAX_ENUM;
        break;

    case LogicOp::LOGIC_OP_NAND:
        vkOp = VkLogicOp::VK_LOGIC_OP_NAND;
        break;

    case LogicOp::LOGIC_OP_NOR:
        vkOp = VkLogicOp::VK_LOGIC_OP_NOR;
        break;

    case LogicOp::LOGIC_OP_NO_OP:
        vkOp = VkLogicOp::VK_LOGIC_OP_NO_OP;
        break;

    case LogicOp::LOGIC_OP_OR:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR;
        break;

    case LogicOp::LOGIC_OP_OR_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR_INVERTED;
        break;

    case LogicOp::LOGIC_OP_OR_REVERSE:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR_REVERSE;
        break;

    case LogicOp::LOGIC_OP_SET:
        vkOp = VkLogicOp::VK_LOGIC_OP_SET;
        break;

    case LogicOp::LOGIC_OP_XOR:
        vkOp = VkLogicOp::VK_LOGIC_OP_XOR;
        break;

    default: ASSERT_MSG_DEBUG(0, "op not found");
    }

    return vkOp;
}

VkBlendOp VulkanGraphicsPipelineFactory::UwrapBlendOp(BlendOp op)
{
    VkBlendOp vkOp;

    switch (op)
    {
    case BlendOp::BLEND_OP_ADD:
        vkOp = VkBlendOp::VK_BLEND_OP_ADD;
        break;

    case BlendOp::BLEND_OP_MAX:
        vkOp = VkBlendOp::VK_BLEND_OP_MAX;
        break;

    case BlendOp::BLEND_OP_MAX_ENUM:
        vkOp = VkBlendOp::VK_BLEND_OP_MAX_ENUM;
        break;

    case BlendOp::BLEND_OP_MIN:
        vkOp = VkBlendOp::VK_BLEND_OP_MIN;
        break;

    case BlendOp::BLEND_OP_REVERSE_SUBTRACT:
        vkOp = VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;
        break;
    
    case BlendOp::BLEND_OP_SUBTRACT:
        vkOp = VkBlendOp::VK_BLEND_OP_SUBTRACT;
        break;

    default: ASSERT_MSG_DEBUG(0, "Op not found");
    }

    return vkOp;
}

VkColorComponentFlags VulkanGraphicsPipelineFactory::UnwrapColorComponents(ColorComponentFlagBits flags)
{
    VkColorComponentFlags vkFlags;

    switch (flags)
    {
    case ColorComponentFlagBits::COLOR_COMPONENT_A_BIT :
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
        break;

    case ColorComponentFlagBits::COLOR_COMPONENT_B_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
        break;

    case ColorComponentFlagBits::COLOR_COMPONENT_FLAG_BITS_MAX_ENUM:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
        break;

    case ColorComponentFlagBits::COLOR_COMPONENT_G_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
        break;

    case ColorComponentFlagBits::COLOR_COMPONENT_R_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
        break;

    case ColorComponentFlagBits::COLOR_COMPONENT_ALL_TRUE_BIT:
        vkFlags = 0xf;
        break;

    default: ASSERT_MSG_DEBUG(0, "flag not found");
    }

    return vkFlags;
}

VkBlendFactor VulkanGraphicsPipelineFactory::UnwrapBlendFactor(BlendFactor blend)
{
    VkBlendFactor vkFactor;

    switch(blend)
    {
    case BlendFactor::BLEND_FACTOR_CONSTANT_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_CONSTANT_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_DST_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_DST_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_MAX_ENUM:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_MAX_ENUM;
        break;

    case BlendFactor::BLEND_FACTOR_ONE:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_DST_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_SRC1_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC1_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_SRC1_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC1_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_SRC_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
        break;

    case BlendFactor::BLEND_FACTOR_SRC_ALPHA_SATURATE:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        break;

    case BlendFactor::BLEND_FACTOR_SRC_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
        break;

    case BlendFactor::BLEND_FACTOR_ZERO:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
        break;

    default: ASSERT_MSG_DEBUG(0, "flag not found");
    }

    return vkFactor;
}

VkDynamicState * VulkanGraphicsPipelineFactory::UnwrapDynamicList(const DynamicState * pDynamicStates, const uint32_t & dynamicStateCount)
{
    VkDynamicState * states = new VkDynamicState[dynamicStateCount];

    for (uint32_t i = 0; i < dynamicStateCount; i++)
    {
        switch (pDynamicStates[i])
        {
        case DynamicState::DYNAMIC_STATE_BLEND_CONSTANTS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS;
            break;

        case DynamicState::DYNAMIC_STATE_DEPTH_BIAS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS;
            break;

        case DynamicState::DYNAMIC_STATE_DEPTH_BOUNDS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS;
            break;

        case DynamicState::DYNAMIC_STATE_LINE_WIDTH:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH;
            break;

        case DynamicState::DYNAMIC_STATE_MAX_ENUM:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_MAX_ENUM;
            break;

        case DynamicState::DYNAMIC_STATE_SCISSOR:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_SCISSOR;
            break;

        case DynamicState::DYNAMIC_STATE_STENCIL_COMPARE_MASK:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
            break;

        case DynamicState::DYNAMIC_STATE_STENCIL_REFERENCE:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_REFERENCE;
            break;

        case DynamicState::DYNAMIC_STATE_STENCIL_WRITE_MASK:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
            break;

        case DynamicState::DYNAMIC_STATE_VIEWPORT:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT;
            break;

        default: ASSERT_MSG_DEBUG(0, "State not found");
        }
        
    }
    return states;
}

void VulkanGraphicsPipelineFactory::DestroyVertexInputState()
{
    for each(auto obj in idToVertexInputMap)
    {
        delete[] obj.second->pVertexAttributeDescriptions;
        delete[] obj.second->pVertexBindingDescriptions;
        delete obj.second;
    }

    idToVertexInputMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyInputAssemblyState()
{
    for each(auto obj in idToInputAssemblyMap)
    {
        delete obj.second;
    }

    idToInputAssemblyMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyShaderState()
{
    //shader stage
    {
        std::vector<VkShaderStageWrapper *>::iterator it;
        for (it = shaderStageWrapperList.begin(); it != shaderStageWrapperList.end(); it++)
        {
            delete[](*it)->shaderStageCreateInfo;
            delete (*it);
        }
        shaderStageWrapperList.clear();
    }

}

void VulkanGraphicsPipelineFactory::DestroyTessellationState()
{
    for each(auto obj in idToTessellationMap)
    {
        delete obj.second;
    }

    idToTessellationMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyViewportState()
{
    for each(auto obj in idToViewportMap)
    {
        if (obj.second->pScissors != nullptr)
            delete[] obj.second->pScissors;
        if (obj.second->pViewports != nullptr)
            delete[] obj.second->pViewports;

        delete obj.second;
    }

    idToViewportMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyRasterisationState()
{
    for each(auto obj in idToRasterizationMap)
    {
        delete obj.second;
    }

    idToRasterizationMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyDepthStencilState()
{
    for each(auto obj in idToDepthStencilMap)
    {
        delete obj.second;
    }

    idToDepthStencilMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyColorBlendState()
{
    for each(auto obj in idToColorBlendMap)
    {
        if (obj.second->attachmentCount > 0)
            delete[] obj.second->pAttachments;
        delete obj.second;
    }

    idToColorBlendMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyMultiSampleState()
{
    for each(auto obj in idToMultiSampleMap)
    {
        if (obj.second->pSampleMask != nullptr)
            delete obj.second->pSampleMask;
        delete obj.second;
    }

    idToMultiSampleMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyDynamicState()
{
    for each(auto obj in idToDynamicMap)
    {
        if (obj.second->dynamicStateCount > 0)
            delete[] obj.second->pDynamicStates;
        delete obj.second;
    }

    idToDynamicMap.clear();
}

void VulkanGraphicsPipelineFactory::DestroyPipelines()
{
    for each(auto obj in idToPipelineMap)
    {
        vkDestroyPipeline(*CoreObjects::logicalDeviceObj, *obj.second, CoreObjects::pAllocator);
    }
    idToPipelineMap.clear();

    for each(auto obj in tempVectorToBeDeleted)
    {
        delete[] obj;
    }
    tempVectorToBeDeleted.clear();
}

//deprecated
VkVertexInputAttributeDescription VulkanGraphicsPipelineFactory::UnwrapVertexInputAttributeInfo(VertexInputAttributeInfo * info)
{
    VkVertexInputAttributeDescription obj = {};
    obj.binding = info->binding;
    obj.location = info->location;
    obj.offset = info->offset;
    obj.format = VulkanUnwrap::UnWrapFormat(info->format);

    return obj;
}

VkVertexInputAttributeDescription * VulkanGraphicsPipelineFactory::UnwrapVertexInputAttributeInfo(VertexInputAttributeInfo * info, const uint32_t & count)
{
    VkVertexInputAttributeDescription * inputAttributeDesc = new VkVertexInputAttributeDescription[count];
    for (uint32_t i = 0; i < count; i++)
    {
        inputAttributeDesc[i].binding = info[i].binding;
        inputAttributeDesc[i].location = info[i].location;
        inputAttributeDesc[i].offset = info[i].offset;
        inputAttributeDesc[i].format = VulkanUnwrap::UnWrapFormat(info[i].format);
    }
    return inputAttributeDesc;
}

//deprecated
VkVertexInputBindingDescription VulkanGraphicsPipelineFactory::UnwrapVertexInputBindingInfo(VertexInputBindingInfo * info)
{
    VkVertexInputBindingDescription obj = {};
    obj.binding = info->binding;
    obj.stride = info->stride;

    if (info->inputRate == VertexIputRate::PerVertex)
        obj.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
    else if (info->inputRate == VertexIputRate::PerInstance)
        obj.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
    else
        ASSERT_MSG_DEBUG(0, "Vertex input rate incorrect");

    return obj;
}

VkVertexInputBindingDescription * VulkanGraphicsPipelineFactory::UnwrapVertexInputBindingInfo(VertexInputBindingInfo * info, const uint32_t & count)
{
    VkVertexInputBindingDescription * inputBindingDesc = new VkVertexInputBindingDescription[count];
    for (uint32_t i = 0; i < count; i++)
    {
        inputBindingDesc[i].binding = info->binding;
        inputBindingDesc[i].stride = info->stride;

        if (info->inputRate == VertexIputRate::PerVertex)
            inputBindingDesc[i].inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        else if (info->inputRate == VertexIputRate::PerInstance)
            inputBindingDesc[i].inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        else
            ASSERT_MSG_DEBUG(0, "Vertex input rate incorrect");
    }

    return inputBindingDesc;
}

void VulkanGraphicsPipelineFactory::Init()
{
    PLOGD << "VulkanGraphicsPipelineFactory init";

    //dynamicStateList.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    //dynamicStateList.push_back(VK_DYNAMIC_STATE_SCISSOR);
    
    //VulkanGraphicsPipeline::mandatoryStates.push_back(PipelineStates::VertexInputState);
}

void VulkanGraphicsPipelineFactory::DeInit()
{
    PLOGD << "VulkanGraphicsPipelineFactory Deinit";
    
    DestroyVertexInputState();
    DestroyInputAssemblyState();
    DestroyShaderState();
    DestroyTessellationState();
    DestroyViewportState();
    DestroyRasterisationState();
    DestroyDepthStencilState();
    DestroyColorBlendState();
    DestroyMultiSampleState();
    DestroyDynamicState();
    DestroyPipelines();
/*
    size_t numObjects = pipelineList.size();
    for (uint32_t i = 0; i < numObjects; i++)
    {
        delete pipelineList[i];
    }*/
}

void VulkanGraphicsPipelineFactory::Update()
{

}

VulkanGraphicsPipelineFactory * VulkanGraphicsPipelineFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VulkanGraphicsPipelineFactory();
    }
    return instance;
}

VulkanGraphicsPipelineFactory::~VulkanGraphicsPipelineFactory()
{

}

void VulkanGraphicsPipelineFactory::CreateVertexInputState(const VertexInputWrapper * vertexInputWrapper)
{
    VertexInputState * inputState = vertexInputWrapper->inputState;

    VkPipelineVertexInputStateCreateInfo * pipelineVertexInputStateCreateInfo = new VkPipelineVertexInputStateCreateInfo;
    pipelineVertexInputStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo->pVertexAttributeDescriptions = UnwrapVertexInputAttributeInfo ( inputState->attribInfo, inputState->attribCount);
    pipelineVertexInputStateCreateInfo->pVertexBindingDescriptions = UnwrapVertexInputBindingInfo(inputState->bindingInfo, inputState->bindingCount);
    pipelineVertexInputStateCreateInfo->vertexAttributeDescriptionCount = inputState->attribCount;
    pipelineVertexInputStateCreateInfo->vertexBindingDescriptionCount = inputState->bindingCount;
    pipelineVertexInputStateCreateInfo->pNext = nullptr;
    pipelineVertexInputStateCreateInfo->flags = 0;

    idToVertexInputMap.insert( std::pair<uint32_t, VkPipelineVertexInputStateCreateInfo *>(
    { (vertexInputWrapper)->GetId(), 
        pipelineVertexInputStateCreateInfo }) );
}

void VulkanGraphicsPipelineFactory::CreateInputAssemblyState(const InputAssemblyWrapper * inputAssemblyWrapper)
{
    VkPipelineInputAssemblyStateCreateInfo * pipelineInputAssemblyStateCreateInfo = new VkPipelineInputAssemblyStateCreateInfo;
    InputAssemblyState * inputAssembly = inputAssemblyWrapper->assemblyState;
    pipelineInputAssemblyStateCreateInfo->primitiveRestartEnable = inputAssembly->isPrimtiveRestartEnabled;
    pipelineInputAssemblyStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo->topology = UnwrapPrimitiveInfo(inputAssembly->primitiveType);
    pipelineInputAssemblyStateCreateInfo->flags = 0;
    pipelineInputAssemblyStateCreateInfo->pNext = nullptr;

    idToInputAssemblyMap.insert(std::pair<uint32_t, VkPipelineInputAssemblyStateCreateInfo *>(
    { (inputAssemblyWrapper)->GetId(), pipelineInputAssemblyStateCreateInfo }));
}

void VulkanGraphicsPipelineFactory::CreateShaderState(const ShaderStateWrapper * shaderStateWrapper)
{
    VkShaderStageWrapper * vkStateWrapper = new VkShaderStageWrapper;
    vkStateWrapper->shaderCount = shaderStateWrapper->shaderCount;

    vkStateWrapper->shaderStageCreateInfo = new VkPipelineShaderStageCreateInfo[vkStateWrapper->shaderCount];
    vkStateWrapper->id = shaderStateWrapper->GetId();
    for (uint32_t i = 0; i < vkStateWrapper->shaderCount; i++)
    {
        VkPipelineShaderStageCreateInfo * vkInfo = &vkStateWrapper->shaderStageCreateInfo[i];
        uint32_t shaderId = shaderStateWrapper->shader[i].shaderId;
        vkInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkInfo->pName = "main";
        vkInfo->pNext = nullptr;
        vkInfo->pSpecializationInfo = nullptr;
        vkInfo->flags = 0;
        vkInfo->module = *VkShaderFactory::GetInstance()->GetShaderModule(shaderId) ;
        vkInfo->stage = VkShaderFactory::GetInstance()->GetShaderStageFlag(shaderId);
    }

    shaderStageWrapperList.push_back(vkStateWrapper);
}

void VulkanGraphicsPipelineFactory::CreateTessellationState(const TessellationStateWrapper * wrapper)
{
    VkPipelineTessellationStateCreateInfo * info = new VkPipelineTessellationStateCreateInfo;
    info->flags = 0;
    info->pNext = nullptr;
    info->patchControlPoints = wrapper->tessellationState->patchControlPoints;
    info->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    idToTessellationMap.insert(std::pair<uint32_t, VkPipelineTessellationStateCreateInfo *>
        ({ wrapper->GetId() , info}));
}

void VulkanGraphicsPipelineFactory::CreateViewportState(const ViewPortStateWrapper * wrapper)
{
    VkPipelineViewportStateCreateInfo * info = new VkPipelineViewportStateCreateInfo;
    info->flags = 0;
    info->pNext = nullptr;
    info->scissorCount = wrapper->viewportState->scissorCount;
    info->viewportCount = wrapper->viewportState->viewportCount;
    info->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    if (wrapper->viewportState->pScissors == nullptr)
        info->pScissors = nullptr;
    else
    {
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
    }

    if (wrapper->viewportState->pViewports == nullptr)
        info->pViewports = nullptr;
    else
    {
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
    }
    
    idToViewportMap.insert(std::pair<uint32_t, VkPipelineViewportStateCreateInfo *>({wrapper->GetId(), info}));
}

void VulkanGraphicsPipelineFactory::CreateRasterisationState(const RasterizationStateWrapper * wrapper)
{
    RasterizationState * info = wrapper->rasterState;

    VkPipelineRasterizationStateCreateInfo * vkInfo = new VkPipelineRasterizationStateCreateInfo;
    vkInfo->depthBiasClamp = info->depthBiasClamp;
    vkInfo->cullMode = UnwrapCullMode(info->cullMode);
    vkInfo->depthBiasConstantFactor = info->depthBiasConstantFactor;
    vkInfo->depthBiasEnable = info->depthBiasEnable;
    vkInfo->depthBiasSlopeFactor = info->depthBiasSlopeFactor;
    vkInfo->depthClampEnable = info->depthClampEnable;
    vkInfo->flags = 0;
    vkInfo->frontFace = UnwrapFrontFace(info->frontFace);
    vkInfo->lineWidth = info->lineWidth;
    vkInfo->pNext = nullptr;
    vkInfo->polygonMode = UnwrapPolygonMode(info->polygonMode);
    vkInfo->rasterizerDiscardEnable = info->rasterizerDiscardEnable;
    vkInfo->sType= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    idToRasterizationMap.insert(std::pair<uint32_t, VkPipelineRasterizationStateCreateInfo*>({ wrapper->GetId(), vkInfo }));
}

void VulkanGraphicsPipelineFactory::CreateDepthStencilState(const DepthStencilStateWrapper * wrapper)
{
    DepthStencilState * state = wrapper->depthState;

    VkPipelineDepthStencilStateCreateInfo * info = new VkPipelineDepthStencilStateCreateInfo;
    info->back = UnwrapStencilOpState( state->back);
    info->depthBoundsTestEnable = state->depthBoundsTestEnable;
    info->depthCompareOp = VulkanUnwrap::UnwrapCompareOp( state->depthCompareOp);
    info->depthTestEnable = state->depthTestEnable;
    info->depthWriteEnable = state->depthWriteEnable;
    info->flags = 0;
    info->front = UnwrapStencilOpState(state->front);
    info->maxDepthBounds = state->maxDepthBounds;
    info->minDepthBounds = state->minDepthBounds;
    info->pNext = nullptr;
    info->stencilTestEnable = state->stencilTestEnable;
    info->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    idToDepthStencilMap.insert(std::pair<uint32_t, VkPipelineDepthStencilStateCreateInfo*>({ wrapper->GetId(), info}));
}

void VulkanGraphicsPipelineFactory::CreateColorBlendState(const ColorBlendStateWrapper * wrapper)
{
    ColorBlendState * info = wrapper->colorBlendState;
    
    VkPipelineColorBlendStateCreateInfo * vkinfo = new VkPipelineColorBlendStateCreateInfo;
    vkinfo->attachmentCount = info->attachmentCount;
    vkinfo->blendConstants[0] = info->blendConstants[0];
    vkinfo->blendConstants[1] = info->blendConstants[1];
    vkinfo->blendConstants[2] = info->blendConstants[2];
    vkinfo->blendConstants[3] = info->blendConstants[3];
    vkinfo->flags = 0;
    vkinfo->logicOp = UnwrapLogicOp(info->logicOp);
    vkinfo->logicOpEnable = info->logicOpEnable;
    vkinfo->pAttachments = UnwrapColorBlendAttachment(info->pAttachments, info->attachmentCount);
    vkinfo->pNext = nullptr;
    vkinfo->sType= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    idToColorBlendMap.insert(std::pair<uint32_t, VkPipelineColorBlendStateCreateInfo*>({ wrapper->GetId(), vkinfo }));
}

void VulkanGraphicsPipelineFactory::CreateMultiSampleState(const MultiSampleStateWrapper * wrapper)
{
    MultiSampleState * info = wrapper->multiSampleState;

    VkPipelineMultisampleStateCreateInfo * vkinfo = new VkPipelineMultisampleStateCreateInfo;
    vkinfo->alphaToCoverageEnable = info->alphaToCoverageEnable;
    vkinfo->alphaToOneEnable= info->alphaToOneEnable;
    vkinfo->flags = 0;
    vkinfo->minSampleShading = info->minSampleShading;
    vkinfo->pNext = nullptr;
    vkinfo->pSampleMask = NULL;
    vkinfo->rasterizationSamples = VulkanUnwrap::UnWrapSampleCount( info->sampleCount);
    vkinfo->sampleShadingEnable = info->sampleShadingEnable;
    vkinfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    idToMultiSampleMap.insert(std::pair<uint32_t, VkPipelineMultisampleStateCreateInfo *>({ wrapper->GetId(), vkinfo }));
}

void VulkanGraphicsPipelineFactory::CreateDynamicState(const DynamicStateWrapper * wrapper)
{
    DynamicStateList * list = wrapper->dynamicStates;

    VkPipelineDynamicStateCreateInfo * info = new VkPipelineDynamicStateCreateInfo;
    info->dynamicStateCount = list->dynamicStateCount;
    info->flags = 0;
    info->pDynamicStates = UnwrapDynamicList(list->pDynamicStates, list->dynamicStateCount);
    info->pNext = nullptr;
    info->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    idToDynamicMap.insert(std::pair<uint32_t, VkPipelineDynamicStateCreateInfo *>({ wrapper->GetId(), info }));
}

void VulkanGraphicsPipelineFactory::CreatePipeline(PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineIds)
{
    std::vector<VkGraphicsPipelineCreateInfo> infoList;
    infoList.resize(pipelineCount);
    
    VkPipeline * list = new VkPipeline[pipelineCount];

    for (uint32_t i = 0; i < pipelineCount; i++)
    {
        std::vector<VkShaderStageWrapper *>::iterator it;

        it = std::find_if(shaderStageWrapperList.begin(), shaderStageWrapperList.end(), [&](VkShaderStageWrapper * e) {
            return e->id == info[i].statesToIdMap[PipelineStates::ShaderStage]; });

        ASSERT_MSG_DEBUG(it != shaderStageWrapperList.end(), "Shader stage id not found");

        VkPipelineLayout layout = *VkShaderResourceManager::GetInstance()->GetPipelineLayout(info[i].statesToIdMap[PipelineStates::ShaderResourcesLayout]);
        VkRenderPass renderpass = *VkRenderPassFactory::GetInstance()->GetRenderPass(info[i].renderPassId);

        VkGraphicsPipelineCreateInfo vkinfo = {};
        vkinfo.basePipelineHandle = VK_NULL_HANDLE;
        vkinfo.basePipelineIndex = -1;
        vkinfo.flags = 0;
        vkinfo.layout = layout;
        vkinfo.pColorBlendState = idToColorBlendMap[info[i].statesToIdMap[PipelineStates::ColorBlendState]];
        vkinfo.pDepthStencilState = idToDepthStencilMap[info[i].statesToIdMap[PipelineStates::DepthStencilState]];
        vkinfo.pDynamicState = idToDynamicMap[info[i].statesToIdMap[PipelineStates::DynamicState]];
        vkinfo.pInputAssemblyState = idToInputAssemblyMap[info[i].statesToIdMap[PipelineStates::InputAssemblyState]];
        vkinfo.pMultisampleState = idToMultiSampleMap[info[i].statesToIdMap[PipelineStates::MultisampleState]];
        vkinfo.pNext = nullptr;
        vkinfo.pRasterizationState = idToRasterizationMap[info[i].statesToIdMap[PipelineStates::RasterizationState]];
        vkinfo.pStages = (*it)->shaderStageCreateInfo;
        vkinfo.pTessellationState = idToTessellationMap[info[i].statesToIdMap[PipelineStates::TessellationState]];
        vkinfo.pVertexInputState = idToVertexInputMap[info[i].statesToIdMap[PipelineStates::VertexInputState]];
        vkinfo.pViewportState = idToViewportMap[info[i].statesToIdMap[PipelineStates::ViewportState]];
        vkinfo.renderPass = renderpass;
        vkinfo.stageCount = (*it)->shaderCount;
        vkinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vkinfo.subpass = info[i].subpassId;

        infoList[i] = vkinfo;
    }

    VkPipelineCache cache = VK_NULL_HANDLE;

    ErrorCheck(vkCreateGraphicsPipelines(
        *CoreObjects::logicalDeviceObj,
        cache,
        (uint32_t)infoList.size(),
        infoList.data(),
        CoreObjects::pAllocator,
        list));
    
    for (uint32_t i = 0; i < pipelineCount; i++)
    {
        idToPipelineMap.insert(std::pair<uint32_t, VkPipeline *>({pipelineIds[i], &list[i]}));
    }

    tempVectorToBeDeleted.push_back(list);
}

VkPipeline * VulkanGraphicsPipelineFactory::GetPipeline(const uint32_t & id) 
{
    VkPipeline * pipeline = idToPipelineMap[id];
    ASSERT_MSG_DEBUG(pipeline != nullptr, "Id not found");

    return pipeline;
}

/*
void VulkanGraphicsPipelineFactory::InitPipelineCache()
{

}

void VulkanGraphicsPipelineFactory::CreatePipelineShaderStage()
{

}

void VulkanGraphicsPipelineFactory::CreateDynamicState()
{
    pipelineDynamicStateCreateInfoObj.dynamicStateCount = (uint32_t)dynamicStateList.size();
    pipelineDynamicStateCreateInfoObj.pDynamicStates = dynamicStateList.data();
    pipelineDynamicStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}



void VulkanGraphicsPipelineFactory::CreatePipelineVertexInputState()
{

}

void VulkanGraphicsPipelineFactory::CreatePipelineInputAssemblyState()
{
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void VulkanGraphicsPipelineFactory::CreatePipelineTessellationState()
{
}

void VulkanGraphicsPipelineFactory::CreatePipelineRasterizationState()
{
    pipelineRasterizationStateCreateInfoObj.cullMode = VK_CULL_MODE_BACK_BIT;
    pipelineRasterizationStateCreateInfoObj.depthBiasClamp = 0;
    pipelineRasterizationStateCreateInfoObj.depthBiasConstantFactor = 0;
    pipelineRasterizationStateCreateInfoObj.depthBiasEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfoObj.depthBiasSlopeFactor = 0;
    pipelineRasterizationStateCreateInfoObj.depthClampEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfoObj.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationStateCreateInfoObj.lineWidth = 1.0f;
    pipelineRasterizationStateCreateInfoObj.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfoObj.rasterizerDiscardEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
}

void VulkanGraphicsPipelineFactory::CreatePipelineColorBlendState()
{
    attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
    attachments[0].blendEnable = VK_FALSE;
    attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
    attachments[0].colorWriteMask = 0xf;
    attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;

    pipelineColorBlendStateCreateInfoObj.attachmentCount = 1;
    pipelineColorBlendStateCreateInfoObj.blendConstants[0] = 1.0f;
    pipelineColorBlendStateCreateInfoObj.blendConstants[1] = 1.0f;
    pipelineColorBlendStateCreateInfoObj.blendConstants[2] = 1.0f;
    pipelineColorBlendStateCreateInfoObj.blendConstants[3] = 1.0f;
    pipelineColorBlendStateCreateInfoObj.logicOp = VK_LOGIC_OP_NO_OP;
    pipelineColorBlendStateCreateInfoObj.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfoObj.pAttachments = attachments;
    pipelineColorBlendStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
}

void VulkanGraphicsPipelineFactory::CreatePipelineViewportState()
{
    pipelineViewportStateCreateInfoObj.pScissors = nullptr;
    pipelineViewportStateCreateInfoObj.pViewports = nullptr;
    pipelineViewportStateCreateInfoObj.scissorCount = 1;
    pipelineViewportStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfoObj.viewportCount = 1;
}

void VulkanGraphicsPipelineFactory::CreatePipelineDepthStencilState()
{
    pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineDepthStencilStateCreateInfo.pNext = NULL;
    pipelineDepthStencilStateCreateInfo.flags = 0;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.minDepthBounds = 0;
    pipelineDepthStencilStateCreateInfo.maxDepthBounds = 0;
    pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    pipelineDepthStencilStateCreateInfo.back.compareMask = 0;
    pipelineDepthStencilStateCreateInfo.back.reference = 0;
    pipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    pipelineDepthStencilStateCreateInfo.back.writeMask = 0;
    pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;
}

void VulkanGraphicsPipelineFactory::CreatePipelineMultiSampleState()
{
    pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.pNext = NULL;
    pipelineMultisampleStateCreateInfo.flags = 0;
    pipelineMultisampleStateCreateInfo.pSampleMask = NULL;
    pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.minSampleShading = 0.0;
}
*/
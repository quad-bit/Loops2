#include "pipeline/VulkanGraphicsPipelineFactory.h"
#include "pipeline/VulkanGraphicsPipeline.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "utility/VkRenderingUnwrapper.h"
#include <CorePrecompiled.h>
#include "shading/VkShaderFactory.h"
#include "utility/VulkanUtility.h"
#include "VkRenderPassFactory.h"
#include "shading/VkShaderResourceManager.h"

GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory* GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::instance = nullptr;

VkPrimitiveTopology GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapPrimitiveInfo(Core::Enums::PrimtiveType * primitive)
{
    switch (*primitive)
    {
    case Core::Enums::PrimtiveType::TOPOLOGY_POINT_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    case Core::Enums::PrimtiveType::TOPOLOGY_LINE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    case Core::Enums::PrimtiveType::TOPOLOGY_LINE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    case Core::Enums::PrimtiveType::TOPOLOGY_TRIANGLE_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    case Core::Enums::PrimtiveType::TOPOLOGY_TRIANGLE_STRIP:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    case Core::Enums::PrimtiveType::TOPOLOGY_TRIANGLE_FAN:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

    case Core::Enums::PrimtiveType::TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

    case Core::Enums::PrimtiveType::TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

    case Core::Enums::PrimtiveType::TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

    case Core::Enums::PrimtiveType::TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

    case Core::Enums::PrimtiveType::TOPOLOGY_PATCH_LIST:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

    case Core::Enums::PrimtiveType::TOPOLOGY_MAX_ENUM:
        return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    }

    ASSERT_MSG_DEBUG(0, "invalid primitive");
    return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkCullModeFlags GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapCullMode(Core::Enums::CullMode mode)
{
    VkCullModeFlags vkmode;

    switch (mode)
    {
    case Core::Enums::CullMode::CULL_MODE_BACK_BIT:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        break;

    case Core::Enums::CullMode::CULL_MODE_FLAG_BITS_MAX_ENUM:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        break;

    case Core::Enums::CullMode::CULL_MODE_FRONT_AND_BACK:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
        break;

    case Core::Enums::CullMode::CULL_MODE_FRONT_BIT:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
        break;

    case Core::Enums::CullMode::CULL_MODE_NONE:
        vkmode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
        break;

    default : 
        ASSERT_MSG_DEBUG(0, "Cull mode not found");
    }

    return vkmode;
}
VkFrontFace GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapFrontFace(Core::Enums::FrontFace face)
{
    VkFrontFace vkFace;

    switch (face)
    {
    case Core::Enums::FrontFace::FRONT_FACE_CLOCKWISE:
        vkFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        break;
    
    case Core::Enums::FrontFace::FRONT_FACE_COUNTER_CLOCKWISE:
        vkFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;

    case Core::Enums::FrontFace::FRONT_FACE_MAX_ENUM:
        vkFace = VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
        break;

    default: ASSERT_MSG_DEBUG(0, "Face not found");
    }

    return vkFace;
}

VkPolygonMode GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapPolygonMode(Core::Enums::PolygonMode mode)
{
    VkPolygonMode vkMode;

    switch (mode)
    {
    case Core::Enums::PolygonMode::POLYGON_MODE_FILL:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        break;
    case Core::Enums::PolygonMode::POLYGON_MODE_FILL_RECTANGLE_NV:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_FILL_RECTANGLE_NV;
        break;
    case Core::Enums::PolygonMode::POLYGON_MODE_LINE:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
        break;
    case Core::Enums::PolygonMode::POLYGON_MODE_MAX_ENUM:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_MAX_ENUM;
        break;
    case Core::Enums::PolygonMode::POLYGON_MODE_POINT:
        vkMode = VkPolygonMode::VK_POLYGON_MODE_POINT;
        break;
    
    default: ASSERT_MSG_DEBUG(0, "Mode not found");

    }

    return vkMode;
}

VkStencilOpState GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapStencilOpState(Core::Wrappers::StencilOpState state)
{
    VkStencilOpState vkState;
    vkState.compareMask = state.compareMask;
    vkState.compareOp = GfxVk::Unwrap::UnwrapCompareOp(state.compareOp);
    vkState.depthFailOp = UnwrapStencilOp(state.depthFailOp);
    vkState.failOp = UnwrapStencilOp(state.failOp);
    vkState.passOp = UnwrapStencilOp(state.passOp);
    vkState.reference = state.reference;
    vkState.writeMask = state.writeMask;

    return vkState;
}


VkStencilOp GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapStencilOp(Core::Enums::StencilOp op)
{
    VkStencilOp vkOp;

    switch (op)
    {
    case Core::Enums::StencilOp::STENCIL_OP_DECREMENT_AND_CLAMP:
        vkOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_DECREMENT_AND_WRAP:
        vkOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_INCREMENT_AND_CLAMP:
        vkOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_INCREMENT_AND_WRAP:
        vkOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_INVERT:
        vkOp = VkStencilOp::VK_STENCIL_OP_INVERT;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_KEEP:
        vkOp = VkStencilOp::VK_STENCIL_OP_KEEP;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_MAX_ENUM:
        vkOp = VkStencilOp::VK_STENCIL_OP_MAX_ENUM;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_REPLACE:
        vkOp = VkStencilOp::VK_STENCIL_OP_REPLACE;
        break;

    case Core::Enums::StencilOp::STENCIL_OP_ZERO:
        vkOp = VkStencilOp::VK_STENCIL_OP_ZERO;
        break;

    default: ASSERT_MSG_DEBUG(0, "Mode not found");
    }

    return vkOp;
}

VkPipelineColorBlendAttachmentState * GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapColorBlendAttachment(Core::Wrappers::PipelineColorBlendAttachmentState * state, const uint32_t & count)
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

VkLogicOp GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapLogicOp(Core::Enums::LogicOp op)
{
    VkLogicOp vkOp;

    switch (op)
    {
    case Core::Enums::LogicOp::LOGIC_OP_AND:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_AND_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND_INVERTED;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_AND_REVERSE:
        vkOp = VkLogicOp::VK_LOGIC_OP_AND_REVERSE;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_CLEAR:
        vkOp = VkLogicOp::VK_LOGIC_OP_CLEAR;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_COPY:
        vkOp = VkLogicOp::VK_LOGIC_OP_COPY;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_COPY_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_COPY_INVERTED;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_EQUIVALENT:
        vkOp = VkLogicOp::VK_LOGIC_OP_EQUIVALENT;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_INVERT:
        vkOp = VkLogicOp::VK_LOGIC_OP_INVERT;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_MAX_ENUM:
        vkOp = VkLogicOp::VK_LOGIC_OP_MAX_ENUM;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_NAND:
        vkOp = VkLogicOp::VK_LOGIC_OP_NAND;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_NOR:
        vkOp = VkLogicOp::VK_LOGIC_OP_NOR;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_NO_OP:
        vkOp = VkLogicOp::VK_LOGIC_OP_NO_OP;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_OR:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_OR_INVERTED:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR_INVERTED;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_OR_REVERSE:
        vkOp = VkLogicOp::VK_LOGIC_OP_OR_REVERSE;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_SET:
        vkOp = VkLogicOp::VK_LOGIC_OP_SET;
        break;

    case Core::Enums::LogicOp::LOGIC_OP_XOR:
        vkOp = VkLogicOp::VK_LOGIC_OP_XOR;
        break;

    default: ASSERT_MSG_DEBUG(0, "op not found");
    }

    return vkOp;
}

VkBlendOp GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UwrapBlendOp(Core::Enums::BlendOp op)
{
    VkBlendOp vkOp;

    switch (op)
    {
    case Core::Enums::BlendOp::BLEND_OP_ADD:
        vkOp = VkBlendOp::VK_BLEND_OP_ADD;
        break;

    case Core::Enums::BlendOp::BLEND_OP_MAX:
        vkOp = VkBlendOp::VK_BLEND_OP_MAX;
        break;

    case Core::Enums::BlendOp::BLEND_OP_MAX_ENUM:
        vkOp = VkBlendOp::VK_BLEND_OP_MAX_ENUM;
        break;

    case Core::Enums::BlendOp::BLEND_OP_MIN:
        vkOp = VkBlendOp::VK_BLEND_OP_MIN;
        break;

    case Core::Enums::BlendOp::BLEND_OP_REVERSE_SUBTRACT:
        vkOp = VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;
        break;
    
    case Core::Enums::BlendOp::BLEND_OP_SUBTRACT:
        vkOp = VkBlendOp::VK_BLEND_OP_SUBTRACT;
        break;

    default: ASSERT_MSG_DEBUG(0, "Op not found");
    }

    return vkOp;
}

VkColorComponentFlags GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapColorComponents(Core::Enums::ColorComponentFlagBits flags)
{
    VkColorComponentFlags vkFlags;

    switch (flags)
    {
    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_A_BIT :
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
        break;

    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_B_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
        break;

    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_FLAG_BITS_MAX_ENUM:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
        break;

    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_G_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
        break;

    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_R_BIT:
        vkFlags = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
        break;

    case Core::Enums::ColorComponentFlagBits::COLOR_COMPONENT_ALL_TRUE_BIT:
        vkFlags = 0xf;
        break;

    default: ASSERT_MSG_DEBUG(0, "flag not found");
    }

    return vkFlags;
}

VkBlendFactor GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapBlendFactor(Core::Enums::BlendFactor blend)
{
    VkBlendFactor vkFactor;

    switch(blend)
    {
    case Core::Enums::BlendFactor::BLEND_FACTOR_CONSTANT_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_CONSTANT_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_DST_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_DST_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_MAX_ENUM:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_MAX_ENUM;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_DST_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_SRC1_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC1_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_SRC1_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC1_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_SRC_ALPHA:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_SRC_ALPHA_SATURATE:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_SRC_COLOR:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
        break;

    case Core::Enums::BlendFactor::BLEND_FACTOR_ZERO:
        vkFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
        break;

    default: ASSERT_MSG_DEBUG(0, "flag not found");
    }

    return vkFactor;
}

VkDynamicState * GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapDynamicList(const Core::Enums::DynamicState * pDynamicStates, const uint32_t & dynamicStateCount)
{
    VkDynamicState * states = new VkDynamicState[dynamicStateCount];

    for (uint32_t i = 0; i < dynamicStateCount; i++)
    {
        switch (pDynamicStates[i])
        {
        case Core::Enums::DynamicState::DYNAMIC_STATE_BLEND_CONSTANTS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_DEPTH_BIAS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_DEPTH_BOUNDS:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_LINE_WIDTH:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_MAX_ENUM:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_MAX_ENUM;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_SCISSOR:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_SCISSOR;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_STENCIL_COMPARE_MASK:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_STENCIL_REFERENCE:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_REFERENCE;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_STENCIL_WRITE_MASK:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
            break;

        case Core::Enums::DynamicState::DYNAMIC_STATE_VIEWPORT:
            states[i] = VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT;
            break;

        default: ASSERT_MSG_DEBUG(0, "State not found");
        }
        
    }
    return states;
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyVertexInputState()
{
    for each(auto obj in idToVertexInputMap)
    {
        if (obj.first == 0)
            continue;
        delete[] obj.second.pVertexAttributeDescriptions;
        delete[] obj.second.pVertexBindingDescriptions;
    }

    idToVertexInputMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyInputAssemblyState()
{
    /*for each(auto obj in idToInputAssemblyMap)
    {
        delete obj.second;
    }*/

    idToInputAssemblyMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyShaderState()
{
    //shader stage
    //{
    //    std::vector<VkShaderStageWrapper *>::iterator it;
    //    for (it = shaderStageWrapperList.begin(); it != shaderStageWrapperList.end(); it++)
    //    {
    //        delete[](*it)->shaderStageCreateInfo;
    //        delete (*it);
    //    }
    //    shaderStageWrapperList.clear();
    //}

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyTessellationState()
{
    //for each(auto obj in idToTessellationMap)
    //{
    //    delete obj.second;
    //}

    idToTessellationMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyViewportState()
{
    for each(auto obj in idToViewportMap)
    {
        if (obj.first == 0)
            continue;
        if (obj.second.pScissors != nullptr)
            delete[] obj.second.pScissors;
        if (obj.second.pViewports != nullptr)
            delete[] obj.second.pViewports;

        //delete obj.second;
    }

    idToViewportMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyRasterisationState()
{
    //for each(auto obj in idToRasterizationMap)
    //{
    //    delete obj.second;
    //}

    idToRasterizationMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyDepthStencilState()
{
    //for each(auto obj in idToDepthStencilMap)
    //{
    //    delete obj.second;
    //}

    idToDepthStencilMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyColorBlendState()
{
    for each(auto obj in idToColorBlendMap)
    {
        if (obj.first == 0)
            continue;

        if (obj.second.attachmentCount > 0)
            delete[] obj.second.pAttachments;
        //delete obj.second;
    }

    idToColorBlendMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyMultiSampleState()
{
    for each(auto obj in idToMultiSampleMap)
    {
        if (obj.first == 0)
            continue;

        if (obj.second.pSampleMask != nullptr)
            delete obj.second.pSampleMask;
        //delete obj.second;
    }

    idToMultiSampleMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyDynamicState()
{
    for each(auto obj in idToDynamicMap)
    {
        if (obj.first == 0)
            continue;

        if (obj.second.dynamicStateCount > 0)
            delete[] obj.second.pDynamicStates;
        //delete obj.second;
    }

    idToDynamicMap.clear();
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DestroyPipelines()
{
    for each(auto obj in idToPipelineMap)
    {
        vkDestroyPipeline(DeviceInfo::GetLogicalDevice(), obj.second, DeviceInfo::GetAllocationCallback());
    }
    idToPipelineMap.clear();

    for each(auto obj in tempVectorToBeDeleted)
    {
        delete[] obj;
    }
    tempVectorToBeDeleted.clear();
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetStateId(const Core::Enums::PipelineStates& state)
{
    auto id = 0;
    switch (state)
    {
    case Core::Enums::PipelineStates::ColorBlendState:
        id = m_pipelineColorBlendStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::DepthStencilState:
        id = m_pipelineDepthStencilStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::DynamicState:
        id = m_pipelineDynamicStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::InputAssemblyState:
        id = m_pipelineInputAssemblyStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::MultisampleState:
        id = m_pipelineMultisampleStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::RasterizationState:
        id = m_pipelineRasterizationStateCreateInfoObjCounter++;
        break;
    case Core::Enums::PipelineStates::TessellationState:
        id = m_pipelineTessellationStateCreateInfoCounter++;
        break;
    case Core::Enums::PipelineStates::ViewportState:
        id = m_pipelineViewportStateCreateInfoObjCounter++;
        break;
    default:
        ASSERT_MSG_DEBUG(0, "state not found");
    }
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetGraphicsPipelineId()
{
    return m_graphicsPipelineId++;
}

//deprecated
VkVertexInputAttributeDescription GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapVertexInputAttributeInfo(Core::Wrappers::VertexInputAttributeInfo * info)
{
    VkVertexInputAttributeDescription obj = {};
    obj.binding = info->binding;
    obj.location = info->location;
    obj.offset = info->offset;
    obj.format = GfxVk::Unwrap::UnWrapFormat(info->format);

    return obj;
}

VkVertexInputAttributeDescription * GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapVertexInputAttributeInfo(Core::Wrappers::VertexInputAttributeInfo * info, const uint32_t & count)
{
    VkVertexInputAttributeDescription * inputAttributeDesc = new VkVertexInputAttributeDescription[count];
    for (uint32_t i = 0; i < count; i++)
    {
        inputAttributeDesc[i].binding = info[i].binding;
        inputAttributeDesc[i].location = info[i].location;
        inputAttributeDesc[i].offset = info[i].offset;
        inputAttributeDesc[i].format = GfxVk::Unwrap::UnWrapFormat(info[i].format);
    }
    return inputAttributeDesc;
}

//deprecated
VkVertexInputBindingDescription GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapVertexInputBindingInfo(Core::Wrappers::VertexInputBindingInfo * info)
{
    VkVertexInputBindingDescription obj = {};
    obj.binding = info->binding;
    obj.stride = info->stride;

    if (info->inputRate == Core::Enums::VertexIputRate::PerVertex)
        obj.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
    else if (info->inputRate == Core::Enums::VertexIputRate::PerInstance)
        obj.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
    else
        ASSERT_MSG_DEBUG(0, "Vertex input rate incorrect");

    return obj;
}

VkVertexInputBindingDescription * GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::UnwrapVertexInputBindingInfo(Core::Wrappers::VertexInputBindingInfo * info, const uint32_t & count)
{
    VkVertexInputBindingDescription * inputBindingDesc = new VkVertexInputBindingDescription[count];
    for (uint32_t i = 0; i < count; i++)
    {
        inputBindingDesc[i].binding = info->binding;
        inputBindingDesc[i].stride = info->stride;

        if (info->inputRate == Core::Enums::VertexIputRate::PerVertex)
            inputBindingDesc[i].inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        else if (info->inputRate == Core::Enums::VertexIputRate::PerInstance)
            inputBindingDesc[i].inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        else
            ASSERT_MSG_DEBUG(0, "Vertex input rate incorrect");
    }

    return inputBindingDesc;
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::Init(const Core::WindowSettings& windowSettings)
{
    PLOGD << "VulkanGraphicsPipelineFactory init";

    m_windowSettings = windowSettings;

    dynamicStateList.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateList.push_back(VK_DYNAMIC_STATE_SCISSOR);

    CreatePipelineVertexInputState();
    CreatePipelineInputAssemblyState();
    CreatePipelineTessellationState();
    CreatePipelineViewportState();
    CreatePipelineRasterizationState();
    CreatePipelineMultiSampleState();
    CreatePipelineDepthStencilState();
    CreatePipelineColorBlendState();
    CreateDynamicState();
    //VulkanGraphicsPipeline::mandatoryStates.push_back(PipelineStates::VertexInputState);

    //TessellationStateCreateInfo
    //{
    //    m_pipelineTessellationStateCreateInfo.flags = 0;
    //    m_pipelineTessellationStateCreateInfo.patchControlPoints = 0;
    //    m_pipelineTessellationStateCreateInfo.pNext = nullptr;
    //    m_pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    //}

    ////ViewportStateCreateInfo
    //{
    //    m_defaultScissor = { 0, 0, m_windowSettings.m_renderWidth, m_windowSettings.m_renderHeight };
    //    m_defaultViewport = { 0, 0, (float)m_windowSettings.m_renderWidth, (float)m_windowSettings.m_renderHeight, 0.0f, 1.0f };

    //    m_pipelineViewportStateCreateInfoObj.flags = 0;
    //    m_pipelineViewportStateCreateInfoObj.pNext = nullptr;
    //    m_pipelineViewportStateCreateInfoObj.pScissors = &m_defaultScissor;
    //    m_pipelineViewportStateCreateInfoObj.pViewports = &m_defaultViewport;
    //    m_pipelineViewportStateCreateInfoObj.scissorCount = 1;
    //    m_pipelineViewportStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //    m_pipelineViewportStateCreateInfoObj.viewportCount = 1;
    //}

    ////RasterizationStateCreateInfo 
    //{
    //    m_pipelineRasterizationStateCreateInfoObj.cullMode = VK_CULL_MODE_BACK_BIT;
    //    m_pipelineRasterizationStateCreateInfoObj.depthBiasClamp = 0;
    //    m_pipelineRasterizationStateCreateInfoObj.depthBiasConstantFactor = 0;
    //    m_pipelineRasterizationStateCreateInfoObj.depthBiasEnable = VK_FALSE;
    //    m_pipelineRasterizationStateCreateInfoObj.depthBiasSlopeFactor = 0;
    //    m_pipelineRasterizationStateCreateInfoObj.depthClampEnable = VK_FALSE;
    //    m_pipelineRasterizationStateCreateInfoObj.flags = 0;
    //    m_pipelineRasterizationStateCreateInfoObj.frontFace = VK_FRONT_FACE_CLOCKWISE;
    //    m_pipelineRasterizationStateCreateInfoObj.lineWidth = 1.0f;
    //    m_pipelineRasterizationStateCreateInfoObj.pNext = nullptr;
    //    m_pipelineRasterizationStateCreateInfoObj.polygonMode = VK_POLYGON_MODE_FILL;
    //    m_pipelineRasterizationStateCreateInfoObj.rasterizerDiscardEnable = VK_FALSE;
    //    m_pipelineRasterizationStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //}

    ////MultisampleStateCreateInfo
    //{
    //    m_pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    //    m_pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    //    m_pipelineMultisampleStateCreateInfo.flags = 0;
    //    m_pipelineMultisampleStateCreateInfo.minSampleShading = 0.5f;
    //    m_pipelineMultisampleStateCreateInfo.pNext = nullptr;
    //    m_pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
    //    m_pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //    m_pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    //    m_pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    //}

    ////DepthStencilStateCreateInfo
    //{
    //    m_pipelineDepthStencilStateCreateInfo.back.compareMask = 0;
    //    m_pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    //    m_pipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    //    m_pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    //    m_pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    //    m_pipelineDepthStencilStateCreateInfo.back.reference = 0;
    //    m_pipelineDepthStencilStateCreateInfo.back.writeMask = 0;

    //    m_pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    //    m_pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    //    m_pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    //    m_pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    //    m_pipelineDepthStencilStateCreateInfo.flags = 0;
    //    m_pipelineDepthStencilStateCreateInfo.front = m_pipelineDepthStencilStateCreateInfo.back;
    //    m_pipelineDepthStencilStateCreateInfo.maxDepthBounds = 0.0f;
    //    m_pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0;
    //    m_pipelineDepthStencilStateCreateInfo.pNext = nullptr;
    //    m_pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    //    m_pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    //}

    ////ColorBlendStateCreateInfo
    //{
    //    m_attachmentsDefault.alphaBlendOp = VK_BLEND_OP_ADD;
    //    m_attachmentsDefault.blendEnable = VK_FALSE;
    //    m_attachmentsDefault.colorBlendOp = VK_BLEND_OP_ADD;
    //    m_attachmentsDefault.colorWriteMask = 0xf;/* VK_COLOR_COMPONENT_R_BIT |
    //        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;*/
    //    m_attachmentsDefault.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    //    m_attachmentsDefault.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    //    m_attachmentsDefault.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    //    m_attachmentsDefault.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;

    //    m_pipelineColorBlendStateCreateInfoObj.attachmentCount = 1;
    //    m_pipelineColorBlendStateCreateInfoObj.blendConstants[0] = 1.0f;
    //    m_pipelineColorBlendStateCreateInfoObj.blendConstants[1] = 1.0f;
    //    m_pipelineColorBlendStateCreateInfoObj.blendConstants[2] = 1.0f;
    //    m_pipelineColorBlendStateCreateInfoObj.blendConstants[3] = 1.0f;
    //    m_pipelineColorBlendStateCreateInfoObj.flags = 0;
    //    m_pipelineColorBlendStateCreateInfoObj.logicOp = VK_LOGIC_OP_NO_OP;
    //    m_pipelineColorBlendStateCreateInfoObj.logicOpEnable = VK_FALSE;
    //    m_pipelineColorBlendStateCreateInfoObj.pAttachments = &m_attachmentsDefault;
    //    m_pipelineColorBlendStateCreateInfoObj.pNext = nullptr;
    //    m_pipelineColorBlendStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //}
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::DeInit()
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

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::Update()
{

}

GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory * GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory();
    }
    return instance;
}

GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::~VulkanGraphicsPipelineFactory()
{

}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateVertexInputState(const Core::Utility::VertexInputWrapper& vertexInputWrapper)
{
    Core::Wrappers::VertexInputState * inputState = vertexInputWrapper.inputState;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = UnwrapVertexInputAttributeInfo(inputState->attribInfo, inputState->attribCount);
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = UnwrapVertexInputBindingInfo(inputState->bindingInfo, inputState->bindingCount);
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = inputState->attribCount;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = inputState->bindingCount;
    pipelineVertexInputStateCreateInfo.pNext = nullptr;
    pipelineVertexInputStateCreateInfo.flags = 0;

    auto id = vertexInputWrapper.GetId();
    idToVertexInputMap.insert( std::pair<uint32_t, VkPipelineVertexInputStateCreateInfo>(
    { id, pipelineVertexInputStateCreateInfo }) );

    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateInputAssemblyState(const Core::Utility::InputAssemblyWrapper& inputAssemblyWrapper)
{
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
    Core::Wrappers::InputAssemblyState * inputAssembly = inputAssemblyWrapper.assemblyState;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = inputAssembly->isPrimtiveRestartEnabled;
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = UnwrapPrimitiveInfo(inputAssembly->primitiveType);
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.pNext = nullptr;

    auto id = inputAssemblyWrapper.GetId();
    idToInputAssemblyMap.insert(std::pair<uint32_t, VkPipelineInputAssemblyStateCreateInfo>(
    {id , pipelineInputAssemblyStateCreateInfo }));

    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateShaderState(const Core::Utility::ShaderStateWrapper& shaderStateWrapper)
{
    /*VkShaderStageWrapper * vkStateWrapper = new VkShaderStageWrapper;
    vkStateWrapper->shaderCount = shaderStateWrapper.shaderCount;

    vkStateWrapper->shaderStageCreateInfo = new VkPipelineShaderStageCreateInfo[vkStateWrapper->shaderCount];
    auto id = shaderStateWrapper.GetId();
    vkStateWrapper->id = id;
    for (uint32_t i = 0; i < vkStateWrapper->shaderCount; i++)
    {
        VkPipelineShaderStageCreateInfo * vkInfo = &vkStateWrapper->shaderStageCreateInfo[i];
        uint32_t shaderId = shaderStateWrapper.shader[i].shaderId;
        vkInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkInfo->pName = "main";
        vkInfo->pNext = nullptr;
        vkInfo->pSpecializationInfo = nullptr;
        vkInfo->flags = 0;
        vkInfo->module = *GfxVk::Shading::VkShaderFactory::GetInstance()->GetShaderModule(shaderId) ;
        vkInfo->stage = GfxVk::Shading::VkShaderFactory::GetInstance()->GetShaderStageFlag(shaderId);
    }

    shaderStageWrapperList.push_back(vkStateWrapper);*/
    ASSERT_MSG_DEBUG(0, "not used");
    return 0;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateTessellationState(const Core::Utility::TessellationStateWrapper& wrapper)
{
    VkPipelineTessellationStateCreateInfo info{};
    info.flags = 0;
    info.pNext = nullptr;
    info.patchControlPoints = wrapper.tessellationState->patchControlPoints;
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToTessellationMap.insert(std::pair<uint32_t, VkPipelineTessellationStateCreateInfo>
        ({ id, info}));
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateViewportState(const Core::Utility::ViewPortStateWrapper& wrapper)
{
    VkPipelineViewportStateCreateInfo info{};
    info.flags = 0;
    info.pNext = nullptr;
    info.scissorCount = wrapper.viewportState->scissorCount;
    info.viewportCount = wrapper.viewportState->viewportCount;
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    if (wrapper.viewportState->pScissors == nullptr)
        info.pScissors = nullptr;
    else
    {
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
    }

    if (wrapper.viewportState->pViewports == nullptr)
        info.pViewports = nullptr;
    else
    {
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
    }
    
    auto id = wrapper.GetId();
    idToViewportMap.insert(std::pair<uint32_t, VkPipelineViewportStateCreateInfo>(
        {id, info}));
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateRasterisationState(const Core::Utility::RasterizationStateWrapper& wrapper)
{
    Core::Wrappers::RasterizationState * info = wrapper.rasterState;

    VkPipelineRasterizationStateCreateInfo vkInfo;
    vkInfo.depthBiasClamp = info->depthBiasClamp;
    vkInfo.cullMode = UnwrapCullMode(info->cullMode);
    vkInfo.depthBiasConstantFactor = info->depthBiasConstantFactor;
    vkInfo.depthBiasEnable = info->depthBiasEnable;
    vkInfo.depthBiasSlopeFactor = info->depthBiasSlopeFactor;
    vkInfo.depthClampEnable = info->depthClampEnable;
    vkInfo.flags = 0;
    vkInfo.frontFace = UnwrapFrontFace(info->frontFace);
    vkInfo.lineWidth = info->lineWidth;
    vkInfo.pNext = nullptr;
    vkInfo.polygonMode = UnwrapPolygonMode(info->polygonMode);
    vkInfo.rasterizerDiscardEnable = info->rasterizerDiscardEnable;
    vkInfo.sType= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToRasterizationMap.insert(std::pair<uint32_t, VkPipelineRasterizationStateCreateInfo>(
        { id, vkInfo }));
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateDepthStencilState(const Core::Utility::DepthStencilStateWrapper& wrapper)
{
    Core::Wrappers::DepthStencilState * state = wrapper.depthState;

    VkPipelineDepthStencilStateCreateInfo info{};
    info.back = UnwrapStencilOpState( state->back);
    info.depthBoundsTestEnable = state->depthBoundsTestEnable;
    info.depthCompareOp = GfxVk::Unwrap::UnwrapCompareOp( state->depthCompareOp);
    info.depthTestEnable = state->depthTestEnable;
    info.depthWriteEnable = state->depthWriteEnable;
    info.flags = 0;
    info.front = UnwrapStencilOpState(state->front);
    info.maxDepthBounds = state->maxDepthBounds;
    info.minDepthBounds = state->minDepthBounds;
    info.pNext = nullptr;
    info.stencilTestEnable = state->stencilTestEnable;
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToDepthStencilMap.insert(std::pair<uint32_t, VkPipelineDepthStencilStateCreateInfo>(
        { id, info}));
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateColorBlendState(const Core::Utility::ColorBlendStateWrapper& wrapper)
{
    Core::Wrappers::ColorBlendState * info = wrapper.colorBlendState;
    
    VkPipelineColorBlendStateCreateInfo vkinfo{};
    vkinfo.attachmentCount = info->attachmentCount;
    vkinfo.blendConstants[0] = info->blendConstants[0];
    vkinfo.blendConstants[1] = info->blendConstants[1];
    vkinfo.blendConstants[2] = info->blendConstants[2];
    vkinfo.blendConstants[3] = info->blendConstants[3];
    vkinfo.flags = 0;
    vkinfo.logicOp = UnwrapLogicOp(info->logicOp);
    vkinfo.logicOpEnable = info->logicOpEnable;
    vkinfo.pAttachments = UnwrapColorBlendAttachment(info->pAttachments, info->attachmentCount);
    vkinfo.pNext = nullptr;
    vkinfo.sType= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToColorBlendMap.insert(std::pair<uint32_t, VkPipelineColorBlendStateCreateInfo>({ id, vkinfo }));

    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateMultiSampleState(const Core::Utility::MultiSampleStateWrapper& wrapper)
{
    Core::Wrappers::MultiSampleState * info = wrapper.multiSampleState;

    VkPipelineMultisampleStateCreateInfo vkinfo{};
    vkinfo.alphaToCoverageEnable = info->alphaToCoverageEnable;
    vkinfo.alphaToOneEnable= info->alphaToOneEnable;
    vkinfo.flags = 0;
    vkinfo.minSampleShading = info->minSampleShading;
    vkinfo.pNext = nullptr;
    vkinfo.pSampleMask = NULL;
    vkinfo.rasterizationSamples = GfxVk::Unwrap::UnWrapSampleCount( info->sampleCount);
    vkinfo.sampleShadingEnable = info->sampleShadingEnable;
    vkinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToMultiSampleMap.insert(std::pair<uint32_t, VkPipelineMultisampleStateCreateInfo>({ id, vkinfo }));
    return id;
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateDynamicState(const Core::Utility::DynamicStateWrapper& wrapper)
{
    Core::Wrappers::DynamicStateList * list = wrapper.dynamicStates;

    VkPipelineDynamicStateCreateInfo info{};
    info.dynamicStateCount = list->dynamicStateCount;
    info.flags = 0;
    info.pDynamicStates = UnwrapDynamicList(list->pDynamicStates, list->dynamicStateCount);
    info.pNext = nullptr;
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    auto id = wrapper.GetId();
    idToDynamicMap.insert(std::pair<uint32_t, VkPipelineDynamicStateCreateInfo>({ id, info }));
    return id;
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipeline(Core::Wrappers::PipelineCreateInfo * info, const uint32_t & pipelineCount, uint32_t * pipelineIds)
{
    /*std::vector<VkGraphicsPipelineCreateInfo> infoList;
    infoList.resize(pipelineCount);
    
    VkPipeline * list = new VkPipeline[pipelineCount];

    for (uint32_t i = 0; i < pipelineCount; i++)
    {
        std::vector<VkShaderStageWrapper *>::iterator it;

        it = std::find_if(shaderStageWrapperList.begin(), shaderStageWrapperList.end(), [&](VkShaderStageWrapper * e) {
            return e->id == info[i].statesToIdMap[Core::Enums::PipelineStates::ShaderStage]; });

        ASSERT_MSG_DEBUG(it != shaderStageWrapperList.end(), "Shader stage id not found");

        VkPipelineLayout layout = *GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineLayout(info[i].statesToIdMap[Core::Enums::PipelineStates::ShaderResourcesLayout]);
        VkRenderPass renderpass = *GfxVk::Renderpass::VkRenderPassFactory::GetInstance()->GetRenderPass(info[i].renderPassId);

        VkGraphicsPipelineCreateInfo vkinfo = {};
        vkinfo.basePipelineHandle = VK_NULL_HANDLE;
        vkinfo.basePipelineIndex = -1;
        vkinfo.flags = 0;
        vkinfo.layout = layout;
        vkinfo.pColorBlendState = idToColorBlendMap[info[i].statesToIdMap[Core::Enums::PipelineStates::ColorBlendState]];
        vkinfo.pDepthStencilState = idToDepthStencilMap[info[i].statesToIdMap[Core::Enums::PipelineStates::DepthStencilState]];
        vkinfo.pDynamicState = idToDynamicMap[info[i].statesToIdMap[Core::Enums::PipelineStates::DynamicState]];
        vkinfo.pInputAssemblyState = idToInputAssemblyMap[info[i].statesToIdMap[Core::Enums::PipelineStates::InputAssemblyState]];
        vkinfo.pMultisampleState = idToMultiSampleMap[info[i].statesToIdMap[Core::Enums::PipelineStates::MultisampleState]];
        vkinfo.pNext = nullptr;
        vkinfo.pRasterizationState = idToRasterizationMap[info[i].statesToIdMap[Core::Enums::PipelineStates::RasterizationState]];
        vkinfo.pStages = (*it)->shaderStageCreateInfo;
        vkinfo.pTessellationState = idToTessellationMap[info[i].statesToIdMap[Core::Enums::PipelineStates::TessellationState]];
        vkinfo.pVertexInputState = idToVertexInputMap[info[i].statesToIdMap[Core::Enums::PipelineStates::VertexInputState]];
        vkinfo.pViewportState = idToViewportMap[info[i].statesToIdMap[Core::Enums::PipelineStates::ViewportState]];
        vkinfo.renderPass = renderpass;
        vkinfo.stageCount = (*it)->shaderCount;
        vkinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vkinfo.subpass = info[i].subpassId;

        infoList[i] = vkinfo;
    }

    VkPipelineCache cache = VK_NULL_HANDLE;

    GfxVk::Utility::ErrorCheck(vkCreateGraphicsPipelines(
        DeviceInfo::GetLogicalDevice(),
        cache,
        (uint32_t)infoList.size(),
        infoList.data(),
        DeviceInfo::GetAllocationCallback(),
        list));
    
    for (uint32_t i = 0; i < pipelineCount; i++)
    {
        idToPipelineMap.insert(std::pair<uint32_t, VkPipeline *>({pipelineIds[i], &list[i]}));
    }

    tempVectorToBeDeleted.push_back(list);*/
    ASSERT_MSG_DEBUG(0, "not used");
}

uint32_t GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipeline(const Core::Wrappers::PipelineCreateInfo& info)
{
    auto pipelineLayout = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineLayout(info.pipelineLayoutId);
    auto& shaderStageInfo = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineShaderInfo(info.statesToIdMap.at(Core::Enums::PipelineStates::ShaderStage));
    auto& vertexInputInfo = GfxVk::Shading::VkShaderResourceManager::GetInstance()->GetPipelineVertexInputInfo(info.statesToIdMap.at(Core::Enums::PipelineStates::VertexInputState));
    
    std::vector<VkFormat> colorFormats;
    for (auto& format : info.colorFormats)
    {
        colorFormats.push_back(GfxVk::Unwrap::UnWrapFormat(format));
    }

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.colorAttachmentCount = (uint32_t)info.colorFormats.size();
    if(info.depthFormat.has_value())
        renderingInfo.depthAttachmentFormat = GfxVk::Unwrap::UnWrapFormat(info.depthFormat.value());

    renderingInfo.pColorAttachmentFormats = colorFormats.data();
    //renderingInfo.stencilAttachmentFormat = (&selectedFormatAndColorSpace.format);
    //renderingInfo.viewMask = ;
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

    VkGraphicsPipelineCreateInfo vkinfo = {};
    vkinfo.basePipelineHandle = VK_NULL_HANDLE;
    vkinfo.basePipelineIndex = -1;
    vkinfo.flags = 0;
    vkinfo.layout = *pipelineLayout;
    vkinfo.pColorBlendState = &idToColorBlendMap[info.statesToIdMap.at(Core::Enums::PipelineStates::ColorBlendState)];
    vkinfo.pDepthStencilState = &idToDepthStencilMap[info.statesToIdMap.at(Core::Enums::PipelineStates::DepthStencilState)];
    vkinfo.pDynamicState = &idToDynamicMap[info.statesToIdMap.at(Core::Enums::PipelineStates::DynamicState)];
    vkinfo.pInputAssemblyState = &idToInputAssemblyMap[info.statesToIdMap.at(Core::Enums::PipelineStates::InputAssemblyState)];
    vkinfo.pMultisampleState = &idToMultiSampleMap[info.statesToIdMap.at(Core::Enums::PipelineStates::MultisampleState)];
    vkinfo.pNext = &renderingInfo;
    vkinfo.pRasterizationState = &idToRasterizationMap[info.statesToIdMap.at(Core::Enums::PipelineStates::RasterizationState)];
    vkinfo.pStages = shaderStageInfo.data();
    vkinfo.pTessellationState = &idToTessellationMap[info.statesToIdMap.at(Core::Enums::PipelineStates::TessellationState)];
    vkinfo.pVertexInputState = &vertexInputInfo;
    vkinfo.pViewportState = &idToViewportMap[info.statesToIdMap.at(Core::Enums::PipelineStates::ViewportState)];
    vkinfo.renderPass = VK_NULL_HANDLE;
    vkinfo.stageCount = (uint32_t)shaderStageInfo.size();
    vkinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vkinfo.subpass = 0;

    VkPipelineCache cache = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    GfxVk::Utility::ErrorCheck(vkCreateGraphicsPipelines(
        DeviceInfo::GetLogicalDevice(),
        cache,
        1,
        &vkinfo,
        DeviceInfo::GetAllocationCallback(),
        &pipeline));

    auto id = GetGraphicsPipelineId();
    idToPipelineMap.insert({ id, pipeline});

    return id;
}

const VkPipeline& GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::GetPipeline(const uint32_t & id) 
{
    if (idToPipelineMap.find(id) == idToPipelineMap.end())
    {
        ASSERT_MSG_DEBUG(0, "Id not found");
    }

    return idToPipelineMap[id];
}


void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::InitPipelineCache()
{

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineShaderStage()
{

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreateDynamicState()
{
    m_pipelineDynamicStateCreateInfoObj.dynamicStateCount = (uint32_t)dynamicStateList.size();
    m_pipelineDynamicStateCreateInfoObj.pDynamicStates = dynamicStateList.data();
    m_pipelineDynamicStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    idToDynamicMap.insert({ GetStateId(Core::Enums::PipelineStates::DynamicState), m_pipelineDynamicStateCreateInfoObj });
}



void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineVertexInputState()
{

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineInputAssemblyState()
{
    m_pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    m_pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    idToInputAssemblyMap.insert({ GetStateId(Core::Enums::PipelineStates::InputAssemblyState), m_pipelineInputAssemblyStateCreateInfo });

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineTessellationState()
{
    m_pipelineTessellationStateCreateInfo.flags = 0;
    m_pipelineTessellationStateCreateInfo.patchControlPoints = 0;
    m_pipelineTessellationStateCreateInfo.pNext = nullptr;
    m_pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    idToTessellationMap.insert({ GetStateId(Core::Enums::PipelineStates::TessellationState), m_pipelineTessellationStateCreateInfo });

}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineRasterizationState()
{
    m_pipelineRasterizationStateCreateInfoObj.cullMode = VK_CULL_MODE_BACK_BIT;
    m_pipelineRasterizationStateCreateInfoObj.depthBiasClamp = 0;
    m_pipelineRasterizationStateCreateInfoObj.depthBiasConstantFactor = 0;
    m_pipelineRasterizationStateCreateInfoObj.depthBiasEnable = VK_FALSE;
    m_pipelineRasterizationStateCreateInfoObj.depthBiasSlopeFactor = 0;
    m_pipelineRasterizationStateCreateInfoObj.depthClampEnable = VK_FALSE;
    m_pipelineRasterizationStateCreateInfoObj.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_pipelineRasterizationStateCreateInfoObj.lineWidth = 1.0f;
    m_pipelineRasterizationStateCreateInfoObj.polygonMode = VK_POLYGON_MODE_FILL;
    m_pipelineRasterizationStateCreateInfoObj.rasterizerDiscardEnable = VK_FALSE;
    m_pipelineRasterizationStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    idToRasterizationMap.insert({ GetStateId(Core::Enums::PipelineStates::RasterizationState), m_pipelineRasterizationStateCreateInfoObj});
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineColorBlendState()
{
    m_attachmentsDefault.alphaBlendOp = VK_BLEND_OP_ADD;
    m_attachmentsDefault.blendEnable = VK_FALSE;
    m_attachmentsDefault.colorBlendOp = VK_BLEND_OP_ADD;
    m_attachmentsDefault.colorWriteMask = 0xf;
    m_attachmentsDefault.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    m_attachmentsDefault.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    m_attachmentsDefault.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    m_attachmentsDefault.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;

    m_pipelineColorBlendStateCreateInfoObj.attachmentCount = 1;
    m_pipelineColorBlendStateCreateInfoObj.blendConstants[0] = 1.0f;
    m_pipelineColorBlendStateCreateInfoObj.blendConstants[1] = 1.0f;
    m_pipelineColorBlendStateCreateInfoObj.blendConstants[2] = 1.0f;
    m_pipelineColorBlendStateCreateInfoObj.blendConstants[3] = 1.0f;
    m_pipelineColorBlendStateCreateInfoObj.logicOp = VK_LOGIC_OP_NO_OP;
    m_pipelineColorBlendStateCreateInfoObj.logicOpEnable = VK_FALSE;
    m_pipelineColorBlendStateCreateInfoObj.pAttachments = &m_attachmentsDefault;
    m_pipelineColorBlendStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    idToColorBlendMap.insert({ GetStateId(Core::Enums::PipelineStates::ColorBlendState), m_pipelineColorBlendStateCreateInfoObj });
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineViewportState()
{
    m_pipelineViewportStateCreateInfoObj.pScissors = nullptr;
    m_pipelineViewportStateCreateInfoObj.pViewports = nullptr;
    m_pipelineViewportStateCreateInfoObj.scissorCount = 1;
    m_pipelineViewportStateCreateInfoObj.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    m_pipelineViewportStateCreateInfoObj.viewportCount = 1;
    idToViewportMap.insert({ GetStateId(Core::Enums::PipelineStates::ViewportState), m_pipelineViewportStateCreateInfoObj });
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineDepthStencilState()
{
    m_pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_pipelineDepthStencilStateCreateInfo.pNext = NULL;
    m_pipelineDepthStencilStateCreateInfo.flags = 0;
    m_pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    m_pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    m_pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    m_pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    m_pipelineDepthStencilStateCreateInfo.minDepthBounds = 0;
    m_pipelineDepthStencilStateCreateInfo.maxDepthBounds = 0;
    m_pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    m_pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    m_pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    m_pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    m_pipelineDepthStencilStateCreateInfo.back.compareMask = 0;
    m_pipelineDepthStencilStateCreateInfo.back.reference = 0;
    m_pipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    m_pipelineDepthStencilStateCreateInfo.back.writeMask = 0;
    m_pipelineDepthStencilStateCreateInfo.front = m_pipelineDepthStencilStateCreateInfo.back;
    idToDepthStencilMap.insert({ GetStateId(Core::Enums::PipelineStates::DepthStencilState), m_pipelineDepthStencilStateCreateInfo });
}

void GfxVk::VulkanPipeline::VulkanGraphicsPipelineFactory::CreatePipelineMultiSampleState()
{
    m_pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_pipelineMultisampleStateCreateInfo.pNext = NULL;
    m_pipelineMultisampleStateCreateInfo.flags = 0;
    m_pipelineMultisampleStateCreateInfo.pSampleMask = NULL;
    m_pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    m_pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    m_pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    m_pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    m_pipelineMultisampleStateCreateInfo.minSampleShading = 0.0;
    idToMultiSampleMap.insert({ GetStateId(Core::Enums::PipelineStates::MultisampleState), m_pipelineMultisampleStateCreateInfo });
}

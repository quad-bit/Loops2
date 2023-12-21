#include "CorePrecompiled.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"

std::string Core::Utility::ConvertPipelineStageFlagToString(const Core::Enums::PipelineStageFlagBits2& flag)
{
    std::string value;
    switch (flag)
    {
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BLIT_BIT:
        value = "PIPELINE_STAGE_2_BLIT_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT:
        value = "PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_CLEAR_BIT:
        value = "PIPELINE_STAGE_2_CLEAR_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT:
        value = "PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COMPUTE_SHADER_BIT:
        value = "PIPELINE_STAGE_2_COMPUTE_SHADER_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COPY_BIT:
        value = "PIPELINE_STAGE_2_COPY_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT:
        value = "PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT:
        value = "PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT:
        value = "PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_NONE:
        value = "PIPELINE_STAGE_2_NONE";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT:
        value = "PIPELINE_STAGE_2_TOP_OF_PIPE_BIT";
        break;
    case Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TRANSFER_BIT:
        value = "PIPELINE_STAGE_2_TRANSFER_BIT";
        break;
    default:
        ASSERT_MSG_DEBUG(0, "invalid case");
    }
    return value;
}

std::string Core::Utility::ConvertPipelineAccessFlagToString(const Core::Enums::PipelineAccessFlagBits2& flag)
{
    std::string value;
    switch (flag)
    {
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_COLOR_ATTACHMENT_READ_BIT:
        value = "ACCESS_2_COLOR_ATTACHMENT_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT:
        value = "ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
        value = "ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
        value = "ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_HOST_READ_BIT:
        value = "ACCESS_2_HOST_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_HOST_WRITE_BIT:
        value = "ACCESS_2_HOST_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_MEMORY_READ_BIT:
        value = "ACCESS_2_MEMORY_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_MEMORY_WRITE_BIT:
        value = "ACCESS_2_MEMORY_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE:
        value = "ACCESS_2_NONE";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_READ_BIT:
        value = "ACCESS_2_SHADER_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_SAMPLED_READ_BIT:
        value = "ACCESS_2_SHADER_SAMPLED_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_WRITE_BIT:
        value = "ACCESS_2_SHADER_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_READ_BIT:
        value = "ACCESS_2_TRANSFER_READ_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_WRITE_BIT:
        value = "ACCESS_2_TRANSFER_WRITE_BIT";
        break;
    case Core::Enums::PipelineAccessFlagBits2::ACCESS_2_UNIFORM_READ_BIT:
        value = "ACCESS_2_UNIFORM_READ_BIT";
        break;
    default:
        ASSERT_MSG_DEBUG(0, "invalid case");
    }
    return value;
}

std::string Core::Utility::ConvertImageLayoutToString(const Core::Enums::ImageLayout& layout)
{
    std::string value;
    switch (layout)
    {
    case Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        value = "COLOR_ATTACHMENT";
        break;
    case Core::Enums::ImageLayout::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        value = "DEPTH_ATTACHMENT";
        break;
    case Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        value = "DEPTH_STENCIL_ATTACHMENT";
        break;
    case Core::Enums::ImageLayout::LAYOUT_GENERAL:
        value = "GENERAL";
        break;
    case Core::Enums::ImageLayout::LAYOUT_PREINITIALIZED:
        value = "PREINITIALIZED";
        break;
    case Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR:
        value = "PRESENT_SRC_KHR";
        break;
    case Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        value = "SHADER_READ_ONLY";
        break;
    case Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
        value = "TRANSFER_DST";
        break;
    case Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL:
        value = "TRANSFER_SRC";
        break;
    case Core::Enums::ImageLayout::LAYOUT_UNDEFINED:
        value = "UNDEFINED";
        break;
    default:
        ASSERT_MSG_DEBUG(0, "invalid");
        break;
    }
    return value;
}

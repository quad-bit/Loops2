#pragma once
#include <stdint.h>
#include <glm\glm.hpp>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <bitset>
#include "PlatformSettings.h"
#include <variant>
#include <optional>

namespace Core
{
    namespace Enums
    {
        enum class ImageViewType
        {
            IMAGE_VIEW_TYPE_1D = 0,
            IMAGE_VIEW_TYPE_2D = 1,
            IMAGE_VIEW_TYPE_3D = 2,
            IMAGE_VIEW_TYPE_CUBE = 3,
            IMAGE_VIEW_TYPE_1D_ARRAY = 4,
            IMAGE_VIEW_TYPE_2D_ARRAY = 5,
            IMAGE_VIEW_TYPE_CUBE_ARRAY = 6,
            IMAGE_VIEW_TYPE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class ImageType
        {
            IMAGE_TYPE_1D = 0,
            IMAGE_TYPE_2D = 1,
            IMAGE_TYPE_3D = 2,
            IMAGE_TYPE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class Format
        {
            UNDEFINED,
            B8G8R8A8_UNORM,
            D32_SFLOAT_S8_UINT,
            D24_UNORM_S8_UINT,
            D16_UNORM_S8_UINT,
            D32_SFLOAT,
            D16_UNORM,
            R32G32B32A32_SFLOAT,
            R32G32B32_SFLOAT
        };

        enum class ColorSpace
        {
            COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        enum class Dimensions
        {
            Dim1,
            Dim2,
            Dim3
        };

        enum class ComponentSwizzle
        {
            COMPONENT_SWIZZLE_IDENTITY = 0,
            COMPONENT_SWIZZLE_ZERO = 1,
            COMPONENT_SWIZZLE_ONE = 2,
            COMPONENT_SWIZZLE_R = 3,
            COMPONENT_SWIZZLE_G = 4,
            COMPONENT_SWIZZLE_B = 5,
            COMPONENT_SWIZZLE_A = 6,
            COMPONENT_SWIZZLE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class BufferUsage
        {
            BUFFER_USAGE_TRANSFER_SRC_BIT = 0x00000001,
            BUFFER_USAGE_TRANSFER_DST_BIT = 0x00000002,
            BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
            BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
            BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000010,
            BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,
            BUFFER_USAGE_INDEX_BUFFER_BIT = 0x00000040,
            BUFFER_USAGE_VERTEX_BUFFER_BIT = 0x00000080,
            BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00000100,
            BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT = 0x00020000,
        };

        enum class ImageUsage
        {
            USAGE_TRANSFER_SRC_BIT = 0x00000001,
            USAGE_TRANSFER_DST_BIT = 0x00000002,
            USAGE_SAMPLED_BIT = 0x00000004,
            USAGE_STORAGE_BIT = 0x00000008,
            USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,
            USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000020,
            USAGE_TRANSIENT_ATTACHMENT_BIT = 0x00000040,
            USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,
            USAGE_SHADING_RATE_IMAGE_BIT_NV = 0x00000100,
            USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT = 0x00000200,
        };

        enum class Samples
        {
            SAMPLE_COUNT_1_BIT = 0x00000001,
            SAMPLE_COUNT_2_BIT = 0x00000002,
            SAMPLE_COUNT_4_BIT = 0x00000004,
            SAMPLE_COUNT_8_BIT = 0x00000008,
            SAMPLE_COUNT_16_BIT = 0x00000010,
            SAMPLE_COUNT_32_BIT = 0x00000020,
            SAMPLE_COUNT_64_BIT = 0x00000040,
        };

        enum class Filter
        {
            FILTER_NEAREST = 0,
            FILTER_LINEAR = 1,
        };

        enum class SamplerMipmapMode
        {
            SAMPLER_MIPMAP_MODE_NEAREST = 0,
            SAMPLER_MIPMAP_MODE_LINEAR = 1,
        };

        enum class SamplerAddressMode
        {
            SAMPLER_ADDRESS_MODE_REPEAT = 0,
            SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
            SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
            SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
            SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4,
        };

        enum class BorderColor
        {
            BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
            BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
            BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
            BORDER_COLOR_INT_OPAQUE_BLACK = 3,
            BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
            BORDER_COLOR_INT_OPAQUE_WHITE = 5,
        };

        enum ImageAspectFlag
        {
            IMAGE_ASPECT_COLOR_BIT = 0x00000001,
            IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
            IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
            IMAGE_ASPECT_METADATA_BIT = 0x00000008,
            IMAGE_ASPECT_PLANE_0_BIT = 0x00000010,
            IMAGE_ASPECT_PLANE_1_BIT = 0x00000020,
            IMAGE_ASPECT_PLANE_2_BIT = 0x00000040,
            IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT = 0x00000080,
            IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT = 0x00000100,
            IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT = 0x00000200,
            IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT = 0x00000400,
            IMAGE_ASPECT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
        };

        enum class ImageLayout
        {
            LAYOUT_UNDEFINED,
            LAYOUT_GENERAL,
            LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            LAYOUT_TRANSFER_SRC_OPTIMAL,
            LAYOUT_TRANSFER_DST_OPTIMAL,
            LAYOUT_PREINITIALIZED,
            LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
            LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
            LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
            LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
            LAYOUT_PRESENT_SRC_KHR
        };

        enum class LoadOperation
        {
            LOAD_OP_LOAD = 0,
            LOAD_OP_CLEAR = 1,
            LOAD_OP_DONT_CARE = 2,
        };

        enum class StoreOperation
        {
            STORE_OP_STORE = 0,
            STORE_OP_DONT_CARE = 1,
        };

        enum class CommandPoolProperty
        {
            TRANSIENT_BIT = 0x00000001,
            RESET_COMMAND_BUFFER_BIT = 0x00000002,
            TRANS_RESET = 0x00000003
        };

        enum class CommandBufferLevel
        {
            PRIMARY = 0,
            SECONDARY = 1
        };

        enum class CommandBufferProperty
        {
            SHORT_LIVED = 0,
            RESET_ALLOWED = 1
        };

        enum class PipelineType
        {
            GRAPHICS,
            COMPUTE,
            TRANSFER,
        };

        enum class CommandBufferUsage
        {
            USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001,
            RENDER_PASS_CONTINUE_BIT = 0x00000002,
            SIMULTANEOUS_USE_BIT = 0x00000004
        };

        enum class SubpassContentStatus
        {
            SUBPASS_CONTENTS_INLINE = 0,
            SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS = 1,
        };

        enum class RenderPassTag
        {
            None = 0,
            ColorPass = 1,
            DepthPass = 2,
            NUM_PASSES = 3,
        };

        enum class PipelineStage
        {
            TOP_OF_PIPE_BIT = 0x00000001,
            DRAW_INDIRECT_BIT = 0x00000002,
            VERTEX_INPUT_BIT = 0x00000004,
            VERTEX_SHADER_BIT = 0x00000008,
            TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
            TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
            GEOMETRY_SHADER_BIT = 0x00000040,
            FRAGMENT_SHADER_BIT = 0x00000080,
            EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
            LATE_FRAGMENT_TESTS_BIT = 0x00000200,
            COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
            COMPUTE_SHADER_BIT = 0x00000800,
            TRANSFER_BIT = 0x00001000,
            BOTTOM_OF_PIPE_BIT = 0x00002000,
            HOST_BIT = 0x00004000,
            ALL_GRAPHICS_BIT = 0x00008000,
            ALL_COMMANDS_BIT = 0x00010000,
            TRANSFORM_FEEDBACK_BIT_EXT = 0x01000000,
            CONDITIONAL_RENDERING_BIT_EXT = 0x00040000,
            RAY_TRACING_SHADER_BIT_KHR = 0x00200000,
            ACCELERATION_STRUCTURE_BUILD_BIT_KHR = 0x02000000,
            SHADING_RATE_IMAGE_BIT_NV = 0x00400000,
            TASK_SHADER_BIT_NV = 0x00080000,
            MESH_SHADER_BIT_NV = 0x00100000,
            FRAGMENT_DENSITY_PROCESS_BIT_EXT = 0x00800000,
            COMMAND_PREPROCESS_BIT_NV = 0x00020000,
            FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
        };

        enum class PipelineStageFlagBits2 : long long
        {
            PIPELINE_STAGE_2_NONE = 0ULL,
            PIPELINE_STAGE_2_NONE_KHR = 0ULL,
            PIPELINE_STAGE_2_TOP_OF_PIPE_BIT = 0x00000001ULL,
            PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR = 0x00000001ULL,
            PIPELINE_STAGE_2_DRAW_INDIRECT_BIT = 0x00000002ULL,
            PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR = 0x00000002ULL,
            PIPELINE_STAGE_2_VERTEX_INPUT_BIT = 0x00000004ULL,
            PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR = 0x00000004ULL,
            PIPELINE_STAGE_2_VERTEX_SHADER_BIT = 0x00000008ULL,
            PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR = 0x00000008ULL,
            PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010ULL,
            PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT_KHR = 0x00000010ULL,
            PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020ULL,
            PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT_KHR = 0x00000020ULL,
            PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT = 0x00000040ULL,
            PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT_KHR = 0x00000040ULL,
            PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT = 0x00000080ULL,
            PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR = 0x00000080ULL,
            PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT = 0x00000100ULL,
            PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR = 0x00000100ULL,
            PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT = 0x00000200ULL,
            PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR = 0x00000200ULL,
            PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400ULL,
            PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR = 0x00000400ULL,
            PIPELINE_STAGE_2_COMPUTE_SHADER_BIT = 0x00000800ULL,
            PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR = 0x00000800ULL,
            PIPELINE_STAGE_2_ALL_TRANSFER_BIT = 0x00001000ULL,
            PIPELINE_STAGE_2_ALL_TRANSFER_BIT_KHR = 0x00001000ULL,
            PIPELINE_STAGE_2_TRANSFER_BIT = 0x00001000ULL,
            PIPELINE_STAGE_2_TRANSFER_BIT_KHR = 0x00001000ULL,
            PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT = 0x00002000ULL,
            PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT_KHR = 0x00002000ULL,
            PIPELINE_STAGE_2_HOST_BIT = 0x00004000ULL,
            PIPELINE_STAGE_2_HOST_BIT_KHR = 0x00004000ULL,
            PIPELINE_STAGE_2_ALL_GRAPHICS_BIT = 0x00008000ULL,
            PIPELINE_STAGE_2_ALL_GRAPHICS_BIT_KHR = 0x00008000ULL,
            PIPELINE_STAGE_2_ALL_COMMANDS_BIT = 0x00010000ULL,
            PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR = 0x00010000ULL,
            PIPELINE_STAGE_2_COPY_BIT = 0x100000000ULL,
            PIPELINE_STAGE_2_COPY_BIT_KHR = 0x100000000ULL,
            PIPELINE_STAGE_2_RESOLVE_BIT = 0x200000000ULL,
            PIPELINE_STAGE_2_RESOLVE_BIT_KHR = 0x200000000ULL,
            PIPELINE_STAGE_2_BLIT_BIT = 0x400000000ULL,
            PIPELINE_STAGE_2_BLIT_BIT_KHR = 0x400000000ULL,
            PIPELINE_STAGE_2_CLEAR_BIT = 0x800000000ULL,
            PIPELINE_STAGE_2_CLEAR_BIT_KHR = 0x800000000ULL,
            PIPELINE_STAGE_2_INDEX_INPUT_BIT = 0x1000000000ULL,
            PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR = 0x1000000000ULL,
            PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT = 0x2000000000ULL,
            PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR = 0x2000000000ULL,
            PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT = 0x4000000000ULL,
            PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT_KHR = 0x4000000000ULL
        };

        enum class PipelineAccessFlagBits2 : long long
        {
            ACCESS_2_NONE = 0ULL,
            ACCESS_2_NONE_KHR = 0ULL,
            ACCESS_2_INDIRECT_COMMAND_READ_BIT = 0x00000001ULL,
            ACCESS_2_INDIRECT_COMMAND_READ_BIT_KHR = 0x00000001ULL,
            ACCESS_2_INDEX_READ_BIT = 0x00000002ULL,
            ACCESS_2_INDEX_READ_BIT_KHR = 0x00000002ULL,
            ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT = 0x00000004ULL,
            ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT_KHR = 0x00000004ULL,
            ACCESS_2_UNIFORM_READ_BIT = 0x00000008ULL,
            ACCESS_2_UNIFORM_READ_BIT_KHR = 0x00000008ULL,
            ACCESS_2_INPUT_ATTACHMENT_READ_BIT = 0x00000010ULL,
            ACCESS_2_INPUT_ATTACHMENT_READ_BIT_KHR = 0x00000010ULL,
            ACCESS_2_SHADER_READ_BIT = 0x00000020ULL,
            ACCESS_2_SHADER_READ_BIT_KHR = 0x00000020ULL,
            ACCESS_2_SHADER_WRITE_BIT = 0x00000040ULL,
            ACCESS_2_SHADER_WRITE_BIT_KHR = 0x00000040ULL,
            ACCESS_2_COLOR_ATTACHMENT_READ_BIT = 0x00000080ULL,
            ACCESS_2_COLOR_ATTACHMENT_READ_BIT_KHR = 0x00000080ULL,
            ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100ULL,
            ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR = 0x00000100ULL,
            ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200ULL,
            ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR = 0x00000200ULL,
            ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400ULL,
            ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR = 0x00000400ULL,
            ACCESS_2_TRANSFER_READ_BIT = 0x00000800ULL,
            ACCESS_2_TRANSFER_READ_BIT_KHR = 0x00000800ULL,
            ACCESS_2_TRANSFER_WRITE_BIT = 0x00001000ULL,
            ACCESS_2_TRANSFER_WRITE_BIT_KHR = 0x00001000ULL,
            ACCESS_2_HOST_READ_BIT = 0x00002000ULL,
            ACCESS_2_HOST_READ_BIT_KHR = 0x00002000ULL,
            ACCESS_2_HOST_WRITE_BIT = 0x00004000ULL,
            ACCESS_2_HOST_WRITE_BIT_KHR = 0x00004000ULL,
            ACCESS_2_MEMORY_READ_BIT = 0x00008000ULL,
            ACCESS_2_MEMORY_READ_BIT_KHR = 0x00008000ULL,
            ACCESS_2_MEMORY_WRITE_BIT = 0x00010000ULL,
            ACCESS_2_MEMORY_WRITE_BIT_KHR = 0x00010000ULL,
            ACCESS_2_SHADER_SAMPLED_READ_BIT = 0x100000000ULL,
            ACCESS_2_SHADER_SAMPLED_READ_BIT_KHR = 0x100000000ULL,
            ACCESS_2_SHADER_STORAGE_READ_BIT = 0x200000000ULL,
            ACCESS_2_SHADER_STORAGE_READ_BIT_KHR = 0x200000000ULL,
            ACCESS_2_SHADER_STORAGE_WRITE_BIT = 0x400000000ULL,
            ACCESS_2_SHADER_STORAGE_WRITE_BIT_KHR = 0x400000000ULL
        };

        enum class QueueType
        {
            RENDER,
            PRESENT,
            COMPUTE,
            TRANSFER,
            NONE
        };

        enum class ShaderType
        {
            VERTEX,
            TESSELLATION,
            GEOMETRY,
            FRAGMENT,
            COMPUTE
        };

        enum class BufferType
        {
            TRANSFER_SRC_BIT = 0x00000001,
            TRANSFER_DST_BIT = 0x00000002,
            UNIFORM_TEXEL_BUFFER_BIT = 0x00000004,
            STORAGE_TEXEL_BUFFER_BIT = 0x00000008,
            UNIFORM_BUFFER_BIT = 0x00000010,
            STORAGE_BUFFER_BIT = 0x00000020,
            INDEX_BUFFER_BIT = 0x00000040,
            VERTEX_BUFFER_BIT = 0x00000080,
            INDIRECT_BUFFER_BIT = 0x00000100
        };

        enum class MemoryType
        {
            DEVICE_LOCAL_BIT = 0x00000001,
            HOST_VISIBLE_BIT = 0x00000002,
            HOST_COHERENT_BIT = 0x00000004,
            HOST_CACHED_BIT = 0x00000008,
            LAZILY_ALLOCATED_BIT = 0x00000010,
            PROTECTED_BIT = 0x00000020
        };

        enum class PipelineStates
        {
            VertexInputState = 0,
            InputAssemblyState = 1,
            ShaderStage = 2,
            ShaderResourcesLayout = 3,
            TessellationState = 4,
            ViewportState = 5,
            RasterizationState = 6,
            MultisampleState = 7,
            DepthStencilState = 8,
            ColorBlendState = 9,
            DynamicState = 10,
            NumStates = 11
        };

        enum class VertexIputRate
        {
            PerVertex,
            PerInstance
        };

        enum class PrimtiveType
        {
            TOPOLOGY_POINT_LIST = 0,
            TOPOLOGY_LINE_LIST = 1,
            TOPOLOGY_LINE_STRIP = 2,
            TOPOLOGY_TRIANGLE_LIST = 3,
            TOPOLOGY_TRIANGLE_STRIP = 4,
            TOPOLOGY_TRIANGLE_FAN = 5,
            TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
            TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
            TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
            TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
            TOPOLOGY_PATCH_LIST = 10,
            TOPOLOGY_MAX_ENUM = 0x7FFFFFFF
        };

        enum class PolygonMode
        {
            POLYGON_MODE_FILL = 0,
            POLYGON_MODE_LINE = 1,
            POLYGON_MODE_POINT = 2,
            POLYGON_MODE_FILL_RECTANGLE_NV = 1000153000,
            POLYGON_MODE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class FrontFace {
            FRONT_FACE_COUNTER_CLOCKWISE = 0,
            FRONT_FACE_CLOCKWISE = 1,
            FRONT_FACE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class CullMode {
            CULL_MODE_NONE = 0,
            CULL_MODE_FRONT_BIT = 0x00000001,
            CULL_MODE_BACK_BIT = 0x00000002,
            CULL_MODE_FRONT_AND_BACK = 0x00000003,
            CULL_MODE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
        };

        enum class IndexType {
            INDEX_TYPE_UINT16 = 0,
            INDEX_TYPE_UINT32 = 1,
        };

        enum class ShaderResourceClassification
        {
            PER_SCENE,
            PER_MATERIAL,
            PER_MATERIAL_INSTANCE,
            PER_OBJECT
        };

        enum class CompareOp
        {
            COMPARE_OP_NEVER = 0,
            COMPARE_OP_LESS = 1,
            COMPARE_OP_EQUAL = 2,
            COMPARE_OP_LESS_OR_EQUAL = 3,
            COMPARE_OP_GREATER = 4,
            COMPARE_OP_NOT_EQUAL = 5,
            COMPARE_OP_GREATER_OR_EQUAL = 6,
            COMPARE_OP_ALWAYS = 7,
            COMPARE_OP_MAX_ENUM = 0x7FFFFFFF
        };

        enum class StencilOp
        {
            STENCIL_OP_KEEP = 0,
            STENCIL_OP_ZERO = 1,
            STENCIL_OP_REPLACE = 2,
            STENCIL_OP_INCREMENT_AND_CLAMP = 3,
            STENCIL_OP_DECREMENT_AND_CLAMP = 4,
            STENCIL_OP_INVERT = 5,
            STENCIL_OP_INCREMENT_AND_WRAP = 6,
            STENCIL_OP_DECREMENT_AND_WRAP = 7,
            STENCIL_OP_MAX_ENUM = 0x7FFFFFFF
        };

        enum class LogicOp {
            LOGIC_OP_CLEAR = 0,
            LOGIC_OP_AND = 1,
            LOGIC_OP_AND_REVERSE = 2,
            LOGIC_OP_COPY = 3,
            LOGIC_OP_AND_INVERTED = 4,
            LOGIC_OP_NO_OP = 5,
            LOGIC_OP_XOR = 6,
            LOGIC_OP_OR = 7,
            LOGIC_OP_NOR = 8,
            LOGIC_OP_EQUIVALENT = 9,
            LOGIC_OP_INVERT = 10,
            LOGIC_OP_OR_REVERSE = 11,
            LOGIC_OP_COPY_INVERTED = 12,
            LOGIC_OP_OR_INVERTED = 13,
            LOGIC_OP_NAND = 14,
            LOGIC_OP_SET = 15,
            LOGIC_OP_MAX_ENUM = 0x7FFFFFFF
        };

        enum class BlendOp
        {
            BLEND_OP_ADD = 0,
            BLEND_OP_SUBTRACT = 1,
            BLEND_OP_REVERSE_SUBTRACT = 2,
            BLEND_OP_MIN = 3,
            BLEND_OP_MAX = 4,
            BLEND_OP_ZERO_EXT = 1000148000,
            BLEND_OP_SRC_EXT = 1000148001,
            BLEND_OP_DST_EXT = 1000148002,
            BLEND_OP_SRC_OVER_EXT = 1000148003,
            BLEND_OP_DST_OVER_EXT = 1000148004,
            BLEND_OP_SRC_IN_EXT = 1000148005,
            BLEND_OP_DST_IN_EXT = 1000148006,
            BLEND_OP_SRC_OUT_EXT = 1000148007,
            BLEND_OP_DST_OUT_EXT = 1000148008,
            BLEND_OP_SRC_ATOP_EXT = 1000148009,
            BLEND_OP_DST_ATOP_EXT = 1000148010,
            BLEND_OP_XOR_EXT = 1000148011,
            BLEND_OP_MULTIPLY_EXT = 1000148012,
            BLEND_OP_SCREEN_EXT = 1000148013,
            BLEND_OP_OVERLAY_EXT = 1000148014,
            BLEND_OP_DARKEN_EXT = 1000148015,
            BLEND_OP_LIGHTEN_EXT = 1000148016,
            BLEND_OP_COLORDODGE_EXT = 1000148017,
            BLEND_OP_COLORBURN_EXT = 1000148018,
            BLEND_OP_HARDLIGHT_EXT = 1000148019,
            BLEND_OP_SOFTLIGHT_EXT = 1000148020,
            BLEND_OP_DIFFERENCE_EXT = 1000148021,
            BLEND_OP_EXCLUSION_EXT = 1000148022,
            BLEND_OP_INVERT_EXT = 1000148023,
            BLEND_OP_INVERT_RGB_EXT = 1000148024,
            BLEND_OP_LINEARDODGE_EXT = 1000148025,
            BLEND_OP_LINEARBURN_EXT = 1000148026,
            BLEND_OP_VIVIDLIGHT_EXT = 1000148027,
            BLEND_OP_LINEARLIGHT_EXT = 1000148028,
            BLEND_OP_PINLIGHT_EXT = 1000148029,
            BLEND_OP_HARDMIX_EXT = 1000148030,
            BLEND_OP_HSL_HUE_EXT = 1000148031,
            BLEND_OP_HSL_SATURATION_EXT = 1000148032,
            BLEND_OP_HSL_COLOR_EXT = 1000148033,
            BLEND_OP_HSL_LUMINOSITY_EXT = 1000148034,
            BLEND_OP_PLUS_EXT = 1000148035,
            BLEND_OP_PLUS_CLAMPED_EXT = 1000148036,
            BLEND_OP_PLUS_CLAMPED_ALPHA_EXT = 1000148037,
            BLEND_OP_PLUS_DARKER_EXT = 1000148038,
            BLEND_OP_MINUS_EXT = 1000148039,
            BLEND_OP_MINUS_CLAMPED_EXT = 1000148040,
            BLEND_OP_CONTRAST_EXT = 1000148041,
            BLEND_OP_INVERT_OVG_EXT = 1000148042,
            BLEND_OP_RED_EXT = 1000148043,
            BLEND_OP_GREEN_EXT = 1000148044,
            BLEND_OP_BLUE_EXT = 1000148045,
            BLEND_OP_MAX_ENUM = 0x7FFFFFFF
        };

        enum class BlendFactor
        {
            BLEND_FACTOR_ZERO = 0,
            BLEND_FACTOR_ONE = 1,
            BLEND_FACTOR_SRC_COLOR = 2,
            BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
            BLEND_FACTOR_DST_COLOR = 4,
            BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
            BLEND_FACTOR_SRC_ALPHA = 6,
            BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
            BLEND_FACTOR_DST_ALPHA = 8,
            BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9,
            BLEND_FACTOR_CONSTANT_COLOR = 10,
            BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
            BLEND_FACTOR_CONSTANT_ALPHA = 12,
            BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
            BLEND_FACTOR_SRC_ALPHA_SATURATE = 14,
            BLEND_FACTOR_SRC1_COLOR = 15,
            BLEND_FACTOR_ONE_MINUS_SRC1_COLOR = 16,
            BLEND_FACTOR_SRC1_ALPHA = 17,
            BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA = 18,
            BLEND_FACTOR_MAX_ENUM = 0x7FFFFFFF
        };

        enum class ColorComponentFlagBits
        {
            COLOR_COMPONENT_R_BIT = 0x00000001,
            COLOR_COMPONENT_G_BIT = 0x00000002,
            COLOR_COMPONENT_B_BIT = 0x00000004,
            COLOR_COMPONENT_A_BIT = 0x00000008,
            COLOR_COMPONENT_ALL_TRUE_BIT = 0xf,
            COLOR_COMPONENT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
        };

        enum class AccessFlagBits
        {
            ACCESS_INDIRECT_COMMAND_READ_BIT = 0x00000001,
            ACCESS_INDEX_READ_BIT = 0x00000002,
            ACCESS_VERTEX_ATTRIBUTE_READ_BIT = 0x00000004,
            ACCESS_UNIFORM_READ_BIT = 0x00000008,
            ACCESS_INPUT_ATTACHMENT_READ_BIT = 0x00000010,
            ACCESS_SHADER_READ_BIT = 0x00000020,
            ACCESS_SHADER_WRITE_BIT = 0x00000040,
            ACCESS_COLOR_ATTACHMENT_READ_BIT = 0x00000080,
            ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
            ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = 0x00000200,
            ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = 0x00000400,
            ACCESS_TRANSFER_READ_BIT = 0x00000800,
            ACCESS_TRANSFER_WRITE_BIT = 0x00001000,
            ACCESS_HOST_READ_BIT = 0x00002000,
            ACCESS_HOST_WRITE_BIT = 0x00004000,
            ACCESS_MEMORY_READ_BIT = 0x00008000,
            ACCESS_MEMORY_WRITE_BIT = 0x00010000,
            ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT = 0x02000000,
            ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT = 0x04000000,
            ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT = 0x08000000,
            ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT = 0x00100000,
            ACCESS_COMMAND_PROCESS_READ_BIT_NVX = 0x00020000,
            ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX = 0x00040000,
            ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT = 0x00080000,
            ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV = 0x00800000,
            ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV = 0x00200000,
            ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV = 0x00400000,
            ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT = 0x01000000,
            ACCESS_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
        };

        enum class DependencyFlagBits
        {
            DEPENDENCY_BY_REGION_BIT = 0x00000001,
            DEPENDENCY_DEVICE_GROUP_BIT = 0x00000004,
            DEPENDENCY_VIEW_LOCAL_BIT = 0x00000002,
        };

        enum class DescriptorType
        {
            SAMPLER = 0,
            COMBINED_IMAGE_SAMPLER = 1,
            SAMPLED_IMAGE = 2,
            STORAGE_IMAGE = 3,
            UNIFORM_TEXEL_BUFFER = 4,
            STORAGE_TEXEL_BUFFER = 5,
            UNIFORM_BUFFER = 6,
            STORAGE_BUFFER = 7,
            UNIFORM_BUFFER_DYNAMIC = 8,
            STORAGE_BUFFER_DYNAMIC = 9,
            INPUT_ATTACHMENT = 10,
            NUM_TYPES = 11
        };


        enum class DynamicState
        {
            DYNAMIC_STATE_VIEWPORT = 0,
            DYNAMIC_STATE_SCISSOR = 1,
            DYNAMIC_STATE_LINE_WIDTH = 2,
            DYNAMIC_STATE_DEPTH_BIAS = 3,
            DYNAMIC_STATE_BLEND_CONSTANTS = 4,
            DYNAMIC_STATE_DEPTH_BOUNDS = 5,
            DYNAMIC_STATE_STENCIL_COMPARE_MASK = 6,
            DYNAMIC_STATE_STENCIL_WRITE_MASK = 7,
            DYNAMIC_STATE_STENCIL_REFERENCE = 8,
            DYNAMIC_STATE_VIEWPORT_W_SCALING_NV = 1000087000,
            DYNAMIC_STATE_DISCARD_RECTANGLE_EXT = 1000099000,
            DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT = 1000143000,
            DYNAMIC_STATE_VIEWPORT_SHADING_RATE_PALETTE_NV = 1000164004,
            DYNAMIC_STATE_VIEWPORT_COARSE_SAMPLE_ORDER_NV = 1000164006,
            DYNAMIC_STATE_EXCLUSIVE_SCISSOR_NV = 1000205001,
            DYNAMIC_STATE_LINE_STIPPLE_EXT = 1000259000,
            DYNAMIC_STATE_CULL_MODE_EXT = 1000267000,
            DYNAMIC_STATE_FRONT_FACE_EXT = 1000267001,
            DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT = 1000267002,
            DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT = 1000267003,
            DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT = 1000267004,
            DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT = 1000267005,
            DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT = 1000267006,
            DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT = 1000267007,
            DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT = 1000267008,
            DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT = 1000267009,
            DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT = 1000267010,
            DYNAMIC_STATE_STENCIL_OP_EXT = 1000267011,
            DYNAMIC_STATE_MAX_ENUM = 0x7FFFFFFF
        };

        enum ResourceSets
        {
            CAMERA = 0,
            LIGHT = 1,
            SURFACE = 3, //Left 2 for future use
            TRANSFORM = 4
        };

        enum class RenderLayers
        {
            // OPAQUE, TRANSPARENT are defines in some core windows header file
            OPAQUE_LR,
            TRANSPARENT_LR
        };
    }

    namespace Wrappers
    {
        struct ShaderDescription
        {
            Core::Enums::ShaderType type;
            std::string shaderName;
        };

        struct MeshInfo
        {
            std::bitset<9> attribMaskReq;
            bool isIndexed;
            bool bufferPerAttribRequired;
            Core::Enums::PrimtiveType* primitive = nullptr;
            bool isPrimitiveRestartEnabled;
            glm::vec4 color;
            bool overrideColor = false;
        };

        struct Viewport
        {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        };

        struct Rect2D
        {
            float offsetX;
            float offsetY;
            float lengthX;
            float lengthY;
        };

        #if (RENDERING_API == VULKAN)

            struct ImageCreateInfo
            {
                uint32_t                    m_width, m_height, m_depth, m_mips, m_layers;
                Core::Enums::Format         m_format;
                Core::Enums::ImageViewType  m_viewType;
                Core::Enums::ImageType      m_imageType;
                Core::Enums::ColorSpace     m_colorSpace;
                Core::Enums::Samples        m_sampleCount;
                Core::Enums::ImageLayout    m_initialLayout;
                std::vector<Core::Enums::ImageUsage> m_usages;
            };

            struct ImageViewCreateInfo
            {
                Core::Enums::ImageViewType            m_viewType;
                Core::Enums::Format                   m_format;
                Core::Enums::ComponentSwizzle         m_components[4];
                Core::Enums::ImageAspectFlag          m_imageAspect;
                uint32_t                              m_baseMipLevel;
                uint32_t                              m_levelCount;
                uint32_t                              m_baseArrayLayer;
                uint32_t                              m_layerCount;
            };


            struct BufferCreateInfo
            {
                size_t size;
                std::vector<Core::Enums::BufferUsage> usage;
            };

            struct SamplerCreateInfo
            {
                Core::Enums::Filter magFilter;
                Core::Enums::Filter minFilter;
                Core::Enums::SamplerMipmapMode mipmapMode;
                Core::Enums::SamplerAddressMode addressModeU;
                Core::Enums::SamplerAddressMode addressModeV;
                Core::Enums::SamplerAddressMode addressModeW;
                float mipLodBias;
                bool anisotropyEnable;
                float maxAnisotropy;
                bool compareEnable;
                Core::Enums::CompareOp compareOp;
                float minLod;
                float maxLod;
                Core::Enums::BorderColor borderColor;
                bool unnormalizedCoordinates;
            };

            struct MemoryRequirementInfo
            {
                size_t    size;
                size_t    alignment;
                uint32_t  memoryTypeBits;
            };

            struct RenderPassAttachmentInfo
            {
                RenderPassAttachmentInfo()
                {
                    static uint32_t counter = 0;
                    id = counter++;
                }
                uint32_t id;
                Core::Enums::Format format;
                Core::Enums::Samples sampleCount;
                Core::Enums::LoadOperation loadOp;
                Core::Enums::StoreOperation storeOp;
                Core::Enums::LoadOperation stencilLoadOp;
                Core::Enums::StoreOperation stencilLStoreOp;
                Core::Enums::ImageLayout initialLayout, finalLayout;
            };

            struct AttachmentRef
            {
                uint32_t index;
                Core::Enums::ImageLayout layoutInSubPass;
            };

            struct SubpassInfo
            {
                SubpassInfo()
                {
                    static uint32_t counter = 0;
                    id = counter++;

                    inputAttachmentCount = 0;
                    pInputAttachments = nullptr;
                    colorAttachmentCount = 0;
                    pColorAttachments = nullptr;
                    pResolveAttachments = nullptr;
                    pDepthStencilAttachment = nullptr;
                }
                uint32_t                        id;
                uint32_t                        inputAttachmentCount;
                AttachmentRef *                 pInputAttachments;
                uint32_t                        colorAttachmentCount;
                AttachmentRef *                 pColorAttachments;
                AttachmentRef *                 pResolveAttachments;
                AttachmentRef *                 pDepthStencilAttachment;
            };

            struct SubpassDependency
            {
                SubpassDependency()
                {
                    static uint32_t counter = 0;
                    id = counter++;
                }
                uint32_t id;

                int           srcSubpass; // int (-1) to identify VK_EXTERNAL_SUBPASS
                int           dstSubpass; // int (-1) to identify VK_EXTERNAL_SUBPASS
                std::vector<Core::Enums::PipelineStage>          srcStageMask;
                std::vector<Core::Enums::PipelineStage>          dstStageMask;
                std::vector<Core::Enums::AccessFlagBits>         srcAccessMask;
                std::vector<Core::Enums::AccessFlagBits>         dstAccessMask;
                std::vector<Core::Enums::DependencyFlagBits>     dependencyFlags;
            };

            struct RenderPassBeginInfo
            {
                //float clearColorValue[4]{ -1.0f };
                std::vector<std::array<float, 4>> clearColorValue;
                float depthClearValue = -1.0f;
                float stencilClearValue = -1.0f;
                uint32_t frameBufferId;
                uint32_t renderPassId;
                float renderAreaPosition[2];
                float renderAreaExtent[2];
            };

            struct CommandBufferInheritanceInfo
            {

            };
    
            struct RenderingPassInfo
            {
                Core::Enums::RenderPassTag passTag;
                uint16_t tagMask;
                uint32_t renderPassId;
                uint32_t subpassId;
            };

            struct SubmitInfo
            {
                uint32_t waitSemaphoreCount;
                uint32_t * waitSemaphoreIds;
                uint32_t signalSemaphoreCount;
                uint32_t * signalSemaphoreIds;
                uint32_t commandBufferCount;
                uint32_t * commandBufferIds;
                Core::Enums::PipelineStage pipelineStage;
                std::optional<uint32_t> queueId;
                Core::Enums::PipelineType * queueType;
                Core::Enums::QueueType * purpose;
            };

            struct PresentInfo
            {
                std::vector<uint32_t> m_waitSemaphoreIds;
                std::vector<uint32_t> m_imageIndices;
                std::optional<uint32_t> m_queueId;
            };

            struct QueueWrapper
            {
                uint32_t queueId;
                uint32_t queueFamilyId;
                Core::Enums::PipelineType queueType;
                Core::Enums::QueueType purpose;
            };

            struct BufferInfo
            {
                Core::Enums::BufferType * bufType;
                Core::Enums::MemoryType * memType;
                uint32_t memTypeCount;
                void * data;
                size_t dataSize;
                void * pGpuMem;
            };

            struct VertexInputBindingInfo
            {
                uint32_t             binding;
                uint32_t             stride;
                Core::Enums::VertexIputRate       inputRate;
            };

            struct VertexInputAttributeInfo 
            {
                uint32_t    location;
                uint32_t    binding;
                Core::Enums::Format      format;
                uint32_t    offset;
            };

            struct VertexInputState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::VertexInputState;
                VertexInputBindingInfo * bindingInfo;
                VertexInputAttributeInfo * attribInfo;
                uint32_t bindingCount;
                uint32_t attribCount;
            };

            struct InputAssemblyState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::InputAssemblyState;
                Core::Enums::PrimtiveType * primitiveType;
                bool isPrimtiveRestartEnabled;
            };

            struct TessellationState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::TessellationState;
                uint32_t patchControlPoints;
            };

            struct MultiSampleState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::MultisampleState;
                bool sampleShadingEnable;
                float minSampleShading;
                bool alphaToCoverageEnable;
                bool alphaToOneEnable;
                Core::Enums::Samples sampleCount;
            };

            struct ViewportState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::ViewportState;
                uint32_t viewportCount;
                Viewport* pViewports;
                uint32_t scissorCount;
                Rect2D* pScissors;
            };

            struct RasterizationState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::RasterizationState;
                bool depthClampEnable;
                bool rasterizerDiscardEnable;
                Core::Enums::PolygonMode polygonMode;
                Core::Enums::CullMode cullMode;
                Core::Enums::FrontFace frontFace;
                bool depthBiasEnable;
                float depthBiasConstantFactor;
                float depthBiasClamp;
                float depthBiasSlopeFactor;
                float lineWidth;
            };

            struct StencilOpState 
            {
                Core::Enums::StencilOp failOp;
                Core::Enums::StencilOp passOp;
                Core::Enums::StencilOp depthFailOp;
                Core::Enums::CompareOp compareOp;
                uint32_t compareMask;
                uint32_t writeMask;
                uint32_t reference;
            } ;


            struct DepthStencilState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::DepthStencilState;

                bool depthTestEnable;
                bool depthWriteEnable;
                bool depthBoundsTestEnable;
                bool stencilTestEnable;
                Core::Enums::CompareOp depthCompareOp;
                StencilOpState front;
                StencilOpState back;
                float minDepthBounds;
                float maxDepthBounds;
            };

            struct PipelineColorBlendAttachmentState 
            {

                bool blendEnable;
                Core::Enums::BlendFactor srcColorBlendFactor;
                Core::Enums::BlendFactor dstColorBlendFactor;
                Core::Enums::BlendOp colorBlendOp;
                Core::Enums::BlendFactor srcAlphaBlendFactor;
                Core::Enums::BlendFactor dstAlphaBlendFactor;
                Core::Enums::BlendOp alphaBlendOp;
                Core::Enums::ColorComponentFlagBits colorWriteMask;
            };

            struct ColorBlendState
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::ColorBlendState;

                bool logicOpEnable;
                Core::Enums::LogicOp logicOp;
                uint32_t attachmentCount;
                PipelineColorBlendAttachmentState* pAttachments;
                float blendConstants[4];
            };

            struct DynamicStateList
            {
                Core::Enums::PipelineStates state = Core::Enums::PipelineStates::DynamicState;

                uint32_t                            dynamicStateCount;
                const Core::Enums::DynamicState*    pDynamicStates;
            };

   
            struct DescriptorSetLayoutBinding 
            {
                uint32_t                            binding;
                Core::Enums::DescriptorType         descriptorType;
                uint32_t                            descriptorCount;
                std::vector<Core::Enums::ShaderType> stageFlags;
                //const VkSampler*      pImmutableSamplers;
            };

            struct UniformStructMember
            {
                char name[32];
                uint32_t size;
                uint32_t offset;
                char defaultValue[64];
            };

            struct BindingWrapper
            {
                uint32_t id; // not getting used
                DescriptorSetLayoutBinding bindingObj;
                std::vector<UniformStructMember> memberList;
                std::string bindingName;
            };

            struct SetWrapper
            {
                uint32_t id;// this is unique
                uint32_t setValue; // this is not unique as another layout can be created for the same set number
                std::vector<BindingWrapper> bindingWrapperList;
                std::vector<std::string> shaderNames;
                std::string effectName;
                std::vector<Core::Enums::ShaderType> shaderFlags;
                uint32_t descriptorSetLayoutId;

                //uint32_t descriptorSetId;
            };

            struct PipelineCreateInfo
            {
                std::map<Core::Enums::PipelineStates, uint32_t> statesToIdMap;
                uint32_t renderPassId;
                uint32_t subpassId;
                std::vector<uint32_t> meshList;
                std::vector<SetWrapper*> setsPerPipeline;
                uint32_t pipelineLayoutId;
                uint16_t tagMask;
            };

            // used for vkCmdBindDescriptorSets
            struct DescriptorSetBindingInfo
            {
                Core::Enums::PipelineType* pipelineBindPoint;
                uint32_t pipelineLayoutId;
                uint32_t firstSet;
                std::vector<uint32_t> descriptorSetIds;
                uint32_t descriptorSetId;
                uint32_t dynamicOffsetCount;
                const uint32_t* pDynamicOffsets;
            };

            // vkCmdBindVertexBuffer 
            struct VertexBufferBindingInfo
            {
                uint32_t firstBinding;
                uint32_t bindingCount;
                std::vector<uint32_t> bufferIds;
                std::vector<size_t> pOffsets;
            };

            // vkCmdBindIndexBuffer 
            struct IndexBufferBindingInfo
            {
                uint32_t bufferId;
                size_t offset;
                Core::Enums::IndexType indexType;
            };

            struct IndexedDrawInfo
            {
                uint32_t indexCount;
                uint32_t instanceCount;
                uint32_t firstIndex;
                int32_t vertexOffset;
                uint32_t firstInstance;
            };

            struct DrawArrayInfo
            {
                uint32_t vertexCount;
                uint32_t instanceCount;
                uint32_t firstVertex;
                uint32_t firstInstance;
            };

            struct ShaderWrapper
            {
                std::string shaderName;
                const char* shaderText;
                Core::Enums::ShaderType shaderType;
                uint32_t shaderId;
            };

            struct FrameInfo
            {
                uint32_t m_swapBufferIndex;
                uint32_t m_farmeInFlightIndex;
            };

            class CommandBufferInfo
            {
            private:
                uint32_t m_bufId;
                std::variant<uint32_t, Core::Enums::QueueType> m_poolIdOrPoolType;

                CommandBufferInfo() = delete;
                CommandBufferInfo(CommandBufferInfo const&) = delete;

            public:
                CommandBufferInfo(uint32_t bufId, std::variant<uint32_t, Core::Enums::QueueType> poolIdOrPoolType) :
                    m_bufId(bufId), m_poolIdOrPoolType(poolIdOrPoolType) {}

                const std::variant<uint32_t, Core::Enums::QueueType>& GetPool()
                {
                    return m_poolIdOrPoolType;
                }

                const uint32_t& GetBufferId()
                {
                    return m_bufId;
                }
            };

            struct ImageSubresourceRange
            {
                std::vector<Core::Enums::ImageAspectFlag>          m_imageAspect;
                uint32_t                              m_baseMipLevel;
                uint32_t                              m_levelCount;
                uint32_t                              m_baseArrayLayer;
                uint32_t                              m_layerCount;
            };

            struct MemoryBarrier2
            {
                std::vector<Core::Enums::PipelineStageFlagBits2> m_srcStageMask, m_dstStageMask;
                std::vector<Core::Enums::PipelineAccessFlagBits2> m_srcAccessMask, m_dstAccessMask;
            };

            struct ImageBarrier2
            {
                std::vector<Core::Enums::PipelineStageFlagBits2> m_srcStageMask, m_dstStageMask;
                std::vector<Core::Enums::PipelineAccessFlagBits2> m_srcAccessMask, m_dstAccessMask;
                Core::Enums::ImageLayout   m_oldLayout;
                Core::Enums::ImageLayout   m_newLayout;
                uint32_t                   m_srcQueueFamilyIndex;
                uint32_t                   m_dstQueueFamilyIndex;
                uint32_t                   m_imageId;
                ImageSubresourceRange      m_subresourceRange;
                std::string                m_imageName;
            };

            struct BufferBarrier2
            {
                std::vector<Core::Enums::PipelineStageFlagBits2> m_srcStageMask, m_dstStageMask;
                std::vector<Core::Enums::PipelineAccessFlagBits2> m_srcAccessMask, m_dstAccessMask;
                uint32_t                 m_srcQueueFamilyIndex;
                uint32_t                 m_dstQueueFamilyIndex;
                uint32_t                 m_bufferId;
                size_t                   m_offset;
                size_t                   m_size;
                std::string              m_bufferName;
            };

            struct BarrierDependencyInfo
            {
                uint32_t                         m_memoryBarrierCount;
                const MemoryBarrier2*            m_pMemoryBarriers;
                uint32_t                         m_bufferMemoryBarrierCount;
                const BufferBarrier2*            m_pBufferMemoryBarriers;
                uint32_t                         m_imageMemoryBarrierCount;
                const ImageBarrier2*             m_pImageMemoryBarriers;
            };


        #elif (RENDERING_API == DX12)

        #endif

        inline Core::Enums::RenderPassTag operator | (const Core::Enums::RenderPassTag& lhs, const Core::Enums::RenderPassTag& rhs)
        {
            using T = std::underlying_type_t <Core::Enums::RenderPassTag>;
            return static_cast<Core::Enums::RenderPassTag>(static_cast<T>(lhs) | static_cast<T>(rhs));
        }
    }

    namespace Utility
    {
        std::string ConvertPipelineStageFlagToString(const Core::Enums::PipelineStageFlagBits2& flag);
        std::string ConvertPipelineAccessFlagToString(const Core::Enums::PipelineAccessFlagBits2& flag);
        std::string ConvertImageLayoutToString(const Core::Enums::ImageLayout& layout);

    }
}
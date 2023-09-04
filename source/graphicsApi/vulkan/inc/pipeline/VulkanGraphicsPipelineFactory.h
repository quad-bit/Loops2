#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include "utility/PipelineStateWrappers.h"

class VulkanGraphicsPipeline;
class Shader;

namespace GfxVk
{
    namespace VulkanPipeline
    {
        struct VkShaderStageWrapper
        {
            uint32_t id;
            VkPipelineShaderStageCreateInfo* shaderStageCreateInfo;
            uint32_t shaderCount;
        };

        class VulkanGraphicsPipelineFactory
        {
        private:
            VulkanGraphicsPipelineFactory() {}
            VulkanGraphicsPipelineFactory(VulkanGraphicsPipelineFactory const&) {}
            VulkanGraphicsPipelineFactory const& operator= (VulkanGraphicsPipelineFactory const&) {}

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

            std::vector<VulkanGraphicsPipeline*> pipelineList;
            std::map<uint32_t, VkPipelineVertexInputStateCreateInfo*> idToVertexInputMap;
            std::map<uint32_t, VkPipelineInputAssemblyStateCreateInfo*> idToInputAssemblyMap;
            std::vector<VkShaderStageWrapper*> shaderStageWrapperList;
            std::map<uint32_t, VkPipelineTessellationStateCreateInfo*> idToTessellationMap;
            std::map<uint32_t, VkPipelineViewportStateCreateInfo*> idToViewportMap;
            std::map<uint32_t, VkPipelineRasterizationStateCreateInfo*> idToRasterizationMap;
            std::map<uint32_t, VkPipelineMultisampleStateCreateInfo*> idToMultiSampleMap;
            std::map<uint32_t, VkPipelineDepthStencilStateCreateInfo*> idToDepthStencilMap;
            std::map<uint32_t, VkPipelineColorBlendStateCreateInfo*> idToColorBlendMap;
            std::map<uint32_t, VkPipelineDynamicStateCreateInfo*> idToDynamicMap;
            std::map<uint32_t, VkGraphicsPipelineCreateInfo*> idToPipelineInfoMap;
            std::map<uint32_t, VkPipeline*> idToPipelineMap;

            std::vector<VkPipeline*> tempVectorToBeDeleted;

            //deprecated
            VkVertexInputAttributeDescription UnwrapVertexInputAttributeInfo(Core::Wrappers::VertexInputAttributeInfo* info);
            //deprecated
            VkVertexInputBindingDescription UnwrapVertexInputBindingInfo(Core::Wrappers::VertexInputBindingInfo* info);

            VkVertexInputAttributeDescription* UnwrapVertexInputAttributeInfo(Core::Wrappers::VertexInputAttributeInfo* info, const uint32_t& count);
            VkVertexInputBindingDescription* UnwrapVertexInputBindingInfo(Core::Wrappers::VertexInputBindingInfo* info, const uint32_t& count);
            VkPrimitiveTopology UnwrapPrimitiveInfo(Core::Enums::PrimtiveType* primitive);
            VkCullModeFlags UnwrapCullMode(Core::Enums::CullMode mode);
            VkFrontFace UnwrapFrontFace(Core::Enums::FrontFace face);
            VkPolygonMode UnwrapPolygonMode(Core::Enums::PolygonMode mode);
            VkStencilOpState UnwrapStencilOpState(Core::Wrappers::StencilOpState state);
            VkStencilOp UnwrapStencilOp(Core::Enums::StencilOp op);
            VkPipelineColorBlendAttachmentState* UnwrapColorBlendAttachment(Core::Wrappers::PipelineColorBlendAttachmentState* state, const uint32_t& count);
            VkLogicOp UnwrapLogicOp(Core::Enums::LogicOp op);
            VkBlendOp UwrapBlendOp(Core::Enums::BlendOp op);
            VkColorComponentFlags UnwrapColorComponents(Core::Enums::ColorComponentFlagBits bits);
            VkBlendFactor UnwrapBlendFactor(Core::Enums::BlendFactor blend);
            VkDynamicState* UnwrapDynamicList(const Core::Enums::DynamicState* pDynamicStates, const uint32_t& dynamicStateCount);

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

            void CreateVertexInputState(const Core::Utility::VertexInputWrapper* vertexInputWrapper);
            void CreateInputAssemblyState(const Core::Utility::InputAssemblyWrapper* InputAssemblyWrapper);
            void CreateShaderState(const Core::Utility::ShaderStateWrapper* shaderStateWrapper);
            void CreateTessellationState(const Core::Utility::TessellationStateWrapper* wrapper);
            void CreateViewportState(const Core::Utility::ViewPortStateWrapper* wrapper);
            void CreateRasterisationState(const Core::Utility::RasterizationStateWrapper* wrapper);
            void CreateDepthStencilState(const Core::Utility::DepthStencilStateWrapper* wrapper);
            void CreateColorBlendState(const Core::Utility::ColorBlendStateWrapper* wrapper);
            void CreateMultiSampleState(const Core::Utility::MultiSampleStateWrapper* wrapper);
            void CreateDynamicState(const Core::Utility::DynamicStateWrapper* wrapper);

            void CreatePipeline(Core::Wrappers::PipelineCreateInfo* info, const uint32_t& pipelineCount, uint32_t* pipelineId);

            VkPipeline* GetPipeline(const uint32_t& id);
        };
    }
}
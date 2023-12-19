#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <array>
#include <map>
#include "utility/VkRenderingUnwrapper.h"


namespace Core
{
    namespace Enums
    {
        struct RenderPassAttachmentInfo;
        struct SubpassInfo;
        struct SubpassDependency;
    }
}

namespace GfxVk
{
    namespace Renderpass
    {
        class RenderpassWrapper
        {
        public:
            uint32_t id;
            VkRenderPass renderPass;
            VkRenderPassBeginInfo beginInfo;
            //std::vector<VkClearValue> clearValue;
            VkClearValue* clearValue;
            uint32_t clearValueCount;
        };

        struct RenderingInfoWrapper
        {
            uint32_t m_id;
            VkRenderingInfo m_renderingInfo;
            std::vector<VkRenderingAttachmentInfo> m_colorAttachmentInfo;
            std::optional<VkRenderingAttachmentInfo> m_depthAttachmentInfo;
        };

        class VkRenderPassFactory
        {
        private:
            VkRenderPassFactory() {}
            VkRenderPassFactory(VkRenderPassFactory const&) {}
            VkRenderPassFactory const& operator= (VkRenderPassFactory const&) {}

            static VkRenderPassFactory* instance;
            static uint32_t renderpassIdCounter;

            std::vector<VkAttachmentReference> refs;
            uint32_t refCounter;
            std::vector<RenderpassWrapper*> renderpassList;
            std::map<uint32_t, RenderingInfoWrapper> m_renderingInfoList;

            uint32_t GetId();

        public:
            void Init();
            void DeInit();
            void Update();
            static VkRenderPassFactory* GetInstance();
            ~VkRenderPassFactory();

            void CreateRenderPass(
                const VkAttachmentDescription* renderpassAttachmentList, const uint32_t& attachmentCount,
                const VkSubpassDescription* subpassList, const uint32_t& subpassCount,
                const VkSubpassDependency* dependencyList, const uint32_t& dependencyCount,
                uint32_t& renderPassId
            );

            // deprecated.
            void CreateRenderPass(
                const VkAttachmentDescription* renderpassAttachmentList, const uint32_t& attachmentCount,
                const VkSubpassDescription* subpassList, const uint32_t& subpassCount,
                const VkSubpassDependency* dependencyList, const uint32_t& dependencyCount,
                const VkRenderPassBeginInfo beginInfo, uint32_t& renderPassId
            );
            void DestroyRenderPass(const uint32_t& id);

            void SetRenderPassBeginInfo(const VkRenderPassBeginInfo beginInfo, const uint32_t& renderpassId);
            void SetClearColor(std::vector<VkClearValue> clearValue, const uint32_t& renderPassId);

            VkRenderPass* GetRenderPass(const uint32_t& id);
            VkClearValue* GetClearValue(const uint32_t& renderpassId);
            uint32_t GetClearValueCount(const uint32_t& renderpassId);

            uint32_t CreateRenderingInfo(const Core::Wrappers::RenderingInfo& renderingInfo);
            const VkRenderingInfo& GetRenderingInfo(uint32_t infoId) const;
        };
    }
}
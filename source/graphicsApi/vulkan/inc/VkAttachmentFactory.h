#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <stdint.h>
#include "utility/VkRenderingUnwrapper.h"

namespace Core
{
    namespace Enums
    {
        enum class Format;
    }
    namespace Wrappers
    {
        struct ImageInfo;
    }
}

namespace GfxVk
{
    namespace Framebuffer
    {
        class AttachmentWrapper
        {
        public:
            uint32_t id;
            bool attachmentActive;
            VkImage image;
            VkImageView imageView;
            VkDeviceMemory imageMemory = VK_NULL_HANDLE;
            uint32_t memoryId;
            VkMemoryRequirements imageMemoryRequirements;
            VkDeviceSize offsetInMemory;
            VkImageUsageFlags usage;
            bool isDefaultTarget;
            AttachmentWrapper()
            {
                attachmentActive = false;
                id = -1;
            }

            void DeActivateAttachment();
        };

        struct AttachmentInfo
        {
            uint32_t width, height, mips, layers;
            VkFormat format;
            VkImageViewType viewType;
            VkImageType imageType;
            VkColorSpaceKHR colorSpace;
            VkImageUsageFlags usage;
            VkSampleCountFlagBits sampleCount;
            VkImageLayout initialLayout;
        };

        class VkAttachmentFactory
        {
        private:
            VkAttachmentFactory() {}
            VkAttachmentFactory(VkAttachmentFactory const&) {}
            VkAttachmentFactory const& operator= (VkAttachmentFactory const&) {}

            static VkAttachmentFactory* instance;
            static uint32_t imageId;
            VkFormat bestDepthFormat;

            std::vector<AttachmentWrapper* > attachmentList;
            std::vector<VkImage* > imageList;
            std::vector<VkImageView* > imageViewList;
            std::vector<VkDeviceMemory* > memoryList;

            AttachmentInfo* UnwrapImageInfo(Core::Wrappers::ImageInfo* imageInfo);
            uint32_t GetId();

        public:
            void Init();
            void DeInit();
            void Update();
            static VkAttachmentFactory* GetInstance();
            ~VkAttachmentFactory();

            int FindBestDepthFormat(Core::Enums::Format* format, uint32_t count);

            // deprecated.
            void CreateColorAttachment(Core::Wrappers::ImageInfo* info, uint32_t count, bool defaultTarget, std::vector<uint32_t>* ids);
            void CreateSwapChainImage(VkImageCreateInfo imageCreateinfo, VkImageViewCreateInfo imageViewCreateinfo, const uint32_t& count, uint32_t* ids);

            void CreateAttachment(VkImageCreateInfo* info, uint32_t count, uint32_t* ids);

            Core::Wrappers::MemoryRequirementInfo GetImageMemoryRequirement(const uint32_t& id);
            //Needed here as imageViewInfo has image id 
            void CreateImageView(Core::Wrappers::ImageViewInfo* info, const uint32_t& count);
            void CreateImageView(VkImageViewCreateInfo* info, const uint32_t& count, AttachmentWrapper** wrappers);

            //deprecated
            void DestroyAttachment(std::vector<uint32_t> ids, bool defaultTarget);
            void DestroyAttachment(uint32_t* ids, bool* destroyImageView, bool* freeImageMemory, const uint32_t& count);
            void FreeAttachmentMemory(uint32_t* imageIds, const uint32_t& count);
            void DestroySwapChainImageViews(uint32_t* ids, const uint32_t& count);

            VkFormat GetBestDepthFormat() { return bestDepthFormat; }

            VkImageView GetImageView(uint32_t id);
            VkImage* GetImage(const uint32_t& id);
            void BindImageMemory(const uint32_t& imageId, const uint32_t& memId, const size_t& offset);
        };
    }
}
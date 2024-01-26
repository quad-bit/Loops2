#ifndef GFXVULKAN_VKIMAGEFACTORY_H_
#define GFXVULKAN_VKIMAGEFACTORY_H_

#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <map>

namespace GfxVk
{
    namespace Utility
    {
        struct ImageWrapper
        {
            uint32_t m_imageId;
            VkImage m_image = VK_NULL_HANDLE;
            VkImageView m_imageView = VK_NULL_HANDLE;
            std::optional<uint32_t> m_memId;
            std::string m_name;
        };

        class VkImageFactory
        {
        private:
            VkImageFactory() {}
            VkImageFactory(VkImageFactory const&) {}
            VkImageFactory const& operator= (VkImageFactory const&) {}

            static VkImageFactory* s_instance;
            static uint32_t s_imageIdCounter;

            uint32_t GetId();

            std::map<uint32_t, ImageWrapper> m_imageList, m_swapchainImageList;
            uint32_t m_transferQueueId;

        public:
            void Init(uint32_t swapBufferCount, uint32_t transferQueueId);
            void DeInit();
            void Update();
            static VkImageFactory* GetInstance();
            ~VkImageFactory();

            uint32_t CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const Core::Wrappers::ImageViewCreateInfo& viewInfo, const std::string& name);
            uint32_t CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name, bool createView = false);
            uint32_t CreateImage(void* buffer, size_t bufferSize, const Core::Wrappers::ImageCreateInfo& imageInfo, const std::string& name);

            void CreateImageView(const Core::Wrappers::ImageViewCreateInfo& viewInfo, uint32_t imageId);
            void DestroyImage(uint32_t imageId, bool freeImageMemory);

            const VkImageView& GetImageView(uint32_t id);
            const VkImage& GetImage(uint32_t id);
            Core::Wrappers::MemoryRequirementInfo GetImageMemoryRequirement(uint32_t id);
            void BindImageMemory(uint32_t imageId, uint32_t memId, size_t offset);

            const uint32_t& GetSwapchainImage(uint32_t swapchainIndex);

        };
    }
}

#endif //GFXVULKAN_VKIMAGEFACTORY_H_
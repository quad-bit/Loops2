#ifndef GFXVULKAN_VKIMAGEFACTORY_H_
#define GFXVULKAN_VKIMAGEFACTORY_H_

#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <vulkan/vulkan.h>
#include <optional>

namespace GfxVk
{
    namespace Utility
    {
        struct ImageWrapper
        {
            uint32_t m_imageId;
            VkImage m_image;
            VkImageView m_imageView;
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

            std::vector<ImageWrapper> m_imageList;

        public:
            void Init();
            void DeInit();
            void Update();
            static VkImageFactory* GetInstance();
            ~VkImageFactory();

            uint32_t CreateImage(const Core::Wrappers::ImageCreateInfo& imageInfo, const Core::Wrappers::ImageViewCreateInfo& viewInfo, const std::string& name );
            void DestroyImage(uint32_t imageId, bool freeImageMemory);

            VkImageView GetImageView(uint32_t id);
            VkImage GetImage(uint32_t id);
            Core::Wrappers::MemoryRequirementInfo GetImageMemoryRequirement(uint32_t id);
            void BindImageMemory(uint32_t imageId, uint32_t memId, size_t offset);
        };
    }
}

#endif //GFXVULKAN_VKIMAGEFACTORY_H_
#ifndef RENDERER_RESOURCE_H_
#define RENDERER_RESOURCE_H_

#include <optional>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <RendererSettings.h>
#include <VulkanInterface.h>

namespace Renderer
{
    namespace ResourceManagement
    {
        enum class ResourceType
        {
            BUFFER,
            IMAGE
        };

        class Resource
        {
        protected:
            /// <summary>
            /// Will point to the TextureId or BufferId
            /// </summary>
            uint32_t m_physicalResourceId;
            uint32_t m_memId;
            ResourceType m_resourceType;
            std::string m_name;

        public:
            Resource() = delete;
            Resource(const ResourceType& resourceType, const std::string& name) :
                m_resourceType(resourceType), m_name(name)
            {}
            virtual ~Resource()
            {

            }

            void AssignPhysicalResourceId(const uint32_t id)
            {
                m_physicalResourceId = id;
            }

            uint32_t GetPhysicalResourceId()
            {
                return m_physicalResourceId;
            }

            uint32_t GetResourceMemoryId()
            {
                return m_memId;
            }

            std::string GetResourceName()
            {
                return m_name;
            }
        };

        class ImageResource final : public Resource
        {
        private:
            Core::Enums::ImageLayout m_oldLayout, m_newLayout;
            size_t m_width, m_height;
            bool m_isOccupyingSharedMemory;
        public:

            // The image is created externally and the id have been assigned
            ImageResource(
                uint32_t id,
                std::string name,
                const size_t& width,
                const size_t& height,
                uint32_t memId,
                bool isMemoryShared): 
                Resource(ResourceType::IMAGE, name),
                m_width(width),
                m_height(height),
                m_isOccupyingSharedMemory(isMemoryShared)
            {
                m_physicalResourceId = id;
                m_memId = memId;
            }

            ImageResource(const size_t& width, const size_t& height, const std::string& name) :
                Resource(ResourceType::IMAGE, name), m_width(width), m_height(height)
            {
                /*Core::Enums::Format format{ Core::Enums::Format::B8G8R8A8_UNORM };
                Core::Enums::ImageType type{ Core::Enums::ImageType::IMAGE_TYPE_2D };
                std::vector<Core::Enums::ImageUsage> usages{ Core::Enums::ImageUsage::USAGE_SAMPLED_BIT };

                Core::Wrappers::ImageCreateInfo createInfo{};
                createInfo.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
                createInfo.m_depth = 1;
                createInfo.m_format = format;
                createInfo.m_height = height;
                createInfo.m_imageType = type;
                createInfo.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
                createInfo.m_layers = 1;
                createInfo.m_mips = 1;
                createInfo.m_sampleCount = Renderer::RendererSettings::GetMaxSampleCountAvailable();
                createInfo.m_usages = usages;
                createInfo.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;
                createInfo.m_width = width;

                m_imageId = VulkanInterfaceAlias::CreateImage(createInfo, name);*/
            }

            ~ImageResource()
            {
                //if(!m_isOccupyingSharedMemory)
                    //VulkanInterfaceAlias::DestroyImage(m_imageId, true);
            }
        };

        class BufferResource final : public Resource
        {
        private:
            size_t m_dataSize, m_dataSizeAligned;

        public:
            BufferResource(const size_t& dataSize, const std::string& name) :
                Resource(ResourceType::BUFFER, name), m_dataSize(dataSize)
            {}
        };
    }
}

typedef Renderer::ResourceManagement::Resource ResourceAlias;
typedef Renderer::ResourceManagement::ImageResource ImageResourceAlias;
typedef Renderer::ResourceManagement::BufferResource BufferResourceAlias;
#endif // RENDERER_RESOURCE_H_
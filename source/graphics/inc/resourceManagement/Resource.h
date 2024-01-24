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
            bool m_isOccupyingSharedMemory;

        public:
            Resource() = delete;
            Resource(const ResourceType& resourceType, const std::string& name, bool isOccupyingSharedMemory) :
                m_resourceType(resourceType), m_name(name), m_isOccupyingSharedMemory(isOccupyingSharedMemory)
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

            ResourceType GetResourceType()
            {
                return m_resourceType;
            }
        };

        class ImageResource final : public Resource
        {
        private:
            Core::Enums::ImageLayout m_oldLayout, m_newLayout;
            size_t m_width, m_height;
            std::optional<std::array<float, 4>> m_clearColorValues;
            std::optional<std::array<float, 2>> m_clearDepthStencilValues;
            Core::Enums::Format m_imageFormat;

        public:

            // The image is created externally and the id have been assigned
            ImageResource(
                uint32_t id,
                std::string name,
                const size_t& width,
                const size_t& height,
                uint32_t memId,
                const Core::Enums::Format& imageFormat,
                bool isMemoryShared): 
                Resource(ResourceType::IMAGE, name, isMemoryShared),
                m_width(width),
                m_height(height),
                m_imageFormat(imageFormat)
            {
                m_physicalResourceId = id;
                m_memId = memId;
                m_clearColorValues = std::array<float, 4>{0.3f, 0.2f, 0.5f, 1.0f};
                m_clearDepthStencilValues = std::array<float, 2>{1.0f, 0u};
            }

            void SetClearColorValue(std::array<float, 4> clearColor)
            {
                m_clearColorValues = clearColor;
            }

            void SetClearDepthValue(std::array<float, 2> clearDepthStencil)
            {
                m_clearDepthStencilValues = clearDepthStencil;
            }

            std::array<float, 4>& GetClearColorValue()
            {
                return m_clearColorValues.value();
            }

            std::array<float, 2>& GetClearDepthValue()
            {
                return m_clearDepthStencilValues.value();
            }

            const Core::Enums::Format& GetImageFormat() const
            {
                return m_imageFormat;
            }

            uint32_t GetWidth()
            {
                return m_width;
            }

            uint32_t GetHeight()
            {
                return m_height;
            }
            /*
            ImageResource(const size_t& width, const size_t& height, const std::string& name) :
                Resource(ResourceType::IMAGE, name), m_width(width), m_height(height)
            {
                Core::Enums::Format format{ Core::Enums::Format::B8G8R8A8_UNORM };
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

                m_imageId = VulkanInterfaceAlias::CreateImage(createInfo, name);
            }
            */
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
            size_t m_offset;
            //Core::Enums::BufferType m_bufferType;
            std::vector<Core::Enums::BufferUsage> m_bufferUsage;

        public:
            /*BufferResource(const size_t& dataSize, const std::string& name) :
                Resource(ResourceType::BUFFER, name), m_dataSize(dataSize)
            {}
            */
            // The buffer is created externally and the id have been assigned
            BufferResource(
                uint32_t id,
                std::string name,
                const size_t& dataSize,
                const size_t& dataSizeAligned,
                const size_t& offset,
                const std::vector< Core::Enums::BufferUsage>& bufferUsage,
                uint32_t memId,
                bool isMemoryShared) :
                Resource(ResourceType::BUFFER, name, isMemoryShared),
                m_dataSize(dataSize),
                m_dataSizeAligned(dataSizeAligned),
                m_offset(offset),
                m_bufferUsage(bufferUsage)
            {
                m_physicalResourceId = id;
                m_memId = memId;
                
            }

            const size_t GetDataSize()
            {
                return m_dataSize;
            }

            const size_t GetAlignedDataSize()
            {
                return m_dataSize;
            }

            const size_t GetDataOffset()
            {
                return m_dataSize;
            }
        };
    }
}

typedef Renderer::ResourceManagement::Resource ResourceAlias;
typedef Renderer::ResourceManagement::ImageResource ImageResourceAlias;
typedef Renderer::ResourceManagement::BufferResource BufferResourceAlias;
#endif // RENDERER_RESOURCE_H_
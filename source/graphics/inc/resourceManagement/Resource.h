#ifndef RENDERER_RESOURCE_H_
#define RENDERER_RESOURCE_H_

#include <optional>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

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
            /// The resource might be represented by multiple nodes in the graph, hence the logical id 
            /// is assigned to identify the uniqueness of the resource thorughtout the graph.
            /// The physicalId will be assigned in later stages.
            /// </summary>
            uint32_t m_logicalResourceId;

            /// <summary>
            /// Will point to the TextureId or BufferId
            /// </summary>
            std::optional<uint32_t> m_physicalResourceId;
            ResourceType m_resourceType;
            std::string m_name;

        public:
            Resource() = delete;
            Resource(const ResourceType& resourceType, const std::string& name) :
                m_resourceType(resourceType), m_name(name)
            {}

            void AssignPhysicalResourceId(const uint32_t id)
            {
                m_physicalResourceId = id;
            }

            std::string GetResourceName()
            {
                return m_name;
            }
        };

        class ImageResource final : public Resource
        {
        private:
            Core::Enums::ImageLayout m_currentLayout, m_targetLayout;
            size_t m_width, m_height;

        public:
            ImageResource(const size_t& width, const size_t& height, const std::string& name) :
                Resource(ResourceType::IMAGE, name), m_width(width), m_height(height)
            {}
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
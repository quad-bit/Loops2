#ifndef RENDERER_UTILS_H_
#define RENDERER_UTILS_H_

#include <string>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <optional>
#include "renderGraph/Graph.h"
#include <functional>
#include "resourceManagement/Resource.h"

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Utils
        {
            enum class RenderGraphNodeType
            {
                TASK_NODE,
                RESOURCE_NODE
            };

            enum class ResourceMemoryUsage
            {
                READ_ONLY,
                READ_WRITE,
                WRITE_ONLY,
                NONE //for output node resource nodes
            };

            enum class PipelineActivityState
            {
                COMPILE,
                EXECUTE,
                DESTROY,
                NONE
            };

            struct ImageResourceConnectionInfo
            {
                std::vector<Core::Enums::ImageUsage> m_imageUsages;
                Core::Enums::ImageLayout m_prevImageLayout;
                Core::Enums::ImageLayout m_expectedImageLayout;
            };

            struct ConnectionInfo
            {
                Renderer::RenderGraph::Utils::ResourceMemoryUsage m_usage;
                std::vector<ResourceAlias*> m_resource;
                uint32_t m_resourceParentNodeId;
                ImageResourceConnectionInfo* m_imageInfo = nullptr;
            };

            class RenderGraphNodeBase;

            struct ResourceCreationCallback
            {
                std::function<Renderer::ResourceManagement::Resource* (const Core::Wrappers::ImageCreateInfo& imageCreateInfo, const std::string& name)> CreateImageFunc;
                std::function<Renderer::ResourceManagement::Resource* (const Core::Wrappers::BufferCreateInfo& bufferCreateInfo, const std::string& name)> CreateBufferFunc;
                std::function<std::vector<Renderer::ResourceManagement::Resource*> (const Core::Wrappers::ImageCreateInfo& imageCreateInfo, const std::vector<std::string>& name)> CreatePerFrameImageFunc;
                std::function<std::vector<Renderer::ResourceManagement::Resource*>()> GetSwapchainImagesFunc;
            };

            struct GraphTraversalCallback
            {
                std::function<void(Renderer::RenderGraph::Utils::RenderGraphNodeBase*)> PipelineCompileCallback;
            };

            struct CallbackUtility
            {
                ResourceCreationCallback m_resourceCreationCallback;
                GraphTraversalCallback m_graphTraversalCallback;
            };

            class RenderGraphNodeBase
            {
            protected:
                RenderGraphNodeType m_type;
                const std::string m_name;
                Utils::GraphTraversalCallback m_graphTraversalCallback;
                uint32_t m_id;

            public:
                RenderGraphNodeBase(const RenderGraphNodeType& nodeType, const std::string& nodeName, Utils::GraphTraversalCallback& funcs) :
                    m_type(nodeType), m_name(nodeName), m_graphTraversalCallback(funcs)
                {}

                void SetId(uint32_t id)
                {
                    m_id = id;
                }

                const uint32_t& GetId()
                {
                    return m_id;
                }

                const std::string& GetNodeName()
                {
                    return m_name;
                }

                const RenderGraphNodeType& GetNodeType()
                {
                    return m_type;
                }

                virtual void Execute() = 0;
            };

            void AddEdge(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
                const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage = ResourceMemoryUsage::NONE);

            void AddEdge(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
                const Renderer::RenderGraph::Utils::ConnectionInfo& connectionInfo);


            void AddEdge(
                Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* srcNode,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
                const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage,
                const Core::Enums::ImageLayout expectedImageLayout,
                const Core::Enums::ImageLayout previousImageLayout);

            std::pair<std::vector<uint32_t>, std::vector<uint32_t>> CreatePerFrameImageResource(
                const Core::Wrappers::ImageCreateInfo& createInfo,
                std::vector<std::string> names,
                uint32_t count);

            void AddSwapchainNode(
                Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph,
                Renderer::RenderGraph::GraphNode<RenderGraphNodeBase>* destNode,
                const Renderer::RenderGraph::Utils::ResourceMemoryUsage& usage,
                const Core::Enums::ImageLayout expectedImageLayout
            );

            std::vector<uint32_t> GetSwapchainImages();

            void DestroyPerFrameImageResource(const std::vector<uint32_t>& imageIds, std::vector<uint32_t>& memIds);
        }
    }
}

#endif //RENDERER_UTILS_H_

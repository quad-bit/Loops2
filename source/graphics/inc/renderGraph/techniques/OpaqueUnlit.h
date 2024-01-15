#ifndef RENDERER_OPAQUEUNLIT_H_
#define RENDERER_OPAQUEUNLIT_H_

#include <renderGraph/Technique.h>
#include <RenderData.h>
#include <Settings.h>
#include "renderGraph/Task.h"

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Techniques
        {
            class OpaqueUnlit : public Technique
            {
            private:
                const Core::Utility::RenderData& m_renderData;
                void CreateResources() override;

                std::vector<ResourceAlias*> m_depthAttachments, m_backBufferImages;
                std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> m_depthInputNodeBase, m_colorInputNodeBase, m_colorOutputNodeBase;
                Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* m_depthInputGraphNode, * m_colorInputGraphNode,* m_colorOutputGraphNode;

                std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> m_renderTaskNodeBase;
                Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* m_renderTaskGraphNode;
                std::unique_ptr<Renderer::RenderGraph::Task> m_opaqueRenderTask;

                uint32_t m_renderHeight, m_renderWidth;

            public:
                OpaqueUnlit(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
                    const std::string& name,
                    const std::string& effectName);

                ~OpaqueUnlit();

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override;
                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override;

                virtual void SetupFrame(const Core::Wrappers::FrameInfo& frameInfo) override;
            };
        }
    }
}

#endif //RENDERER_OPAQUEUNLIT_H_

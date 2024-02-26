#ifndef RENDERER_BOUNDPASS_H_
#define RENDERER_BOUNDPASS_H_

#include <renderGraph/Effect.h>
#include <RenderData.h>
#include <Settings.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Effects
        {
            class BoundPass : public Effect
            {
            private:
                Core::Utility::RenderData& m_renderData;
                const Core::WindowSettings& m_windowSettings;

            public:
                BoundPass(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs,
                    const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& inputNodes
                );

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
                {
                    return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>{};
                }

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
                {
                    return m_techniques[0]->GetGraphEndResourceNodes();
                }
            };
        }
    }
}

#endif //RENDERER_BOUNDPASS_H_

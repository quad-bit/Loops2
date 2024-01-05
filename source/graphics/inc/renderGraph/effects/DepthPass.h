#ifndef RENDERER_DEPTHPASS_H_
#define RENDERER_DEPTHPASS_H_

#include <renderGraph/Effect.h>
#include "Settings.h"
#include "RenderData.h"

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Effects
        {
            class DepthPass : public Effect
            {
            private:
                Core::Utility::RenderData& m_renderData;
                const Core::WindowSettings& m_windowSettings;
            public:
                DepthPass(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs);

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
                {
                    return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>{};
                }

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
                {
                    return std::vector{ m_techniques[0]->GetResourceNode("transferOutput") };
                }
            };
        }
    }
}

#endif //RENDERER_DEPTHPASS_H_

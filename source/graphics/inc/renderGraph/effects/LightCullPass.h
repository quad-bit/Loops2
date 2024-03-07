#ifndef RENDERER_LIGHTCULLPASS_H_
#define RENDERER_LIGHTCULLPASS_H_

#include <renderGraph/Effect.h>
#include <RenderData.h>
#include <Settings.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Effects
        {
            class LightCullPass : public Effect
            {
            private:
                Core::Utility::RenderData& m_renderData;
                const Core::WindowSettings& m_windowSettings;

            public:
                LightCullPass(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs
                );

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
                {
                    return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>{};
                }

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
                {
                    return std::vector{ m_techniques[0]->GetGraphEndResourceNodes()};
                }
            };
        }
    }
}

#endif //RENDERER_LIGHTCULLPASS_H_

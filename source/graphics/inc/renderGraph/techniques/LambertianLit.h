#ifndef RENDERER_LAMBERTIANLIT_H_
#define RENDERER_LAMBERTIANLIT_H_

#include <renderGraph/Technique.h>
#include <RenderData.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Techniques
        {
            class LambertianLit : public Technique
            {
            private:
                Core::Utility::RenderData& m_renderData;

            public:
                LambertianLit(Core::Utility::RenderData& renderData,
                Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                Renderer::RenderGraph::Utils::CallbackUtility& funcs,
                const std::string& effectName,
                const Core::Utility::EffectInfo& effectInfo) :
                Technique(graph, "LambertianLit", funcs, effectName, effectInfo), m_renderData(renderData)
                {
                    // Create resource node
                }

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override;
                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override;

            };
        }
    }
}

#endif //RENDERER_LAMBERTIANLIT_H_

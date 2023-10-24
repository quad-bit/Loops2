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
                const Core::Utility::RenderData& m_renderData;

            public:
                LambertianLit(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
                    Technique(graph, "LambertianLit", funcs), m_renderData(renderData)
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

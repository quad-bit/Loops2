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
                LambertianLit(const Core::Utility::RenderData& renderData, const Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) :
                    Technique(graph, "LambertianLit"), m_renderData(renderData)
                {
                    // Create resource node

                }
            };
        }
    }
}

#endif //RENDERER_LAMBERTIANLIT_H_

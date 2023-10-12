#ifndef RENDERER_DEPTHVIEWPIPELINE_H_
#define RENDERER_DEPTHVIEWPIPELINE_H_

#include <renderGraph/Pipeline.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Pipelines
        {
            class DepthViewPipeline : public Pipeline
            {
            private:
                virtual void ValidatePipeline() override;
                virtual void CreateLogicalPasses() override;
                virtual void CreatePassResources() override;
                virtual void CreateRenderPasses() override;
                virtual void CreateSynchronisations() override;

            public:
                ~DepthViewPipeline() {}
                DepthViewPipeline(const Core::Utility::RenderData& renderData, const Renderer::RenderGraph::Graph<RenderGraphNode>& graph);

                virtual void CreatePipeline() override;
                virtual void DestroyPipeline() override;
                virtual void ExecutePipeline() override;
            };
        }
    }
}

#endif //RENDERER_DEPTHVIEWPIPELINE_H_

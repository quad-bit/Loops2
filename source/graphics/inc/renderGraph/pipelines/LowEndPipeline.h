#ifndef RENDERER_LOWENDPIPELINE_H_
#define RENDERER_LOWENDPIPELINE_H_

#include <renderGraph/Pipeline.h>
#include <RenderData.h>
#include <renderGraph/Effect.h>

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Pipelines
        {
            class LowEndPipeline : public Pipeline
            {
            protected:
                /*virtual void ValidatePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void CreateLogicalPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void CreatePassResources(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void CreateRenderPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void CreateSynchronisations(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;*/

            public:
                ~LowEndPipeline() {}
                LowEndPipeline(Core::Utility::RenderData& renderData, const std::string& name);

                /*virtual void CreatePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void CompilePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;
                virtual void DestroyPipeline() override;
                virtual void ExecutePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) override;*/
            };
        }
    }
}

#endif //RENDERER_LOWENDPIPELINE_H_

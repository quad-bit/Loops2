#ifndef RENDERER_PIPELINE_H_
#define RENDERER_PIPELINE_H_

#include <vector>
#include <renderGraph/Effect.h>
#include <RenderData.h>
#include <memory>

namespace Renderer
{
    namespace RenderGraph
    {
        /// <summary>
        /// Bake:
        /// 1. Validate
        /// 2. Traverse the dependency graph and create array of render passes
        /// 3. Render pass reordering (advanced)
        /// 4. Logical to physical resource assignment
        /// 5. Logical to physical render pass assignment
        /// 6. Build synchronisation
        /// 
        /// Execute:
        /// 1. Record the render passes into command buffers
        /// </summary>

        class Pipeline
        {
        protected:
            /// List of effects included in a pipeline
            std::vector<std::unique_ptr<Effect>> m_effects;

            virtual void ValidatePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>&) = 0;
            virtual void CreateLogicalPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>&) = 0;
            virtual void CreatePassResources(Renderer::RenderGraph::Graph<RenderGraphNodeBase>&) = 0;
            virtual void CreateRenderPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>&) = 0;
            virtual void CreateSynchronisations(Renderer::RenderGraph::Graph<RenderGraphNodeBase>&) = 0;

        public:
            virtual ~Pipeline() {}
            Pipeline() = default;

            /// <summary>
            /// Create all the effects
            /// </summary>
            virtual void CreatePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) = 0;

            /// <summary>
            /// Declare technique and effect dependencies, populate the graph
            /// </summary>
            virtual void CompilePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) = 0;
            virtual void DestroyPipeline() = 0;
            virtual void ExecutePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) = 0;
        };
    }
}

#endif //RENDERER_PIPELINE_H_


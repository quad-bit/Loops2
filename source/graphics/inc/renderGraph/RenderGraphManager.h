#ifndef RENDERER_RENDERGRAPHMANAGER_H_
#define RENDERER_RENDERGRAPHMANAGER_H_

#include <RenderData.h>
#include <renderGraph/Pipeline.h>
#include <memory>
#include <vector>
#include "Graph.h"
#include "renderGraph/Task.h"

namespace Renderer
{
    namespace RenderGraph
    {
        class RenderGraphManager
        {
        private:
            RenderGraphManager() = delete;
            const Core::Utility::RenderData& m_renderData;

            std::vector<std::unique_ptr<Renderer::RenderGraph::Pipeline>> m_pipelines;
            //std::vector<std::unique_ptr<Renderer::RenderGraph::Graph<RenderGraphNodeBase>>> m_graphs;
        public:
            explicit RenderGraphManager(const Core::Utility::RenderData& renderData);
            void AddPipeline(std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline);
            void Init();
            void DeInit();
            void Update(const uint32_t frameIndex);
            ~RenderGraphManager();
        };
    }
}

#endif // RENDERER_RENDERGRAPHMANAGER_H_
#include "RenderGraph/RenderGraphManager.h"

#define MAX_GRAPH_VERTICES 50

Renderer::RenderGraph::RenderGraphManager::RenderGraphManager(const Core::Utility::RenderData& renderData):
    m_renderData(renderData)
{

}

void Renderer::RenderGraph::RenderGraphManager::AddPipeline(std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline)
{
    m_pipelines.push_back(std::move(pipeline));
    std::unique_ptr<Renderer::RenderGraph::Graph<RenderGraphNodeBase>> graph = std::make_unique< Renderer::RenderGraph::Graph<RenderGraphNodeBase>>(MAX_GRAPH_VERTICES);
    m_graphs.push_back(std::move(graph));
}

void Renderer::RenderGraph::RenderGraphManager::Init()
{
    for (uint32_t i = 0; i < m_pipelines.size(); i++)
    {
        m_pipelines[i]->CreatePipeline(m_renderData, *m_graphs[i].get());
    }
}

void Renderer::RenderGraph::RenderGraphManager::DeInit()
{
}

void Renderer::RenderGraph::RenderGraphManager::Update(const uint32_t frameIndex)
{
}

Renderer::RenderGraph::RenderGraphManager::~RenderGraphManager()
{
}

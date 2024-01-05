#include "renderGraph/pipelines/DepthViewPipeline.h"
#include "resourceManagement/Resource.h"
#include "renderGraph/utility/Utils.h"
#include <string>
#include "renderGraph/effects/DepthPass.h"

//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::ValidatePipeline()
//{
//}
//
//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::CreateLogicalPasses()
//{
//}
//
//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::CreatePassResources()
//{
//}
//
//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::CreateRenderPasses()
//{
//}
//
//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::CreateSynchronisations()
//{
//}

Renderer::RenderGraph::Pipelines::DepthViewPipeline::DepthViewPipeline(
    Core::Utility::RenderData& renderData,
    const Core::WindowSettings& windowSettings, const std::string& name):
    Renderer::RenderGraph::Pipeline(renderData, windowSettings, name)
{
}

void Renderer::RenderGraph::Pipelines::DepthViewPipeline::CreatePipeline()
{
    std::unique_ptr<Renderer::RenderGraph::Effect> depthEffect =
        std::make_unique<Renderer::RenderGraph::Effects::DepthPass>(
            m_renderData,
            m_windowSettings,
            *m_graph.get(), "DepthPass", m_callbackUtility);
    m_effects.push_back(std::move(depthEffect));
    m_graph->PrintGraph();
}

//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::DestroyPipeline()
//{
//}

//void Renderer::RenderGraph::Pipelines::DepthViewPipeline::ExecutePipeline()
//{
//}

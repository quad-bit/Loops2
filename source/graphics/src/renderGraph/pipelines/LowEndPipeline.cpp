#include "renderGraph/pipelines/LowEndPipeline.h"
#include "renderGraph/tasks/RenderTask.h"
//#include "renderGraph/Graph.h"

namespace
{
    class Tech0 : public Renderer::RenderGraph::Technique
    {
    private:
        std::unique_ptr<Renderer::RenderGraph::RenderGraphNodeBase> r1, r2;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::RenderGraphNodeBase>* r1Node, *r2Node;
        std::unique_ptr<Renderer::RenderGraph::Resource> r1Image;

        std::unique_ptr<Renderer::RenderGraph::RenderGraphNodeBase> t1, t2;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::RenderGraphNodeBase>* t1Node, *t2Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1, task2;

    public:
        Tech0(Renderer::RenderGraph::Graph<Renderer::RenderGraph::RenderGraphNodeBase>& graph, const std::string& name) :
            Technique(graph, name)
        {
            // Create resource node
            r1Image = std::make_unique<Renderer::RenderGraph::ImageResource>(600, 600, "color1");
            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image.get(), "r1", Renderer::RenderGraph::ResourceType::IMAGE);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image.get(), "r2", Renderer::RenderGraph::ResourceType::IMAGE);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1");
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1));
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            task2 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task2");
            t2 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task2));
            t2Node = graph.Push(t2.get());
            m_taskNodes.push_back(t2Node);

            m_graph.AttachDirectedEdge(r1Node, t1Node);
            m_graph.AttachDirectedEdge(t1Node, r2Node);
            m_graph.AttachDirectedEdge(r2Node, t2Node);

            m_graph.PrintGraph();
        }
    };

    class Tech1 : public Renderer::RenderGraph::Technique
    {

    };

    class Tech3 : public Renderer::RenderGraph::Technique
    {

    };

    class effect0 : public Renderer::RenderGraph::Effect
    {
    public:
        effect0(Renderer::RenderGraph::Graph<Renderer::RenderGraph::RenderGraphNodeBase>& graph, const std::string& name) :
            Effect(graph, name)
        {
            std::unique_ptr<Renderer::RenderGraph::Technique> tech0 = std::make_unique<Tech0>(graph, "Tech0");
            m_techniques.push_back(std::move(tech0));
        }
    };

    class effect1 : public Renderer::RenderGraph::Effect
    {
    public:
        effect1(Renderer::RenderGraph::Graph<Renderer::RenderGraph::RenderGraphNodeBase>& graph, const std::string& name) :
            Effect(graph, name)
        {
            //std::unique_ptr<Renderer::RenderGraph::Technique> tech1 = std::make_unique<Tech1>(graph, "Tech1");
            //m_techniques.push_back(std::move(tech1));
        }
    };

    class effect2 : public Renderer::RenderGraph::Effect
    {

    };

    class effect3 : public Renderer::RenderGraph::Effect
    {

    };

    void AddEffects(Renderer::RenderGraph::Graph<Renderer::RenderGraph::RenderGraphNodeBase>& graph, std::vector<std::unique_ptr<Renderer::RenderGraph::Effect>>& effects)
    {
        std::unique_ptr<Renderer::RenderGraph::Effect> eff0 = std::make_unique<effect0>(graph, "effect0");
        effects.push_back(std::move(eff0));
    }
}

Renderer::RenderGraph::Pipelines::LowEndPipeline::LowEndPipeline()
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::ValidatePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateLogicalPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreatePassResources(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateRenderPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateSynchronisations(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CompilePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreatePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
    AddEffects(graph, m_effects);
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::DestroyPipeline()
{
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::ExecutePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
{
}

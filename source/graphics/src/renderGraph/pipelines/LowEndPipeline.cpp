#include "renderGraph/pipelines/LowEndPipeline.h"
#include "renderGraph/tasks/RenderTask.h"
#include "renderGraph/tasks/ComputeTask.h"
#include "renderGraph/tasks/TransferTask.h"
//#include "renderGraph/Graph.h"
#include "resourceManagement/Resource.h"
#include "renderGraph/utility/Utils.h"

namespace
{
    class Tech0 : public Renderer::RenderGraph::Technique
    {
    private:
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> r1, r2, r3, r4, r5, r6, r7, r8;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* r1Node, *r2Node,
            *r3Node, * r4Node, * r5Node, * r6Node, * r7Node, * r8Node;
        ResourceAlias* r1Image, *r2Image, *r3Image, *r4Image;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1, t2, t3, t4;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node, *t2Node, * t3Node, * t4Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1, task2, task3, task4;

    public:
        Tech0(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Technique(graph, name, funcs)
        {
            // Create resource node
            Core::Wrappers::ImageCreateInfo info{};
            info.m_width = 600;
            info.m_height = 600;

            r1Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color1_T0_E0");
            r2Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color2_T0_E0");
            r3Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color3_T0_E0");
            r4Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color4_T0_E0");

            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r1_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r2Image, "r2_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            r3 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r3_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r3Node = m_graph.Push(r3.get());
            m_resourceNodes.push_back(r3Node);

            r4 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r2Image, "r4_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r4Node = m_graph.Push(r4.get());
            m_resourceNodes.push_back(r4Node);

            r5 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r2Image, "r5_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r5Node = m_graph.Push(r5.get());
            m_resourceNodes.push_back(r5Node);

            r6 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r3Image, "r6_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r6Node = m_graph.Push(r6.get());
            m_resourceNodes.push_back(r6Node);

            r7 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r3Image, "r7_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r7Node = m_graph.Push(r7.get());
            m_resourceNodes.push_back(r7Node);

            r8 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r4Image, "r8_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r8Node = m_graph.Push(r8.get());
            m_resourceNodes.push_back(r8Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T0_E0");
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            task2 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task2_T0_E0");
            t2 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task2), m_callbackUtility.m_graphTraversalCallback);
            t2Node = graph.Push(t2.get());
            m_taskNodes.push_back(t2Node);

            task3 = std::make_unique<Renderer::RenderGraph::Tasks::ComputeTask>("task3_T0_E0");
            t3 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task3), m_callbackUtility.m_graphTraversalCallback);
            t3Node = graph.Push(t3.get());
            m_taskNodes.push_back(t3Node);

            task4 = std::make_unique<Renderer::RenderGraph::Tasks::TransferTask>("task4_T0_E0");
            t4 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task4), m_callbackUtility.m_graphTraversalCallback);
            t4Node = graph.Push(t4.get());
            m_taskNodes.push_back(t4Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r1Node, t1Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, r2Node, t1Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE);
            
            Renderer::RenderGraph::Utils::AddEdge(graph, t1Node, r3Node);
            Renderer::RenderGraph::Utils::AddEdge(graph, t1Node, r4Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r3Node, t2Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, r4Node, t2Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE);

            Renderer::RenderGraph::Utils::AddEdge(graph, t2Node, r5Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r5Node, t3Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, r6Node, t3Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY);

            Renderer::RenderGraph::Utils::AddEdge(graph, t3Node, r7Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r7Node, t4Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);

            Renderer::RenderGraph::Utils::AddEdge(graph, t4Node, r8Node);
        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>GetGraphOriginResourceNodes() override
        {
            return {r1Node, r2Node};
        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return {r8Node};
        }
    };

    class Tech1 : public Renderer::RenderGraph::Technique
    {
    private:
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> r1, r2, r3, r4;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* r1Node, * r2Node, *r3Node, * r4Node;
        ResourceAlias *r1Image, *r2Image;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1, t2, t3;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node, * t2Node, * t3Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1, task2, task3;

    public:
        Tech1(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Technique(graph, name, funcs)
        {
            // Create resource node
            Core::Wrappers::ImageCreateInfo info{};
            info.m_width = 600;
            info.m_height = 600;

            r1Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color1_T1_E1");
            r2Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color2_T1_E1");

            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r1_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r2_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            r3 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r3_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r3Node = m_graph.Push(r3.get());
            m_resourceNodes.push_back(r3Node);

            r4 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r2Image, "r4_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r4Node = m_graph.Push(r4.get());
            m_resourceNodes.push_back(r4Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T1_E1");
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            task2 = std::make_unique<Renderer::RenderGraph::Tasks::ComputeTask>("task2_T1_E1");
            t2 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task2), m_callbackUtility.m_graphTraversalCallback);
            t2Node = graph.Push(t2.get());
            m_taskNodes.push_back(t2Node);

            task3 = std::make_unique<Renderer::RenderGraph::Tasks::TransferTask>("task3_T1_E1");
            t3 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task3), m_callbackUtility.m_graphTraversalCallback);
            t3Node = graph.Push(t3.get());
            m_taskNodes.push_back(t3Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r1Node, t1Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, t1Node, r2Node);
            Renderer::RenderGraph::Utils::AddEdge(graph, r2Node, t2Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, t2Node, r3Node);
            Renderer::RenderGraph::Utils::AddEdge(graph, r3Node, t3Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
            Renderer::RenderGraph::Utils::AddEdge(graph, t3Node, r4Node);

        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>GetGraphOriginResourceNodes() override
        {
            return { r1Node };
        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return { r4Node };
        }
    };

    class Tech2 : public Renderer::RenderGraph::Technique
    {
    private:
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> r1, r2;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* r1Node, * r2Node;
        ResourceAlias* r1Image;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1;

    public:
        Tech2(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Technique(graph, name, funcs)
        {
            // Create resource node
            Core::Wrappers::ImageCreateInfo info{};
            info.m_width = 600;
            info.m_height = 600;

            r1Image = m_callbackUtility.m_resourceCreationCallback.CreateImageFunc(info, "color1");

            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r1_T2_E2", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r2_T2_E2", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T2_E2");
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r1Node, t1Node, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE);
            Renderer::RenderGraph::Utils::AddEdge(graph, t1Node, r2Node);
        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>GetGraphOriginResourceNodes() override
        {
            return { r1Node };
        }

        std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return { r2Node };
        }
    };

    class effect0 : public Renderer::RenderGraph::Effect
    {
    public:
        effect0(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Effect(graph, name, funcs)
        {
            std::unique_ptr<Renderer::RenderGraph::Technique> tech0 = std::make_unique<Tech0>(graph, "Tech0", m_callbackUtility);
            auto tech0_Output = tech0->GetGraphEndResourceNodes();
            m_techniques.push_back(std::move(tech0));

            //std::unique_ptr<Renderer::RenderGraph::Technique> tech1 = std::make_unique<Tech1>(graph, "Tech1", m_funcs);
            //auto tech1TaskNode = tech1->GetTaskNode("task2_TECH1");
            //m_techniques.push_back(std::move(tech1));

            //Renderer::RenderGraph::Utils::AddEdge(graph, tech0_Output[0], tech1TaskNode);
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
        {
            auto origins = m_techniques[0]->GetGraphOriginResourceNodes();
            //origins.insert (origins.end(), m_techniques[1]->GetGraphOriginResourceNodes().begin(), m_techniques[1]->GetGraphOriginResourceNodes().end());

            return origins;
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return m_techniques[0]->GetGraphOriginResourceNodes();
        }
    };

    class effect1 : public Renderer::RenderGraph::Effect
    {
    public:
        effect1(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Effect(graph, name, funcs)
        {
            std::unique_ptr<Renderer::RenderGraph::Technique> tech1 = std::make_unique<Tech1>(graph, "Tech1", m_callbackUtility);
            //auto tech1TaskNode = tech1->GetTaskNode("task2_TECH1");
            m_techniques.push_back(std::move(tech1));
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
        {
            return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>{};
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return std::vector{ m_techniques[0]->GetResourceNode("r3_t1_e1") };
        }
    };

    class effect2 : public Renderer::RenderGraph::Effect
    {
    public:
        effect2(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs) :
            Effect(graph, name, funcs)
        {
            std::unique_ptr<Renderer::RenderGraph::Technique> tech2 = std::make_unique<Tech2>(graph, "Tech2", m_callbackUtility);
            m_techniques.push_back(std::move(tech2));
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override
        {
            return std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>{};
        }

        virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override
        {
            return std::vector{ m_techniques[0]->GetResourceNode("r2_t2_e2") };
        }
    };

    class effect3 : public Renderer::RenderGraph::Effect
    {

    };

    void AddEffects(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, std::vector<std::unique_ptr<Renderer::RenderGraph::Effect>>& effects, Renderer::RenderGraph::Utils::CallbackUtility& funcs)
    {
        std::unique_ptr<Renderer::RenderGraph::Effect> eff0 = std::make_unique<effect0>(graph, "effect0", funcs);
        std::unique_ptr<Renderer::RenderGraph::Effect> eff1 = std::make_unique<effect1>(graph, "effect1", funcs);
        std::unique_ptr<Renderer::RenderGraph::Effect> eff2 = std::make_unique<effect2>(graph, "effect2", funcs);

        const std::vector<std::unique_ptr<Renderer::RenderGraph::Technique>>& eff0Techs = eff0->GetTechniques();
        auto r4_t0_e0 = eff0Techs[0]->GetResourceNode("r4_T0_E0");
        auto t4_t0_e0 = eff0Techs[0]->GetTaskNode("task4_T0_E0");

        const std::vector<std::unique_ptr<Renderer::RenderGraph::Technique>>& eff1Techs = eff1->GetTechniques();
        auto t3_t1_e1 = eff1Techs[0]->GetTaskNode("task3_T1_E1");
        auto r4_t1_e1 = eff1Techs[0]->GetResourceNode("r4_T1_E1");

        const std::vector<std::unique_ptr<Renderer::RenderGraph::Technique>>& eff2Techs = eff2->GetTechniques();
        auto r2_t2_e2 = eff2Techs[0]->GetResourceNode("r2_T2_E2");

        Renderer::RenderGraph::Utils::AddEdge(graph, r2_t2_e2, t4_t0_e0, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
        Renderer::RenderGraph::Utils::AddEdge(graph, r4_t1_e1, t4_t0_e0, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);
        Renderer::RenderGraph::Utils::AddEdge(graph, r4_t0_e0, t3_t1_e1, Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY);

        effects.push_back(std::move(eff0));
        effects.push_back(std::move(eff1));
        effects.push_back(std::move(eff2));
    }
}

Renderer::RenderGraph::Pipelines::LowEndPipeline::LowEndPipeline(Core::Utility::RenderData& renderData, const std::string& name) :
    Renderer::RenderGraph::Pipeline(renderData, name)
{
    AddEffects(*m_graph.get(), m_effects, m_callbackUtility);
    m_graph->PrintGraph();
}

//void Renderer::RenderGraph::Pipelines::LowEndPipeline::ValidatePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateLogicalPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreatePassResources(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateRenderPasses(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreateSynchronisations(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CompilePipeline(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreatePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::DestroyPipeline()
//{
//}
//
//void Renderer::RenderGraph::Pipelines::LowEndPipeline::ExecutePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph)
//{
//}

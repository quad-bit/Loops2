#include "renderGraph/pipelines/LowEndPipeline.h"
#include "renderGraph/tasks/RenderTask.h"
#include "renderGraph/tasks/ComputeTask.h"
#include "renderGraph/tasks/TransferTask.h"
#include "Settings.h"
#include "resourceManagement/Resource.h"
#include "renderGraph/utility/Utils.h"
#include "renderGraph/effects/Opaque.h"
#include "renderGraph/effects/SkyboxPass.h"
#include "renderGraph/effects/BoundPass.h"
#include "renderGraph/effects/LightCullPass.h"

uint32_t g_resourceDistributionCount = 0;
Core::Wrappers::ImageCreateInfo g_info{};
Core::Wrappers::Rect2D g_renderArea;

#if 0
namespace
{
    class Tech0 : public Renderer::RenderGraph::Technique
    {
    private:
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> r1, r2, r3, r4, r5, r6, r7, r8, r9;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* r1Node, *r2Node,
            *r3Node, * r4Node, * r5Node, * r6Node, * r7Node, * r8Node, *backBufferNode;
        std::vector<ResourceAlias*> r1Image, r2Image, r3Image, r4Image, backBufferImages;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1, t2, t3, t4;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node, *t2Node, * t3Node, * t4Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1, task2, task3, task4;

    public:
        Tech0(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
            const std::string& name,
            Renderer::RenderGraph::Utils::CallbackUtility& funcs,
            const std::string& effectName) :
            Technique(graph, name, funcs, effectName)
        {
            r1Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color1_T0_E0_0", "color1_T0_E0_1", "color1_T0_E0_2" }));
            r2Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color2_T0_E0_0", "color2_T0_E0_1", "color2_T0_E0_2" }));
            r3Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color3_T0_E0_0", "color3_T0_E0_1", "color3_T0_E0_2" }));
            r4Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color4_T0_E0_0", "color4_T0_E0_1", "color4_T0_E0_2" }));

            backBufferImages = m_callbackUtility.m_resourceCreationCallback.GetSwapchainImagesFunc();
            r9 = std::make_unique<Renderer::RenderGraph::ResourceNode>(backBufferImages, "backBuffer", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            backBufferNode = m_graph.Push(r9.get());
            m_resourceNodes.push_back(backBufferNode);

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

            r8 = std::make_unique<Renderer::RenderGraph::ResourceNode>(backBufferImages, "r8_T0_E0", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r8Node = m_graph.Push(r8.get());
            m_resourceNodes.push_back(r8Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T0_E0", g_renderArea, m_parentEffectName, m_name);
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            task2 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task2_T0_E0", g_renderArea, m_parentEffectName, m_name);
            t2 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task2), m_callbackUtility.m_graphTraversalCallback);
            t2Node = graph.Push(t2.get());
            m_taskNodes.push_back(t2Node);

            task3 = std::make_unique<Renderer::RenderGraph::Tasks::ComputeTask>("task3_T0_E0");
            t3 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task3), m_callbackUtility.m_graphTraversalCallback);
            t3Node = graph.Push(t3.get());
            m_taskNodes.push_back(t3Node);

            task4 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task4_T0_E0", g_renderArea, m_parentEffectName, m_name);
            t4 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task4), m_callbackUtility.m_graphTraversalCallback);
            t4Node = graph.Push(t4.get());
            m_taskNodes.push_back(t4Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r1Node, t1Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
                Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_UNDEFINED);

            Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, r2Node, t1Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
                Core::Enums::ImageLayout::LAYOUT_UNDEFINED, 0);
            
            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t1Node, r3Node);
            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t1Node, r4Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r3Node, t2Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
                Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, r4Node, t2Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
                Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                0);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t2Node, r5Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r5Node, t3Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
                Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            Renderer::RenderGraph::Utils::AddEdge(graph, r6Node, t3Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
                Core::Enums::ImageLayout::LAYOUT_GENERAL,
                Core::Enums::ImageLayout::LAYOUT_UNDEFINED);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t3Node, r7Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r7Node, t4Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
                Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_GENERAL);

            Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, backBufferNode, t4Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
                Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR, 0);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t4Node, r8Node);

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
        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> r1, r2, r3, r4, r5;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* r1Node, * r2Node, *r3Node, * r4Node, *r5Node;
        std::vector<ResourceAlias*> r1Image, r2Image, r3Image;
        std::vector<ResourceAlias*> r1Buffer;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1, t2, t3;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node, * t2Node, * t3Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1, task2, task3;

    public:
        Tech1(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs,
            const std::string& effectName) :
            Technique(graph, name, funcs, effectName)

        {
            // Create resource node
            r1Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color1_T1_E1_0", "color1_T1_E1_1", "color1_T1_E1_2" }));
            r2Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color2_T1_E1_0", "color2_T1_E1_1", "color2_T1_E1_2" }));
            r3Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color3_T1_E1_0", "color3_T1_E1_1", "color3_T1_E1_2" }));

            Core::Wrappers::BufferCreateInfo info{};
            info.size = 100;
            info.usage = { Core::Enums::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER_BIT, Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_SRC_BIT };
            info.m_name = "buffer_r5_T1_E1";
            r1Buffer = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameBufferFunc(info, std::vector<std::string>({ "buffer1_T1_E1_0", "buffer1_T1_E1_1", "buffer1_T1_E1_2" }));

            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r1_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r2_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            r3 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r3_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r3Node = m_graph.Push(r3.get());
            m_resourceNodes.push_back(r3Node);

            r4 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r4_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r4Node = m_graph.Push(r4.get());
            m_resourceNodes.push_back(r4Node);

            /*r5 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r2Image, "r5_T1_E1", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r5Node = m_graph.Push(r5.get());
            m_resourceNodes.push_back(r5Node);*/

            r5 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Buffer, "r5_T1_E1", Renderer::ResourceManagement::ResourceType::BUFFER, m_callbackUtility.m_graphTraversalCallback);
            r5Node = m_graph.Push(r5.get());
            m_resourceNodes.push_back(r5Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T1_E1", g_renderArea, m_parentEffectName, m_name);
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            task2 = std::make_unique<Renderer::RenderGraph::Tasks::TransferTask>("task2_T1_E1");
            t2 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task2), m_callbackUtility.m_graphTraversalCallback);
            t2Node = graph.Push(t2.get());
            m_taskNodes.push_back(t2Node);

            task3 = std::make_unique<Renderer::RenderGraph::Tasks::ComputeTask>("task3_T1_E1");
            t3 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task3), m_callbackUtility.m_graphTraversalCallback);
            t3Node = graph.Push(t3.get());
            m_taskNodes.push_back(t3Node);

            Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, r1Node, t1Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::WRITE_ONLY,
                Core::Enums::ImageLayout::LAYOUT_UNDEFINED, 0);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t1Node, r2Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r2Node, t2Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
                Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t2Node, r3Node);

            Renderer::RenderGraph::Utils::AddEdge(graph, r3Node, t3Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
                Core::Enums::ImageLayout::LAYOUT_GENERAL,
                Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL);

            /*Renderer::RenderGraph::Utils::AddEdge(graph, r5Node, t2Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
                Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL,
                Core::Enums::ImageLayout::LAYOUT_PREINITIALIZED);*/

            Renderer::RenderGraph::Utils::BufferResourceConnectionInfo bufInfo{};
            bufInfo.expectedUsage = Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_SRC_BIT;

            Renderer::RenderGraph::Utils::ConnectionInfo connection{};
            connection.m_bufInfo = bufInfo;
            connection.m_resource = r1Buffer;
            connection.m_resourceParentNodeId = r5Node->GetNodeId();
            connection.m_usage = Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY;
            Renderer::RenderGraph::Utils::AddEdge(graph, r5Node, t2Node,
                connection);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t3Node, r4Node);

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
        std::vector<ResourceAlias*> r1Image;

        std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> t1;
        Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* t1Node;
        std::unique_ptr<Renderer::RenderGraph::Task> task1;

    public:
        Tech2(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Renderer::RenderGraph::Utils::CallbackUtility& funcs,
            const std::string& effectName) :
            Technique(graph, name, funcs, effectName)
        {
            // Create resource node
            r1Image = m_callbackUtility.m_resourceCreationCallback.CreatePerFrameImageFunc(g_info, std::vector<std::string>({ "color1_T2_E2_0", "color1_T2_E2_1", "color1_T2_E2_2" }));

            r1 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r1_T2_E2", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r1Node = m_graph.Push(r1.get());
            m_resourceNodes.push_back(r1Node);

            r2 = std::make_unique<Renderer::RenderGraph::ResourceNode>(r1Image, "r2_T2_E2", Renderer::ResourceManagement::ResourceType::IMAGE, m_callbackUtility.m_graphTraversalCallback);
            r2Node = m_graph.Push(r2.get());
            m_resourceNodes.push_back(r2Node);

            // Create task node
            task1 = std::make_unique<Renderer::RenderGraph::Tasks::RenderTask>("task1_T2_E2", g_renderArea, m_parentEffectName, m_name);
            t1 = std::make_unique<Renderer::RenderGraph::TaskNode>(std::move(task1), m_callbackUtility.m_graphTraversalCallback);
            t1Node = graph.Push(t1.get());
            m_taskNodes.push_back(t1Node);

            Renderer::RenderGraph::Utils::AddInputAsColorAttachment(graph, r1Node, t1Node,
                Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_WRITE,
                Core::Enums::ImageLayout::LAYOUT_UNDEFINED, 0);

            Renderer::RenderGraph::Utils::AddTaskOutput(graph, t1Node, r2Node);
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
            std::unique_ptr<Renderer::RenderGraph::Technique> tech0 = std::make_unique<Tech0>(graph, "Tech0", m_callbackUtility, m_name);
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
            std::unique_ptr<Renderer::RenderGraph::Technique> tech1 = std::make_unique<Tech1>(graph, "Tech1", m_callbackUtility, m_name);
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
            std::unique_ptr<Renderer::RenderGraph::Technique> tech2 = std::make_unique<Tech2>(graph, "Tech2", m_callbackUtility, m_name);
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

        Renderer::RenderGraph::Utils::AddEdge(graph, r2_t2_e2, t4_t0_e0,
            Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
            Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        Renderer::RenderGraph::Utils::AddEdge(graph, r4_t1_e1, t4_t0_e0,
            Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
            Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            Core::Enums::ImageLayout::LAYOUT_GENERAL);

        Renderer::RenderGraph::Utils::AddEdge(graph, r4_t0_e0, t3_t1_e1,
            Renderer::RenderGraph::Utils::ResourceMemoryUsage::READ_ONLY,
            Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        effects.push_back(std::move(eff0));
        effects.push_back(std::move(eff1));
        effects.push_back(std::move(eff2));
    }

    void CreateTestPipeline(Renderer::RenderGraph::Utils::CallbackUtility& callbackUtility,
        Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* graph,
        std::vector<std::unique_ptr<Renderer::RenderGraph::Effect>>& effects)
    {
        g_resourceDistributionCount = Core::Settings::m_swapBufferCount;

        Core::Enums::Format format{ Core::Enums::Format::B8G8R8A8_UNORM };
        Core::Enums::ImageType type{ Core::Enums::ImageType::IMAGE_TYPE_2D };
        std::vector<Core::Enums::ImageUsage> usages{ Core::Enums::ImageUsage::USAGE_SAMPLED_BIT, Core::Enums::ImageUsage::USAGE_COLOR_ATTACHMENT_BIT,
        Core::Enums::ImageUsage::USAGE_TRANSFER_SRC_BIT , Core::Enums::ImageUsage::USAGE_TRANSFER_DST_BIT };

        g_info.m_colorSpace = Core::Enums::ColorSpace::COLOR_SPACE_SRGB_NONLINEAR_KHR;
        g_info.m_depth = 1;
        g_info.m_format = format;
        g_info.m_height = 1024;
        g_info.m_imageType = type;
        g_info.m_initialLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
        g_info.m_layers = 1;
        g_info.m_mips = 1;
        g_info.m_sampleCount = Renderer::RendererSettings::GetMaxSampleCountAvailable();
        g_info.m_usages = usages;
        g_info.m_viewType = Core::Enums::ImageViewType::IMAGE_VIEW_TYPE_2D;
        g_info.m_width = 1024;

        g_renderArea.lengthX = 1024;
        g_renderArea.lengthY = 1024;
        g_renderArea.offsetX = 0;
        g_renderArea.offsetY = 0;

        AddEffects(*graph, effects, callbackUtility);
        graph->PrintGraph();
    }
}
#endif

Renderer::RenderGraph::Pipelines::LowEndPipeline::LowEndPipeline(Core::Utility::RenderData& renderData, const Core::WindowSettings& windowSettings, const std::string& name) :
    Renderer::RenderGraph::Pipeline(renderData, windowSettings, name)
{
    //CreateTestPipeline(m_callbackUtility, m_graph.get(), m_effects);
}

void Renderer::RenderGraph::Pipelines::LowEndPipeline::CreatePipeline()
{
    std::unique_ptr<Renderer::RenderGraph::Effect> lightCullPass =
        std::make_unique<Renderer::RenderGraph::Effects::LightCullPass>(
            m_renderData,
            m_windowSettings,
            *m_graph.get(), "LightCullPass", m_callbackUtility);

    auto& lightCullOutputNodes = lightCullPass->GetGraphEndResourceNodes();

    std::unique_ptr<Renderer::RenderGraph::Effect> opaquePass =
        std::make_unique<Renderer::RenderGraph::Effects::OpaquePass>(
            m_renderData,
            m_windowSettings,
            *m_graph.get(), "OpaquePass", m_callbackUtility, lightCullOutputNodes);

    auto& opaqueOutputNodes = opaquePass->GetGraphEndResourceNodes();

    std::unique_ptr<Renderer::RenderGraph::Effect> boundPass =
        std::make_unique<Renderer::RenderGraph::Effects::BoundPass>(
            m_renderData,
            m_windowSettings,
            *m_graph.get(), "BoundPass", m_callbackUtility,
            opaqueOutputNodes);

    auto& boundOutputNodes = boundPass->GetGraphEndResourceNodes();

    std::unique_ptr<Renderer::RenderGraph::Effect> skyboxPass =
        std::make_unique<Renderer::RenderGraph::Effects::SkyboxPass>(
            m_renderData,
            m_windowSettings,
            *m_graph.get(), "SkyboxPass", m_callbackUtility,
            boundOutputNodes);

    m_effects.push_back(std::move(opaquePass));
    m_effects.push_back(std::move(boundPass));
    m_effects.push_back(std::move(lightCullPass));
    m_effects.push_back(std::move(skyboxPass));

    m_graph->PrintGraph();
}

#ifndef RENDERER_TECHNIQUE_H_
#define RENDERER_TECHNIQUE_H_

#include <string>
#include <vector>
#include <renderGraph/Task.h>
#include <renderGraph/Graph.h>
#include <renderGraph/tasks/RenderTask.h>
#include <RenderData.h>

namespace Renderer
{
    namespace RenderGraph
    {
        /// <summary>
        /// A method to implement a given effect. Based on LOD opaque can be implemented with different 
        /// techniques.
        /// </summary>
        class Technique
        {
        protected:
            Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& m_graph;
            const std::string m_name;
            //std::vector<std::unique_ptr<Task>> m_tasks;
            std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> m_taskNodes, m_resourceNodes;
            Utils::CallbackUtility& m_callbackUtility;
            std::string m_parentEffectName;

            virtual void CreateResources(){}

            //void CreateVulkanPipeline()
            //{
            //    for (auto& taskNode : m_taskNodes)
            //    {
            //        auto task = static_cast<Renderer::RenderGraph::TaskNode*>(taskNode->GetNodeData())->GetTask();

            //        if (task->GetTaskType() == Renderer::RenderGraph::TaskType::RENDER_TASK)
            //        {
            //            // Three lying in vkShaderResourceManager
            //            // Vertex input Create info
            //            auto vertexInputId = VulkanInterfaceAlias::GetVertexInputStateId(m_parentEffectName,
            //                m_name, task->GetTaskName());
            //            // Shader module info
            //            auto shaderInputId = VulkanInterfaceAlias::GetShaderStateId(m_parentEffectName,
            //            m_name, task->GetTaskName());
            //            // pipelineLayout
            //            auto pipelineLayoutId = VulkanInterfaceAlias::GetPipelineLayoutId(m_parentEffectName,
            //                m_name, task->GetTaskName());

            //            // The rest need to be configured

            //        }
            //        else if (task->GetTaskType() == Renderer::RenderGraph::TaskType::COMPUTE_TASK)
            //        {

            //        }
            //    }
            //}

        public:
            typedef Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase> graphNodeAlias;

            //virtual const uint32_t& GetId() = 0;
            //virtual const std::vector<Task>& GetParts() = 0;
            //virtual void GetLODRange(float& minLOD, float& maxLOD) const = 0;
            //virtual void SetLOD(const float& minLOD, const float& maxLOD) = 0;

            /// Checks if this technique is supported on the current hardware.
            //virtual bool IsSupported() = 0;

            Technique(
                Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                const std::string& name,
                Utils::CallbackUtility& funcs,
                const std::string& effectName) :
                m_graph(graph), m_name(name), m_callbackUtility(funcs), m_parentEffectName(effectName)
            {}

            virtual ~Technique()
            {

            }

            const std::vector<graphNodeAlias*>& GetTaskNodeList()
            {
                return m_taskNodes;
            }

            const std::vector<graphNodeAlias*>& GetResourceNodeList()
            {
                return m_resourceNodes;
            }

            graphNodeAlias* GetTaskNode(const std::string& taskName)
            {
                auto it = std::find_if(m_taskNodes.begin(), m_taskNodes.end(), [&](Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node)
                {
                    auto taskNode = (TaskNode*)node->GetNodeData();
                    auto name = taskNode->GetTask()->GetTaskName();
                    return name == taskName;
                });

                if (it == m_taskNodes.end())
                {
                    ASSERT_MSG_DEBUG(0, "Task not found");
                }
                return *it;
            }

            graphNodeAlias* GetResourceNode(const std::string& resourceName)
            {
                auto it = std::find_if(m_resourceNodes.begin(), m_resourceNodes.end(), [&](Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* node)
                {
                    auto resourcesNode = (ResourceNode*)node->GetNodeData();
                    auto name = resourcesNode->GetNodeName();
                    return name == resourceName;
                });

                if (it == m_resourceNodes.end())
                {
                    ASSERT_MSG_DEBUG(0, "Task not found");
                }
                return *it;
            }

            // technique origin and ends nodes
            virtual std::vector<graphNodeAlias*> GetGraphOriginResourceNodes() = 0;
            virtual std::vector<graphNodeAlias*> GetGraphEndResourceNodes() = 0;
            
            virtual void SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
            {

            }
        };

        struct SetInfo
        {
            Core::Enums::ResourceSets m_setValue;
            std::optional<uint32_t> m_descriptorSetId;
            std::vector<SetInfo*> m_childrenSet;
            uint32_t* m_childIndicies = nullptr;
            uint32_t m_indiciesCount = 0;
        };

        void CreateDrawInfo(const Core::Utility::TransformData& data,
            const std::string& effectName, const std::string& techName, const std::string& taskName,
            RenderGraph::Tasks::DrawInfo& drawInfo);

        void CreateSetInfo(std::map<Core::Enums::ResourceSets,
            std::map<uint32_t, SetInfo>>& setInfoMap, RenderGraph::Tasks::DrawInfo& drawInfo);

        struct FilterInfo
        {
            void* m_data;
            uint32_t m_dataCount;
            uint32_t m_stride;
            Core::Enums::ResourceSets m_setType;
            void* m_next = nullptr;
        };


        /*void Filter(void* data, uint32_t count, uint32_t stride,
            const Core::Enums::ResourceSets& setType,
            const std::function<bool(uint32_t index, void* data, const Core::Enums::ResourceSets& setType)>& filterFunc,
            std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>>& filteredDataList,
            std::map<Core::Enums::ResourceSets, std::map<uint32_t, SetInfo>>& setMap);*/

        void Filter(const FilterInfo& info,
            const std::function<bool(uint32_t index, void* data, void* next)>& filterFunc,
            std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>>& filteredDataList,
            std::map<Core::Enums::ResourceSets, std::map<uint32_t, SetInfo>>& setMap);

        bool CameraFilter(uint32_t index, void* data, void* next);
        bool MaterialFilter(uint32_t index, void* data, void* next);
    }
}

#endif //RENDERER_TECHNIQUE_H_
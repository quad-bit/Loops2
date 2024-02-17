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
            std::string GetNodeName(const std::string& name);

            Core::Utility::TechniqueId m_techniqueId;
            Core::Utility::EffectInfo m_effectInfo;

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
                const std::string& effectName,
                const Core::Utility::EffectInfo& effectInfo) :
                m_graph(graph), m_name(name), 
                m_callbackUtility(funcs), m_parentEffectName(effectName),
                m_effectInfo(effectInfo)
            {
                auto effectId = VulkanInterfaceAlias::GetEffectId(effectName);
                m_techniqueId = VulkanInterfaceAlias::GetTechniqueId(effectId, m_name);
            }

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
                    return name.find(taskName) != std::string::npos;
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
                    auto& name = resourcesNode->GetNodeName();
                    return name == resourceName;
                });

                if (it == m_resourceNodes.end())
                {
                    ASSERT_MSG_DEBUG(0, "Task not found");
                }
                return *it;
            }

            const std::vector<ResourceAlias*>& GetResource(const graphNodeAlias* resourceNode) const
            {
                //ASSERT_MSG_DEBUG( resourceNode->node->GetNodeType() != Renderer::RenderGraph::
                auto& resource = ((ResourceNode*)resourceNode->node)->GetResource();
                return resource;
            }

            // technique origin and ends nodes
            virtual std::vector<graphNodeAlias*> GetGraphOriginResourceNodes() = 0;
            virtual std::vector<graphNodeAlias*> GetGraphEndResourceNodes() = 0;

            virtual void SetupFrame(const Core::Wrappers::FrameInfo& frameInfo)
            {}
        };

        struct GraphNodeWrapper
        {
            std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> m_nodeBase;
            Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* m_graphNode;
        };

        struct SetInfo
        {
            Core::Enums::ResourceSets m_setValue;
            std::optional<uint32_t> m_descriptorSetId;
            std::vector<SetInfo*> m_childrenSet;
            uint32_t* m_childIndicies = nullptr;
            uint32_t m_indiciesCount = 0;
        };

        void CreateDrawInfo(const Core::Utility::TransformData& data, uint32_t taskId,
            /*const std::string& effectName, const std::string& techName, const std::string& taskName,*/
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

        // get all the renderData member objects that are meant for the specific technique
        void Filter(const FilterInfo& info,
            const std::function<bool(uint32_t index, void* data, void* next)>& filterFunc,
            std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>>& filteredDataList,
            std::map<Core::Enums::ResourceSets, std::map<uint32_t, SetInfo>>& setMap);

        bool CameraFilter(uint32_t index, void* data, void* next);
        bool MaterialFilter(uint32_t index, void* data, void* next);

        void CreateResourceNode(
            const std::string& nodeName, const std::vector<ResourceAlias*>& imageList,
            std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& resourceNodes,
            Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
            Renderer::RenderGraph::Utils::GraphTraversalCallback& graphTraversal,
            GraphNodeWrapper& resourceNode
        );

        void CreateTaskGraphNode(
            const std::string& taskName,
            const std::string& parentEffectName,
            const std::string& techName,
            const Core::Wrappers::Rect2D& renderArea,
            Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
            Renderer::RenderGraph::Utils::GraphTraversalCallback& graphTraversal,
            std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*>& taskNodes,
            GraphNodeWrapper& taskNode
        );



        // Use the childIndicies of heigher level set to pick the lower level sets for a set list
        template < typename ParentType, typename ChildType, Core::Enums::ResourceSets setType>
        void Pick(std::vector<std::pair<uint32_t, void*>>& parentList,
            std::map<Core::Enums::ResourceSets, std::vector<std::pair<uint32_t, void*>>>& filteredDataList,
            std::map<Core::Enums::ResourceSets, std::map<uint32_t, Renderer::RenderGraph::SetInfo>>& setInfoMap,
            void* childData, uint32_t dataCount, uint32_t stride,
            std::optional<std::function<void(void*, void*)>> additionalFunc, void* funcData
        )
        {
            // as child indicies are available for transform in mat and there are no filtering params 
            // in transform at this moment
            // Parent is MATERIAL
            for (auto& data : parentList)
            {
                auto parentData = static_cast<ParentType*>(data.second);
                auto& childIndicies = parentData->m_childSetIndicies;

                ASSERT_MSG_DEBUG(childIndicies.size() != 0, "Needs childIndicies");

                void* dataPtr = childData;
                for (auto index : childIndicies)
                {
                    dataPtr = static_cast<char*>(childData) + stride * index;

                    auto childData = static_cast<ChildType*>(dataPtr);

                    auto pair = std::pair(index, dataPtr);
                    //auto pair = std::pair(index, (void*)&m_renderData.m_transformData[index]);
                    if (filteredDataList.find(setType) == filteredDataList.end())
                    {
                        filteredDataList.insert({ setType, {{pair}} });
                    }
                    else
                    {
                        filteredDataList[setType].push_back(pair);
                    }

                    if (additionalFunc.has_value())
                    {
                        auto& func = additionalFunc.value();
                        func(childData, funcData);
                    }

                    Renderer::RenderGraph::SetInfo setInfo{};
                    setInfo.m_setValue = setType;
                    setInfo.m_descriptorSetId = static_cast<ChildType*>(dataPtr)->m_descriptorSetId;

                    if (setInfoMap.find(setType) == setInfoMap.end())
                    {
                        setInfoMap.insert({ setType, { { index, setInfo } } });
                    }
                    else
                    {
                        setInfoMap[setType].insert({ index, setInfo });
                    }
                }
            }
        }
    }
}

#endif //RENDERER_TECHNIQUE_H_
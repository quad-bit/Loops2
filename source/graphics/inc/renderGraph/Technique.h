#ifndef RENDERER_TECHNIQUE_H_
#define RENDERER_TECHNIQUE_H_

#include <string>
#include <vector>
#include <renderGraph/Task.h>
#include <renderGraph/Graph.h>

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
            Renderer::RenderGraph::Graph<RenderGraphNodeBase>& m_graph;
            const std::string m_name;
            std::vector<std::unique_ptr<Task>> m_tasks;
            std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::RenderGraphNodeBase>*> m_taskNodes, m_resourceNodes;

        public:
            //virtual const uint32_t& GetId() = 0;
            //virtual const std::vector<Task>& GetParts() = 0;
            //virtual void GetLODRange(float& minLOD, float& maxLOD) const = 0;
            //virtual void SetLOD(const float& minLOD, const float& maxLOD) = 0;
            
            /// Checks if this technique is supported on the current hardware.
            //virtual bool IsSupported() = 0;
            
            Technique(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph, const std::string& name) :
                m_graph(graph), m_name(name)
            {}
            
            const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::RenderGraphNodeBase>*>& GetTaskNodeList()
            {
                return m_taskNodes;
            }

            const std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::RenderGraphNodeBase>*>& GetResourceNodeList()
            {
                return m_resourceNodes;
            }
        };
    }
}

#endif //RENDERER_TECHNIQUE_H_
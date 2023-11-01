#ifndef RENDERER_EFFECT_H_
#define RENDERER_EFFECT_H_

#include <vector>
#include "Technique.h"
//#include "renderGraph/utility/Utils.h"

namespace Renderer
{
    namespace RenderGraph
    {
        /// <summary>
        /// Any effect such as bloom, opaque pass, transparent pass
        /// </summary>
        class Effect
        {
        protected:
            std::vector<std::unique_ptr<Technique>> m_techniques;
            Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& m_graph;
            const std::string m_name;
            Utils::CallbackUtility& m_callbackUtility;

        public:
            virtual ~Effect()
            {
                for (auto& technique : m_techniques)
                    technique.reset();
                m_techniques.clear();
            }
            //virtual const uint32_t& GetId() = 0;
            //virtual const std::vector<Technique> & GetTechniques() = 0;

            /// Returns an Technique which is supported on the current hardware
            /// and is suitable for the given LOD. Usually the LOD value is the
            /// distance from the camera position to the center of the bounding
            /// volume of the rendered geometry. If more than one technique is
            /// available for a given LOD the algorithm tries to detect which
            /// technique will be used next.
            //virtual Technique GetTechnique(const float effectLOD) = 0;
            //virtual Technique GetTechnique(const std::string& techniqueName) = 0;

            Effect(Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph, const std::string& name, Utils::CallbackUtility& funcs) :
                m_graph(graph), m_name(name), m_callbackUtility(funcs) {}

            virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() = 0;
            virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() = 0;

            const std::vector<std::unique_ptr<Technique>>& GetTechniques()
            {
                return m_techniques;
            }
        };
    }
}

#endif //RENDERER_EFFECT_H_
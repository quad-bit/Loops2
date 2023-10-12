#ifndef RENDERER_EFFECT_H_
#define RENDERER_EFFECT_H_

#include <vector>
#include "Technique.h"

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
            Renderer::RenderGraph::Graph<RenderGraphNodeBase>& m_graph;
            const std::string m_name;

        public:
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

            Effect(Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph, const std::string& name) :
                m_graph(graph), m_name(name) {}
        };
    }
}

#endif //RENDERER_EFFECT_H_
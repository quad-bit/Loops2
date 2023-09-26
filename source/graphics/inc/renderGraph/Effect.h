#ifndef RENDERER_EFFECT_H_
#define RENDERER_EFFECT_H_

#include "Technique.h"

namespace Renderer
{
    namespace RenderGraph
    {
        class Effect
        {
        public:
            /// Returns the identifier of this effect.
            /// @return The identifier of this effect.
            virtual const uint32_t& GetId() = 0;
            
            /// Returns the list of all techniques for this effect.
            /// @return The list of techniques.
            virtual const TechniqueList& GetTechniques() = 0;
            
            /// Returns an Technique which is supported on the current hardware
            /// and is suitable for the given LOD. Usually the LOD value is the
            /// distance from the camera position to the center of the bounding
            /// volume of the rendered geometry. If more than one technique is
            /// available for a given LOD the algorithm tries to detect which
            /// technique will be used next.
            /// @param effectLOD The LOD of the technique wanted. Use a negative number
            /// for the most detailed technique.
            /// @return The Technique, or NULL if no technique is
            /// supported on the current hardware.
            virtual Technique GetTechnique(const float effectLOD) = 0;
        };
    }
}

#endif //RENDERER_EFFECT_H_
#ifndef RENDERER_TECHNIQUE_H_
#define RENDERER_TECHNIQUE_H_

#include <string>

namespace Renderer
{
    namespace RenderGraph
    {
        class Technique
        {
        public:
            /// Returns the identifier of this technique.
            /// @return The identifier of this technique.
            virtual const uint32_t& GetId() = 0;
            /// Returns the list of parts this technique consists of.
            /// @return The list of technique parts.
            virtual const TechniqueNodeList& GetParts() = 0;
            /// Returns the LOD range for this technique.
            /// @param minLOD The minimum LOD value for this technique.
            /// @param maxLOD The maximum LOD value for this technique.
            virtual void GetLODRange(float& minLOD, float& maxLOD) const = 0;
            /// Sets the LOD range for this technique.
            /// @param minLOD The minimum LOD value for this technique.
            /// @param maxLOD The maximum LOD value for this technique.
            virtual void SetLOD(const float& minLOD, const float& maxLOD) = 0;
            /// Checks if this technique is supported on the current hardware.
            /// @return True if it is supported, False otherwise.
            virtual bool IsSupported() = 0;
            /// Returns the list of inputs of all parts of this technique.
            /// @return The list of inputs.
            virtual const InputList& GetInputs() = 0;
            /// Returns an input with the specified name or NULL if no input with
            /// that name was found. If more than one input with that name exists,
            /// only the first one will be returned.
            /// @param name The name of the input.
            /// @return The input.
            virtual Input GetInput(const std::string& name) = 0;
            /// Sets the specified value to ALL inputs with the given name.
            /// @param name The name of the input.
            /// @param value The value to set.
            /// @return True if any input with that name was found, False otherwise.
            virtual bool SetInputValue(const std::string& name, const std::any& value) = 0;
        };
    }
}

#endif //RENDERER_TECHNIQUE_H_
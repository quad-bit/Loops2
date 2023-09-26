#ifndef RENDERER_NODE_H_
#define RENDERER_NODE_H_

namespace Renderer
{
    namespace RenderGraph
    {
        class NodeInput
        {

        };

        class NodeOutput
        {

        };

        class Node
        {
        public:
            /// Returns the identifier of this technique part.
            /// @return The identifier of this technique part.
            virtual const uint32_t& GetId() = 0;
            /// Returns the list of inputs for this technique part.
            /// @return The list of inputs.
            virtual const InputList& GetInputs() = 0;
            /// Returns the list of outputs for this technique part.
            /// @return The list of outputs.
            virtual const OutputList& GetOutputs() = 0;
            /// Returns the group index of this technique part. Group indices can be
            /// used to put different technique parts into different rendering passes.
            /// The default group index is 0.
            /// @return The group index of this technique part.
            virtual const uint32& GetGroupIndex() const = 0;
            /// Returns the list of state dependencies for this technique part.
            /// If the state dependencies are not fulfilled, this
            /// technique part is not ready for rendering.
            /// @return The list of state dependencies.
            virtual const StateDependencyList& GetStateDependencies() = 0;
            /// Returns the list of state changes this technique part will
            /// perform after rendering. Other technique parts may be
            /// dependend on this state changes.
            /// @return The list of state changes.
            virtual const StateChangeList& GetStateChanges() = 0;
            /// Returns if this technique part is equal to the given technique part.
            /// @param part The technique part to check with this technique part.
            /// @return True if the technique parts are equal, False otherwise.
            virtual bool IsEqual(const ITechniquePartPtr& part) = 0;
            /// Returns a version of this technique part which matches the given filter.
            /// @return The version of this technique part which matches the filter or
            /// NULL if this technique part should has no matching version.
            virtual TechniquePartPtr GetFilteredVersion(const Filter& filter) = 0;
            /// Returns the technique this part belongs to.
            /// @return The technique this part belongs to.
            virtual const TechniquePtr& GetTechnique() const = 0;
        };
    }
}

#endif //RENDERER_NODE_H_
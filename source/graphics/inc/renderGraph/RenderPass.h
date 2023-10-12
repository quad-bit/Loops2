#ifndef RENDERER_RENDERPASS_H_
#define RENDERER_RENDERPASS_H_

namespace Renderer
{
    namespace RenderGraph
    {
        /*
        class RenderPass
        {
            /// Returns the identifier of this pass.
            /// @return The identifier of this pass.
            virtual const Yare::Core::Engine::Identifier& GetIdentifier() = 0;
            /// Returns the list of inputs for this pass. This will also include
            /// inputs of assigned technique parts.
            /// @return The list of inputs.
            virtual const IInputList& GetInputs() = 0;
            /// Returns the list of outputs for this pass. This will also include
            /// outputs of assigned technique parts.
            /// @return The list of outputs.
            virtual const IOutputList& GetOutputs() = 0;
            /// Returns the list of state dependencies for this pass.
            /// If this state dependencies are not fulfilled,
            /// this pass is not ready for rendering.
            /// @return The list of state dependencies.
            virtual const StateDependencyList& GetStateDependencies() = 0;
            /// Returns the list of state changes this pass will perform on
            /// rendering. Other passes may be dependend on this state changes.
            /// @return The list of state changes.
            virtual const StateChangeList& GetStateChanges() = 0;
            /// Renders the provided geometries with this pass.
            /// @param variables Variables which can contain e.g. renderstates
            /// or values for inputs.
            /// @param geometry A list of geometries which
            /// should be rendered with this pass.
            virtual void Render(const std::vector<
                std::vector<Yare::Graphics::Variables::IVariable*> >& variables,
                const std::vector<Yare::Graphics::Rendering::IGeometry*>& geometry) = 0;
            /// Returns if this pass is equal to the given pass.
            /// @param pass The pass to check with this pass.
            /// @return True if the passes are equal, False otherwise.
            virtual bool IsEqual(const IPassPtr& pass) = 0;
            /// Creates a copy of this pass.
            /// @return The created clone of this pass.
            virtual IPassPtr Clone() = 0;
        };
        */
    }
}
#endif//RENDERER_RENDERPASS_H_
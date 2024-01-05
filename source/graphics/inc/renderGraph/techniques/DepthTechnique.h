#ifndef RENDERER_DEPTHTECHNIQUE_H_
#define RENDERER_DEPTHTECHNIQUE_H_

#include <renderGraph/Technique.h>
#include <RenderData.h>
#include <Settings.h>
#include "renderGraph/Task.h"

namespace Renderer
{
    namespace RenderGraph
    {
        namespace Techniques
        {
            class DepthTechnique : public Technique
            {
            private:
                const Core::Utility::RenderData& m_renderData;
                void CreateResources() override;

                std::vector<ResourceAlias*> m_depthAttachments, m_backBufferImages;
                std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> depthInputRes, depthOutputRes,
                    transferInput, transferInput2, transferOutput;
                Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* depthResInputNode, *depthResOutputNode,
                    *transferInputNode, *transferInputNode2, *transferOutputNode;

                std::unique_ptr<Renderer::RenderGraph::Utils::RenderGraphNodeBase> taskNodeBase, transferNodeBase;
                Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>* taskNode, *transferTaskNode;
                std::unique_ptr<Renderer::RenderGraph::Task> task, transferTask;
                uint32_t m_renderHeight, m_renderWidth;

            public:
                DepthTechnique(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
                    const std::string& name,
                    const std::string& effectName);

                ~DepthTechnique();

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override;
                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override;

            };
        }
    }
}

#endif //RENDERER_DEPTHTECHNIQUE_H_

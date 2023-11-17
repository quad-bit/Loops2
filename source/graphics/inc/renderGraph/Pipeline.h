#ifndef RENDERER_PIPELINE_H_
#define RENDERER_PIPELINE_H_

#include <vector>
#include <renderGraph/Effect.h>
#include <RenderData.h>
#include <memory>
#include <Utility/RenderingWrappers/RenderingWrapper.h>

#define MAX_GRAPH_VERTICES 50

namespace Renderer
{
    namespace RenderGraph
    {
        /// <summary>
        /// Bake:
        /// 1. Validate
        /// 2. Traverse the dependency graph and create array of render passes
        /// 3. Render pass reordering (advanced)
        /// 4. Logical to physical resource assignment
        /// 5. Logical to physical render pass assignment
        /// 6. Build synchronisation
        /// 
        /// Execute:
        /// 1. Record the render passes into command buffers
        /// </summary>

        struct PathsPerOrigin
        {
            Renderer::RenderGraph::Utils::RenderGraphNodeBase* m_origin = nullptr;
            std::vector<std::vector<Renderer::RenderGraph::Utils::RenderGraphNodeBase*>> m_paths;
        };

        struct PerLevelTaskInfo
        {
            uint32_t m_level;
            std::vector<Renderer::RenderGraph::Utils::RenderGraphNodeBase*> m_taskList;
            //std::vector<std::pair<uint32_t, uint32_t>> m_localDependency;
            std::vector<std::pair<Renderer::RenderGraph::Utils::RenderGraphNodeBase*, Renderer::RenderGraph::Utils::RenderGraphNodeBase*>> m_dependency;

        };

        class Pipeline
        {
        private:
            ResourceAlias* CreateImage(const Core::Wrappers::ImageCreateInfo& imageCreateInfo, const std::string& name)
            {
                std::unique_ptr<ResourceAlias> obj = std::make_unique<ImageResourceAlias>(
                    imageCreateInfo.m_width,
                    imageCreateInfo.m_height,
                    name);
                m_imageList.push_back(std::move(obj));
                return m_imageList[m_imageList.size() - 1].get();
            }

            ResourceAlias* CreateBuffer(const Core::Wrappers::BufferCreateInfo& bufferCreateInfo, const std::string& name)
            {
                std::unique_ptr<ResourceAlias> obj = std::make_unique<BufferResourceAlias>(bufferCreateInfo.size, name);
                m_bufferList.push_back(std::move(obj));
                return m_bufferList[m_bufferList.size() - 1].get();
            }

        protected:
            /// List of effects included in a pipeline
            std::vector<std::unique_ptr<Effect>> m_effects;
            std::unique_ptr<Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>> m_graph;

            Core::Utility::RenderData& m_renderData;
            const std::string m_name;
            std::vector<std::unique_ptr<ResourceAlias>> m_imageList, m_bufferList;

            Utils::ResourceCreationCallback m_resourseCreationCallback;
            Utils::GraphTraversalCallback m_graphTraversalCallback;
            Utils::CallbackUtility m_callbackUtility;

            std::vector<PathsPerOrigin> m_pathPerOrigin;

            uint32_t m_passLevelCount = 0;
            std::map<uint32_t, PerLevelTaskInfo> m_perLevelTaskInfo;

            Renderer::RenderGraph::Utils::RenderGraphNodeBase* m_currentOrigin;
            void PopulatePath(Renderer::RenderGraph::Utils::RenderGraphNodeBase* node);

        public:
            virtual ~Pipeline()
            {
                m_graph.reset();

                for (auto& effect : m_effects)
                    effect.reset();
                m_effects.clear();

                for (auto& buffer : m_bufferList)
                    buffer.reset();
                m_bufferList.clear();

                for (auto& image : m_imageList)
                    image.reset();
                m_imageList.clear();
            }

            /// <summary>
            /// Declare technique and effect dependencies, populate the graph
            /// </summary>
            Pipeline(Core::Utility::RenderData& renderData, const std::string& name) :
                m_renderData(renderData),
                m_name(name),
                m_graph(std::make_unique< Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>>(MAX_GRAPH_VERTICES))
            {
                m_resourseCreationCallback.CreateImageFunc = std::bind(&Pipeline::CreateImage, this, std::placeholders::_1, std::placeholders::_2);
                m_resourseCreationCallback.CreateBufferFunc = std::bind(&Pipeline::CreateBuffer, this, std::placeholders::_1, std::placeholders::_2);

                m_graphTraversalCallback.PipelineCompileCallback = std::bind(&Pipeline::PopulatePath, this, std::placeholders::_1);

                m_callbackUtility.m_graphTraversalCallback = m_graphTraversalCallback;
                m_callbackUtility.m_resourceCreationCallback = m_resourseCreationCallback;
            }

            /// <summary>
            /// Create all the effects
            /// </summary>
            //virtual void CreatePipeline(const Core::Utility::RenderData& renderData, Renderer::RenderGraph::Graph<RenderGraphNodeBase>& graph) = 0;

            /// <summary>
            /// Create flat render pass/ task arrays segregated on level basis
            /// </summary>
            virtual void CompilePipeline();
            //virtual void DestroyPipeline() = 0;
            //virtual void ExecutePipeline() = 0;

            const std::map<uint32_t, PerLevelTaskInfo>& GetPerLevelTaskInfo();
        };
    }
}

#endif //RENDERER_PIPELINE_H_


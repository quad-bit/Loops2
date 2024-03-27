#ifndef RENDERER_LIGHTCULL_H_
#define RENDERER_LIGHTCULL_H_

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
            class LightCull : public Technique
            {
            private:
                struct SceneInfo
                {
                    glm::mat4 m_view;
                    glm::mat4 m_projection;
                    float m_camNear;
                    float m_camFar;
                    uint32_t m_screenWidth;
                    uint32_t m_screenHeight;
                };

                struct Cluster
                {
                    glm::vec4 m_minPos;
                    glm::vec4 m_maxPos;
                    uint32_t lightIds[MAX_LIGHTS_PER_CLUSTER];
                    uint32_t numLights;
                };

                struct ClusterInfo
                {
                    Cluster m_clusters[NUM_CLUSTERS];
                };

                // ========= Light Bounds ================
                struct Light
                {
                    glm::vec3 m_position;
                    float m_radius;
                };
                // light bounds
                struct LightInfo
                {
                    //AABB m_bounds[MAX_POINT_LIGHTS];
                    Light m_lights[MAX_POINT_LIGHTS];
                    uint32_t m_numLights;
                };
                // ========= Bounds ================

                Core::Utility::RenderData& m_renderData;
                void CreateResources() override;

                std::vector<ResourceAlias*> m_sceneBuffer, m_clusterInfoBuffer, m_lightInfoBuffer;

                // cluster task
                GraphNodeWrapper m_sceneBufferInput_1, m_clusterInfoBufferInput;
                GraphNodeWrapper m_clusterInfoBufferOutput_1;

                // light cull task
                GraphNodeWrapper m_lightBoundBufferInput, m_sceneBufferInput_2;
                GraphNodeWrapper m_clusterInfoBufferOutput_2;

                GraphNodeWrapper m_lightCullTaskNode, m_clusterBuilderTaskNode;
                std::unique_ptr<Renderer::RenderGraph::Task> m_ligtCullComputeTask, m_clusterBuilderTask;
                uint32_t m_ligtCullComputeTaskId, m_clusterTaskId;

                uint32_t m_renderHeight, m_renderWidth;
                std::vector<int> m_clusterBuilderDescriptorSetIds, m_lightCullerDescriptorSetIds;

            public:
                LightCull(
                    Core::Utility::RenderData& renderData,
                    const Core::WindowSettings& windowSettings,
                    Renderer::RenderGraph::Graph<Renderer::RenderGraph::Utils::RenderGraphNodeBase>& graph,
                    Renderer::RenderGraph::Utils::CallbackUtility& funcs,
                    const std::string& name,
                    const std::string& effectName,
                    const Core::Utility::EffectInfo& effectInfo);

                ~LightCull();

                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphOriginResourceNodes() override;
                virtual std::vector<Renderer::RenderGraph::GraphNode<Renderer::RenderGraph::Utils::RenderGraphNodeBase>*> GetGraphEndResourceNodes() override;

                virtual void SetupFrame(const Core::Wrappers::FrameInfo& frameInfo) override;
            };
        }
    }
}

#endif //RENDERER_LIGHTCULL_H_

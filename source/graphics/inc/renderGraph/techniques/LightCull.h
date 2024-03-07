#ifndef RENDERER_LIGHTCULL_H_
#define RENDERER_LIGHTCULL_H_

#include <renderGraph/Technique.h>
#include <RenderData.h>
#include <Settings.h>
#include "renderGraph/Task.h"
#include "ECS/ECS_Setting.h"

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

                struct AABB
                {
                    glm::vec3 m_minPos;
                    glm::vec3 m_maxPos;
                };

                struct TileData
                {
                    uint32_t m_lightIdArray[10];
                    uint32_t m_arraySize;
                };

                struct LightDepthInfo
                {
                    uint32_t depthBins[MAX_POINT_LIGHTS][2];
                };


                const Core::Utility::RenderData& m_renderData;
                void CreateResources() override;

                std::vector<ResourceAlias*> m_sceneBuffer, m_boundsBuffer, m_tileBuffer, m_lightDepthBuffer;

                GraphNodeWrapper m_sceneBufferInput, m_boundBufferInput, m_tileBufferInput, m_lightDepthBufferInput;
                GraphNodeWrapper m_tileBufferOutput, m_lightDepthBufferOutput;

                GraphNodeWrapper m_taskNode;
                std::unique_ptr<Renderer::RenderGraph::Task> m_ligtCullComputeTask;
                uint32_t m_ligtCullComputeTaskId;

                uint32_t m_renderHeight, m_renderWidth;

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

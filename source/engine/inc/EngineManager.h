#pragma once
#include <string>
#include <Settings.h>
#include <RenderData.h>
#include <memory>
#include <renderGraph/RenderGraphManager.h>
#include "SceneLoader.h"

namespace Renderer
{
    class GraphicsManager;
    namespace RenderGraph
    {
        class RenderGraphManager;
    }
}

namespace Engine
{
    class SceneManager;
    class EngineManager
    {

    private:
        EngineManager() {}
        EngineManager(EngineManager const&) = delete;
        EngineManager const& operator= (EngineManager const&) = delete;

        static EngineManager* instance;

        SceneManager* sceneManagerObj;
        Renderer::GraphicsManager* m_graphicsMngrObj;

        Core::WindowSettings m_windowSettings;
        Core::Utility::RenderData m_renderData;
        //std::unique_ptr<Renderer::RenderGraph::RenderGraphManager> m_renderGraphManager;

        Core::Wrappers::FrameInfo m_frameInfo;
        std::unique_ptr<Engine::Utility::GltfLoader> m_gltfLoader;

    public:
        void Init(
            const Engine::Utility::SceneLoadInfo& sceneLoadInfo,
            const std::string& windowName = std::string{ "Test" },
            uint32_t windowWidth = 600,
            uint32_t windowHeight = 600,
            uint32_t renderWidth = 600,
            uint32_t renderHeight = 600);

        void DeInit();
        void Update();
        static EngineManager* GetInstance();
        ~EngineManager();

        const Core::Wrappers::FrameInfo& GetFrameInfo();
    };
}
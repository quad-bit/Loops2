#pragma once
#include <string>

namespace Renderer
{
    class GraphicsManager;
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

    public:
        void Init(const std::string& windowName = std::string{ "Test" },
            uint32_t windowWidth = 600,
            uint32_t windowHeight = 600,
            uint32_t renderWidth = 600,
            uint32_t renderHeight = 600);

        void DeInit();
        void Update();
        static EngineManager* GetInstance();
        ~EngineManager();
    };
}
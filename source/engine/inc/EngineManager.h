#pragma once

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
    public:
        void Init();
        void DeInit();
        void Update();
        static EngineManager* GetInstance();
        ~EngineManager();
    };
}
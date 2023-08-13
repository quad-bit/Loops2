#pragma once

class SceneManager;
class EngineManager
{

private:
    EngineManager(){}
    EngineManager(EngineManager const &) {}
    EngineManager const & operator= (EngineManager const &) {}

    static EngineManager* instance;

    SceneManager * sceneManagerObj;
public:
    void Init();
    void DeInit();
    void Update();
    static EngineManager* GetInstance();
    ~EngineManager();
};

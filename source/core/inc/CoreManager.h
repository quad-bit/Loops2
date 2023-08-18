#pragma once

namespace Core
{
    class CoreManager
    {
    private:
        CoreManager() {};
        CoreManager(CoreManager const&) {}
        CoreManager const& operator= (CoreManager const&) {}

        static CoreManager* instance;

    public:
        void Init();
        void DeInit();
        void Update();
        static CoreManager* GetInstance();
        ~CoreManager();
    };
}
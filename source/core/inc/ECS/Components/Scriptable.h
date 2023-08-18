#pragma once
#include <ECS/Component.h>
#include <string>


namespace Core
{
    namespace ECS
    {
        class EntityHandle;
        namespace Components
        {
            class Scriptable : public Component<Scriptable>
            {
            private:
                Scriptable() = delete;

            public:
                //Entity * entity;
                Core::ECS::EntityHandle* entityHandle;
                std::string scriptName;
                bool runInEditMode = false;
                virtual void Activated() {};
                virtual void Init() {};
                virtual void Update(float dt) {};
                virtual void Render(float dt) {};
                virtual void DeInit() {};
                Scriptable(bool runInEditMode = false) { this->runInEditMode = runInEditMode; }
                virtual ~Scriptable() {};
            };
        }
    }
}
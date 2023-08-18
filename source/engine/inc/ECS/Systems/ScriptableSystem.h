
#pragma once
#include "ECS/System.h"

namespace Core
{
    namespace ECS
    {
        namespace Components
        {
            class Scriptable;
        }
    }
}

class ScriptableSystem : public Core::ECS::System
{
private:
    
public:
    virtual void Init() override;
    virtual void DeInit() override;
    virtual void Update(float dt) override;
    virtual void Render(float dt) override;

    ScriptableSystem();
    virtual ~ScriptableSystem();
};

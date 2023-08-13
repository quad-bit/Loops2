#pragma once
#include "Component.h"
#include <string>

class EntityHandle;
class Scriptable : public Component<Scriptable>
{
private:
    Scriptable() = delete;

public:
    //Entity * entity;
    EntityHandle * entityHandle;
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
#pragma once
//#include <InputManager.h>

class EntityHandle;
//struct KeyInputEvent;
//struct NodeAdditionEvent;

namespace ECS
{
    namespace Components
    {
        class Scriptable;
        class Transform;
    }
}

class SceneManager
{
private:
    EntityHandle * sceneRootEntityHandle;
    ECS::Components::Transform * sceneRootTransform;

public:
    SceneManager();
    ~SceneManager();

    ECS::Components::Transform * const GetSceneRootTransform();
    //void HandleSceneControls(KeyInputEvent * inputEvent);

    // previously commented out
    //void HandleSceneNodeAddition(NodeAdditionEvent * inputEvent);
};

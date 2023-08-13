#pragma once
#include <InputManager.h>

class EntityHandle;
class Scriptable;
struct KeyInputEvent;
struct NodeAdditionEvent;
class Transform;

class SceneManager
{
private:
    EntityHandle * sceneRootEntityHandle;
    Transform * sceneRootTransform;
    
public:
    SceneManager();
    ~SceneManager();

    Transform * const GetSceneRootTransform();
    void HandleSceneControls(KeyInputEvent * inputEvent);
    //void HandleSceneNodeAddition(NodeAdditionEvent * inputEvent);
};

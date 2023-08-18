#pragma once
//#include <InputManager.h>

//struct KeyInputEvent;
//struct NodeAdditionEvent;

namespace Core
{
    namespace ECS
    {
        class EntityHandle;
        namespace Components
        {
            class Scriptable;
            class Transform;
        }
    }
}


namespace Engine
{
    class SceneManager
    {
    private:
        Core::ECS::EntityHandle* sceneRootEntityHandle;
        Core::ECS::Components::Transform* sceneRootTransform;

    public:
        SceneManager();
        ~SceneManager();

        Core::ECS::Components::Transform* const GetSceneRootTransform();
        //void HandleSceneControls(KeyInputEvent * inputEvent);

        // previously commented out
        //void HandleSceneNodeAddition(NodeAdditionEvent * inputEvent);
    };
}
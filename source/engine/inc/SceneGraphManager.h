#pragma once
#include <map>
#include "ECS/SceneNode.h"

namespace Core
{
    namespace Containers
    {
        template<typename T>
        class TreeNode;

        template<typename T>
        class Tree;
    }

    namespace ECS
    {
        class SceneNode;
        namespace Events
        {
            struct NodeAdditionEvent;
            struct TreeModificationEvent;
        }
    }
}

namespace Engine
{
    class SceneGraphManager
    {
    private:
        SceneGraphManager() {}
        SceneGraphManager(SceneGraphManager const&) {}
        SceneGraphManager const& operator= (SceneGraphManager const&) {}

        static SceneGraphManager* instance;

        Core::ECS::SceneNode* rootNode;
        Core::Containers::TreeNode<Core::ECS::SceneNode>* rootTreeNode;
        Core::Containers::Tree<Core::ECS::SceneNode>* sceneTree;

        std::map<Core::ECS::SceneNode*, Core::Containers::TreeNode<Core::ECS::SceneNode>* > sceneToTreenodeMap;

    public:
        void Init(Core::ECS::SceneNode* rootNode);
        void DeInit();
        void Update();
        static SceneGraphManager* GetInstance();
        ~SceneGraphManager();

        void CreateSceneGraph();
        void NodeCreationEventHandler(Core::ECS::Events::NodeAdditionEvent* event);
        void TreeModificationEventHandler(Core::ECS::Events::TreeModificationEvent* event);
    };
}
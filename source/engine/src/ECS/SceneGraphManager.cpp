#include "SceneGraphManager.h"
#include "ECS/SceneNode.h"
#include <Utility/Container/Tree.h>
#include <ECS/Events/EventBus.h>
#include <ECS/Events/SceneChangeEvent.h>
#include <CorePrecompiled.h>
#include "ECS/Components/Transform.h"

Engine::SceneGraphManager* Engine::SceneGraphManager::instance = nullptr;

void Engine::SceneGraphManager::Init(Core::ECS::SceneNode * rootNode)
{
    PLOGD << "Scenegraph Init";
    this->rootNode = rootNode;
    this->rootTreeNode = new Core::Containers::TreeNode<Core::ECS::SceneNode>(rootNode);
    sceneTree = new Core::Containers::Tree<Core::ECS::SceneNode>( rootTreeNode );

    //subscribe to scene node/graph changes.
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<SceneGraphManager, Core::ECS::Events::NodeAdditionEvent>(this, &SceneGraphManager::NodeCreationEventHandler);
    Core::ECS::Events::EventBus::GetInstance()->Subscribe<SceneGraphManager, Core::ECS::Events::TreeModificationEvent>(this, &SceneGraphManager::TreeModificationEventHandler);
}

void Engine::SceneGraphManager::DeInit()
{
    PLOGD << "Scenegraph manager deinit";

    delete sceneTree;
}

void Engine::SceneGraphManager::Update()
{
    sceneTree->Traversal();
}

Engine::SceneGraphManager * Engine::SceneGraphManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Engine::SceneGraphManager();
    }
    return instance;
}

Engine::SceneGraphManager::~SceneGraphManager()
{
}

void Engine::SceneGraphManager::CreateSceneGraph()
{
}

void Engine::SceneGraphManager::NodeCreationEventHandler(Core::ECS::Events::NodeAdditionEvent * event)
{
    Core::ECS::SceneNode * sceneNode = event->node;
    Core::Containers::TreeNode<Core::ECS::SceneNode> * treeNode = new Core::Containers::TreeNode<Core::ECS::SceneNode>(sceneNode);
    sceneToTreenodeMap.insert(std::pair< Core::ECS::SceneNode *, Core::Containers::TreeNode<Core::ECS::SceneNode> *>(sceneNode, treeNode));

    sceneTree->AddToTree(treeNode);
}

void Engine::SceneGraphManager::TreeModificationEventHandler(Core::ECS::Events::TreeModificationEvent * event)
{
    switch (event->mod)
    {
    case Core::ECS::Events::TreeModificationEvent::ModType::SET_PARENT:
    {
        Core::ECS::SceneNode * node = event->node;
        Core::ECS::SceneNode * parent = event->newParent;

        Core::Containers::TreeNode<Core::ECS::SceneNode> * treeNode = sceneToTreenodeMap.at(node);
        Core::Containers::TreeNode<Core::ECS::SceneNode> * treeNodeParent = sceneToTreenodeMap.at(parent);
        
        treeNode->SetParent(treeNodeParent);
    }
        break;

    default:
        ASSERT_MSG_DEBUG(0, "yet to be implemented");
        break;
    }
}

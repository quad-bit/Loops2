#include "SceneGraphManager.h"
#include "SceneNode.h"
#include <Tree.h>
#include <EventBus.h>
#include <SceneChangeEvent.h>
#include <CorePrecompiled.h>
#include "Transform.h"

SceneGraphManager* SceneGraphManager::instance = nullptr;

void SceneGraphManager::Init(SceneNode * rootNode)
{
    PLOGD << "Scenegraph Init";
    this->rootNode = rootNode;
    this->rootTreeNode = new TreeNode<SceneNode>(rootNode);
    sceneTree = new Tree<SceneNode>( rootTreeNode );

    //subscribe to scene node/graph changes.
    EventBus::GetInstance()->Subscribe<SceneGraphManager, NodeAdditionEvent>(this, &SceneGraphManager::NodeCreationEventHandler);
    EventBus::GetInstance()->Subscribe<SceneGraphManager, TreeModificationEvent>(this, &SceneGraphManager::TreeModificationEventHandler);
}

void SceneGraphManager::DeInit()
{
    PLOGD << "Scenegraph manager deinit";

    delete sceneTree;
}

void SceneGraphManager::Update()
{
    sceneTree->Traversal();
}

SceneGraphManager * SceneGraphManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new SceneGraphManager();
    }
    return instance;
}

SceneGraphManager::~SceneGraphManager()
{
}

void SceneGraphManager::CreateSceneGraph()
{
}

void SceneGraphManager::NodeCreationEventHandler(NodeAdditionEvent * event)
{
    SceneNode * sceneNode = event->node;
    TreeNode<SceneNode> * treeNode = new TreeNode<SceneNode>(sceneNode);
    sceneToTreenodeMap.insert(std::pair< SceneNode *, TreeNode<SceneNode> *>(sceneNode, treeNode));

    sceneTree->AddToTree(treeNode);
}

void SceneGraphManager::TreeModificationEventHandler(TreeModificationEvent * event)
{
    switch (event->mod)
    {
    case TreeModificationEvent::ModType::SET_PARENT:
    {
        SceneNode * node = event->node;
        SceneNode * parent = event->newParent;

        TreeNode<SceneNode> * treeNode = sceneToTreenodeMap.at(node);
        TreeNode<SceneNode> * treeNodeParent = sceneToTreenodeMap.at(parent);
        
        treeNode->SetParent(treeNodeParent);
    }
        break;

    default:
        ASSERT_MSG_DEBUG(0, "yet to be implemented");
        break;
    }
}

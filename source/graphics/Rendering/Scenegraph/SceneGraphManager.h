#pragma once
#include <map>
#include "SceneNode.h"

template<typename T>
class TreeNode;

template<typename T>
class Tree;

class SceneNode;
struct NodeAdditionEvent;
struct TreeModificationEvent;

class SceneGraphManager
{

private:
    SceneGraphManager(){}
    SceneGraphManager(SceneGraphManager const &) {}
    SceneGraphManager const & operator= (SceneGraphManager const &) {}

    static SceneGraphManager* instance;
    
    SceneNode * rootNode;
    TreeNode<SceneNode> * rootTreeNode;
    Tree<SceneNode> * sceneTree;

    std::map<SceneNode *, TreeNode<SceneNode>* > sceneToTreenodeMap;

public:
    void Init(SceneNode * rootNode );
    void DeInit();
    void Update();
    static SceneGraphManager* GetInstance();
    ~SceneGraphManager();

    void CreateSceneGraph();
    void NodeCreationEventHandler(NodeAdditionEvent * event);
    void TreeModificationEventHandler(TreeModificationEvent * event);
};

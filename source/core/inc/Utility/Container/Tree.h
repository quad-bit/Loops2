#pragma once
//#include "DoublyLinkedList.h"
#include <iostream>
#include <string>
#include <Assertion.h>

template <class T>
class Tree;

template <class T>
class TreeNode
{
    friend class Tree<T>;

private:
    T * data;
    TreeNode<T> * nodeParent;
    TreeNode<T> *next, *prev, *child;

    TreeNode<T> * treeRoot;

    uint32_t nodeId;
    static uint32_t idCounter;
    void AssignId();
    void AddSibling(TreeNode<T> * node);

public:
	TreeNode(T * data);
	~TreeNode();
	void AddChild(TreeNode<T> * child);
	void DetachChild(TreeNode<T> * child);
	void DeleteChild(TreeNode<T> * child);
	void SetParent(TreeNode<T> * nodeParent);
};

template <typename T>
uint32_t TreeNode<T>::idCounter = 0;

template<class T>
inline TreeNode<T>::TreeNode(T * data)
{
	this->data = data;
	next = NULL;
	prev = NULL;
	child = NULL;
    nodeParent = NULL;

    AssignId();
}

template<class T>
inline TreeNode<T>::~TreeNode()
{
    if (child != NULL)
    {
        TreeNode<T> * ptr = child->next;
        if (ptr == NULL)
        {

        }
        else
        {
            while (ptr->next != NULL)
            {
                ptr = ptr->next;
            }

            while (ptr->prev != NULL) // Need unique identifier for each node
            {
                TreeNode<T> * temp = ptr;
                ptr = ptr->prev;
                delete temp;
                temp = NULL;
                ptr->next = NULL;
            }

        }

        delete child;
        child = NULL;
    }

    if (this->next != NULL && this->prev != NULL)
    {
        this->prev->next = this->next;
        this->next->prev = this->prev;
    }

    if (this->next == NULL && this->prev != NULL)
    {
        this->prev->next = NULL;
    }

    this->prev = NULL;
    this->next = NULL;
}

template<class T>
inline void TreeNode<T>::AddChild(TreeNode<T>* node)
{
	if (this->child == NULL)
	{
		this->child = node;
        this->child->prev = NULL;
	}
	else
	{
		child->AddSibling(node);
	}

    node->nodeParent = this;
}

template<class T>
inline void TreeNode<T>::DetachChild(TreeNode<T>* child)
{
    if (child->nodeParent->nodeId == treeRoot->nodeId)
    {
        ASSERT_MSG_DEBUG(0, "Only node deletion possible, if nodeParent is root");
        return;
    }

    // detach child from nodeParent
    // is this the first child
    if ( this->child->nodeId == childe->nodeId)//  child->prev == NULL)
    {
        if (child->next != NULL)
        {
            TreeNode<T> * newChild = child->next;
            this->child = newChild;
            newChild->prev = NULL;
        }
        //no siblings
        else
        {
            this->child = NULL;
        }
    }
    else
    {
        if(child->prev != NULL)
            child->prev->next = child->next;

        if (child->next != NULL)
            child->next->prev = child->prev->next;
    }

    child->next = NULL;
    child->prev = NULL;

    child->SetParent(treeRoot);
}

template<class T>
inline void TreeNode<T>::AssignId()
{
    nodeId = idCounter;
    idCounter++;
}

template<class T>
inline void TreeNode<T>::AddSibling(TreeNode<T>* node)
{
    TreeNode<T> * ptr = next;

	if (next == NULL)
	{
		next = node;
		node->prev = this;
	}
	else
	{
		while (ptr->next != NULL)
			ptr = ptr->next;
		ptr->next = node;
		node->prev = ptr;
	}
    node->nodeParent = this->nodeParent;
}

template<class T>
inline void TreeNode<T>::DeleteChild(TreeNode<T>* child)
{
    DetachChild(child);
    delete child;
}

template<class T>
inline void TreeNode<T>::SetParent(TreeNode<T>* nodeParent)
{
    // initially when the node gets created its parent is NULL
    // when its pushed to the tree it gets added to the root node
    if (this->nodeParent == NULL )
    {
        this->nodeParent = nodeParent;
        nodeParent->AddChild(this);
    }
    else
    {
        // detach child from nodeParent
        // is this the first child
        if (this->prev == NULL) // ||(this->nodeParent->child->nodeId == this->nodeId)
        {
            if (this->next != NULL)
            {
                TreeNode<T> * newChild = this->next;
                this->nodeParent->child = newChild;
                newChild->prev = NULL;
            }
            //no siblings
            else
            {
                this->nodeParent->child = NULL;
            }
        }
        else
        {
            if (this->prev != NULL)
                this->prev->next = this->next;

            if (this->next != NULL)
                this->next->prev = this->prev->next;
        }

        this->prev = NULL;
        this->next = NULL;

        //attach it to new nodeParent
        this->nodeParent = nodeParent;
        nodeParent->AddChild(this);
    }
}


template <class T>
class Tree
{
private:
    TreeNode<T> * root;

public:
    Tree();
    Tree(TreeNode<T> * root);
    ~Tree();
    void AddToTree(TreeNode<T> * node);
    bool Search(TreeNode<T> * root, T * data);
    bool Search(T * data);
    virtual void Traversal(TreeNode<T> * node);
    virtual void Traversal();

    const TreeNode<T> * GetRoot() const;
};

template<class T>
inline Tree<T>::Tree()
{
    //root = new TreeNode<T>(-1);
}

template<class T>
inline Tree<T>::Tree(TreeNode<T>* root)
{
    this->root = root;
}

template<class T>
inline Tree<T>::~Tree()
{
    delete root; // TODO: Handle deletion.
}

template<class T>
inline void Tree<T>::AddToTree(TreeNode<T>* node)
{
    if (node->nodeParent == NULL)
    {
        //node->SetParent(root);
        root->AddChild(node);
    }

    node->treeRoot = root;
}

template<class T>
inline bool Tree<T>::Search(TreeNode<T>* nodeParent, T * data)
{
    if (nodeParent->data == data)
    	return true;

    if (nodeParent->child != NULL)
    {
    	if (Search(nodeParent->child, data) == true)
    		return true;
    }

    if (nodeParent->next != NULL)
    {
    	if (Search(nodeParent->next, data) == true)
    		return true;
    }

    return false;
}

template<class T>
inline bool Tree<T>::Search(T * data)
{
    TreeNode<T> * nodeParent = root;

    if (nodeParent->data == data)
    	return true;

    if (nodeParent->child != NULL)
    {
    	if (Search(nodeParent->child, data) == true)
    		return true;
    }

    if (nodeParent->next != NULL)
    {
    	if (Search(nodeParent->next, data) == true)
    		return true;
    }

    return false;
}

template<class T>
inline void Tree<T>::Traversal(TreeNode<T> * node)
{
    if (node->data != NULL)
        node->data->Entry();

    if (node->child != NULL)
    {
    	//std::cout << std::endl;
    	Traversal(node->child);
    }
    
    if (node->data != NULL)
        node->data->Exit();

    if (node->next != NULL)
    {
    	//std::cout << " ";
    	Traversal(node->next);
    }
}

template<class T>
inline void Tree<T>::Traversal()
{
    TreeNode<T> * node = root;

    if (node->data != NULL)
    	node->data->Entry();

    if (node->child != NULL)
    {
    	Traversal(node->child);
    }

    if (node->data != NULL)
        node->data->Exit();

    if (node->next != NULL)
    {
    	Traversal(node->next);
    }
}

template<class T>
inline const TreeNode<T>* Tree<T>::GetRoot() const
{
    return root;
}

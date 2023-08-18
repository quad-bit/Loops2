#pragma once
#include <iostream>

namespace Core
{
    namespace Containers
    {
        template <class T>
        class BinaryTree;

        template <class T>
        class BTNode
        {
            friend class BinaryTree<T>;

        private:
            T key;
            BTNode* left, * right;

        public:
            BTNode(T obj) : key(obj), left(NULL), right(NULL)
            {}

            ~BTNode()
            {
                if (left != NULL)
                {
                    delete left;
                    left = NULL;
                }

                if (right != NULL)
                {
                    delete right;
                    right = NULL;
                }
            }

            T GetKey()
            {
                return key;
            }
        };

        template <class T>
        class BinaryTree
        {
        private:
            BTNode<T>* root;

            void DisplayInOrder(BTNode<T>* node);
            void DisplayPreOrder(BTNode<T>* node);
            void DisplayPostOrder(BTNode<T>* node);

        public:
            BinaryTree() : root(NULL) {}

            ~BinaryTree()
            {
                if (root != NULL)
                {
                    delete root;
                    root = NULL;
                }
            }

            bool Push(T key);
            bool Search(T key);

            void DisplayInOrder() { DisplayInOrder(root); }
            void DisplayPreOrder() { DisplayPreOrder(root); }
            void DisplayPostOrder() { DisplayPostOrder(root); }
            void Remove(T key);
        };
    }
}

template<class T>
inline void Core::Containers::BinaryTree<T>::DisplayInOrder(BTNode<T>* node)
{
    if (node == NULL)
        return;

    DisplayInOrder(node->left);
    std::cout << node->key << " ";
    DisplayInOrder(node->right);
}

template<class T>
inline void Core::Containers::BinaryTree<T>::DisplayPreOrder(BTNode<T>* node)
{
    if (node != NULL)
    {
        std::cout << node->key << " ";
        DisplayPreOrder(node->left);
        DisplayPreOrder(node->right);
    }
}

template<class T>
inline void Core::Containers::BinaryTree<T>::DisplayPostOrder(BTNode<T>* node)
{
    if (node == NULL)
        return;

    DisplayPostOrder(node->left);
    DisplayPostOrder(node->right);
    std::cout << node->key << " ";
}

template<class T>
inline bool Core::Containers::BinaryTree<T>::Push(T key)
{
    BTNode<T> * node = new BTNode<T>(key);

    if (root == NULL)
    {
        root = node;
    }
    else
    {
        BTNode<T> * parentNode = NULL;
        BTNode<T> * currentNode = root;

        while (1)
        {
            parentNode = currentNode;

            if (key == currentNode->key)
            {
                delete node;
                return false;
            }

            if (currentNode->key > key)
            {
                currentNode = currentNode->left;

                if (currentNode == NULL)
                {
                    parentNode->left = node;
                    return true;
                }
            }
            else
            {
                currentNode = currentNode->right;

                if (currentNode == NULL)
                {
                    parentNode->right = node;
                    return true;
                }
            }
        }
    }

    return true;
}

template<class T>
inline bool Core::Containers::BinaryTree<T>::Search(T key)
{
    if (root == NULL)
    {
        return false;
    }

    BTNode<T> * currentNode;

    currentNode = root;
    while (currentNode->key != key)
    {
        if (currentNode->key < key)
        {
            currentNode = currentNode->right;
        }
        else
        {
            currentNode = currentNode->left;
        }

        if (currentNode == NULL)
            return false;

    }
    return true;
}

template<class T>
inline void Core::Containers::BinaryTree<T>::Remove(T key)
{
    if (root == NULL)
        return;

    BTNode<T> * parent = root;
    BTNode<T> * node = root;

    bool isLeftNode = false;

    while (node->key != key)
    {
        parent = node;

        if (key < node->key)
        {
            node = node->left;
            isLeftNode = true;
        }
        else
        {
            node = node->right;
            isLeftNode = false;
        }

        if (node == NULL)
            return;
    }

    if (node->left == NULL && node->right == NULL)
    {
        if (node == root)
            root = NULL;
        else if (isLeftNode == true)
            parent->left = NULL;
        else
            parent->right = NULL;
    }
    else if (node->left == NULL)
    {
        if (node == root)
            root = node->right;
        else if (isLeftNode)
            parent->left = node->right;
        else
            parent->right = node->right;

    }
    else if (node->right == NULL)
    {
        if (node == root)
            root = node->left;
        else if (isLeftNode)
            parent->left = node->left;
        else
            parent->right = node->left;
    }
    else
    {
        BTNode<T> * tempNode = node->right;
        BTNode<T> * successor = node;
        BTNode<T> * successorParent = node;

        while (tempNode != NULL)
        {
            successorParent = successor;
            successor = tempNode;
            tempNode = tempNode->left;
        }

        if (successor != node->right)
        {
            successorParent->left = successor->right;
            successor->right = node->right;
        }

        if (node == root)
        {
            root = successor;
        }
        else if (isLeftNode)
        {
            node = parent->left;
            parent->left = successor;
        }
        else
        {
            node = parent->right;
            parent->right = successor;
        }

        successor->left = node->left;
    }

    node->left = NULL;
    node->right = NULL;
    delete node;
}


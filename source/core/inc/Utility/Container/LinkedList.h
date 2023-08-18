#pragma once
#include "Platform/Assertion.h"
#include <assert.h>
#include <iostream>
#include <stdint.h>

namespace Core
{
    namespace Containers
    {
        template < class T>
        class LinkList;

        template < class T>
        class LinkIterator;

        template <class T>
        class Node
        {
            friend class LinkList<T>;
            friend class LinkIterator<T>;

        private:
            T data;
            Node* next;
        };

        template < class T>
        class LinkIterator
        {
        private:
            Node<T>* node;
        public:
            LinkIterator() {}
            ~LinkIterator() {}

            void operator = (Node<T>* node)
            {
                this->node = node;
            }

            T& operator*()
            {
                assert(node != NULL);
                return node->data;
            }

            void operator ++()
            {
                assert(node != NULL);
                if (node->next != NULL)
                    node = node->next;
                else
                    std::cout << "last node";
            }

            void operator++(int val)
            {
                assert(node != NULL);
                for (int i = 0; i < val; i++)
                {
                    if (node->next != NULL)
                        node = node->next;
                    else
                    {
                        std::cout << "last node";
                        break;
                    }
                }
            }

            bool operator != (Node<T>* node)
            {
                assert(node != NULL);
                assert(this->node != NULL);

                return (this->node != node);
            }

            bool operator == (Node<T>* node)
            {
                return (this->node == node);
            }
        };


        template < class T>
        class LinkList
        {
        private:
            std::uint32_t size;
            Node<T>* firstNode;
            Node<T>* lastNode;

        public:
            LinkList() : size(0), firstNode(0), lastNode(0)
            {

            }

            ~LinkList()
            {
                Clear();
            }

            Node<T>* Begin() const
            {
                assert(firstNode != NULL);
                return firstNode;
            }

            Node<T>* End() const
            {
                assert(lastNode != NULL);
                return lastNode;
            }

            void Push(T data)
            {
                Node<T>* node = new Node<T>();
                node->data = data;
                node->next = NULL;

                if (firstNode == NULL)
                {
                    firstNode = node;
                    lastNode = node;
                }
                else
                {
                    lastNode->next = node;
                    lastNode = node;
                }
                size++;
            }

            void Pop()
            {
                assert(firstNode != NULL);

                if (firstNode->next == NULL)
                {
                    delete firstNode;
                    firstNode = NULL;
                }
                else
                {
                    Node<T>* prevNode = firstNode;

                    while (prevNode->next != NULL && prevNode->next != lastNode)
                    {
                        prevNode = prevNode->next;
                    }

                    delete lastNode;
                    prevNode->next = NULL;
                    lastNode = prevNode;
                }

                size = (size == 0 ? 0 : size - 1);
            }

            void PushFront(T data)
            {
                Node<T>* node = new Node<T>();
                node->data = data;
                node->next = NULL;

                if (firstNode != NULL)
                {
                    node->next = firstNode;
                    firstNode = node;
                }
                else
                {
                    firstNode = node;
                    lastNode = node;
                }
                size++;
            }

            void PopFront()
            {
                assert(firstNode != NULL);
                Node<T>* temp = firstNode;
                firstNode = firstNode->next;

                delete temp;

                size = (size == 0 ? 0 : size - 1);
            }

            std::uint32_t GetSize() const
            {
                return size;
            }

            void Clear()
            {
                while (firstNode != NULL)
                {
                    Pop();
                }
            }

            void Insert(T data, std::uint32_t location)
            {

                ASSERT(location >= 0);
                ASSERT(location <= size);

                if (location == 0)
                {
                    PushFront(data);
                    return;
                }
                else if (location == GetSize())
                {
                    Push(data);
                    return;
                }

                Node<T>* temp = firstNode;

                for (uint32_t i = 0; i < location - 1; i++)
                {
                    temp = temp->next;
                }

                Node<T>* newNode = new Node<T>;
                newNode->data = data;

                Node<T>* nextNode = temp->next;
                temp->next = newNode;
                newNode->next = nextNode;

                size++;
            }
        };
    }
}
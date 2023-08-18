#pragma once
#include <stdint.h>
#include <assert.h>

namespace Core
{
    namespace Containers
    {
        template < class T>
        class DblLinkList;

        template < class T>
        class DblLinkIterator;

        template <class T>
        class DblLinkNode
        {
            friend class DblLinkList<T>;
            friend class DblLinkIterator<T>;

        private:
            T data;
            DblLinkNode* next, * prev;
        };


        template < class T>
        class DblLinkIterator
        {
        private:
            DblLinkNode<T>* node;

        public:
            DblLinkIterator() {}
            ~DblLinkIterator() {}

            void operator=(DblLinkNode<T>* node)
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

            void operator--()
            {
                assert(node != NULL);

                if (node->prev != NULL)
                    node = node->prev;
                else
                    std::cout << "first node";
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

            void operator--(int val)
            {
                assert(node != NULL);
                for (int i = 0; i < val; i++)
                {
                    if (node->prev != NULL)
                        node = node->prev;
                    else
                    {
                        std::cout << "last node";
                        break;
                    }
                }
            }


            bool operator != (DblLinkNode<T>* node)
            {
                //assert(node != NULL);
                assert(this->node != NULL);

                return (this->node != node);
            }

            bool operator == (DblLinkNode<T>* node)
            {
                return (this->node == node);
            }

        };


        template < class T>
        class DblLinkList
        {
        private:
            uint32_t size;
            DblLinkNode<T>* firstNode;
            DblLinkNode<T>* lastNode;

        public:

            DblLinkList()
            {
                size = 0;
                firstNode = 0;
                lastNode = 0;
            }

            ~DblLinkList()
            {
                Clear();
            }

            DblLinkNode<T>* Begin()
            {
                assert(firstNode != NULL);
                return firstNode;
            }

            DblLinkNode<T>* End()
            {
                assert(lastNode != NULL);
                return lastNode;
            }

            void Push(T data)
            {
                DblLinkNode<T>* node = new DblLinkNode<T>;
                node->data = data;
                node->next = NULL;
                node->prev = NULL;

                if (firstNode == NULL)
                {
                    firstNode = node;
                    lastNode = node;
                }
                else
                {
                    node->prev = lastNode;
                    lastNode->next = node;
                    lastNode = node;
                }
                size++;
            }

            void PushFront(T data)
            {
                DblLinkNode<T>* node = new DblLinkNode<T>;
                node->data = data;
                node->next = NULL;
                node->prev = NULL;

                if (firstNode == NULL)
                {
                    firstNode = node;
                    lastNode = node;
                }
                else
                {
                    node->next = firstNode;
                    firstNode->prev = node;
                    firstNode = node;
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
                    DblLinkNode<T>* prevNode = firstNode;

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

            void PopFront()
            {
                assert(firstNode != NULL);

                if (firstNode->next == NULL)
                {
                    delete firstNode;
                    firstNode = NULL;
                }
                else
                {
                    DblLinkNode<T>* temp = firstNode;

                    firstNode = temp->next;
                    firstNode->prev = NULL;

                    delete temp;
                    temp = NULL;
                }

                size = (size == 0 ? 0 : size - 1);
            }

            uint32_t GetSize()
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

        };
    }
}
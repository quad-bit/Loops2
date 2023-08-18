#pragma once
#include "LinkedList.h"
#include <stdint.h>

namespace Core
{
    namespace Containers
    {
        template <class T>
        class Queue
        {
        private:
            LinkList<T> elems;
            int maxSize;

        public:
            Queue(int size);
            ~Queue() {}
            void Push(T val);
            void Pop();
            const T& Front();

            int GetSize()
            {
                return elems.size();
            }

            int GetMaxSize()
            {
                return maxSize;
            }

            bool IsEmpty()
            {
                return (elems.GetSize() == 0);
            }

            void Resize(int size)
            {
                assert(size > 0);
                this->maxSize = size;
            }
        };
    }
}

template<class T>
inline Core::Containers::Queue<T>::Queue(int size)
{
    assert(size > 0);
    this->maxSize = size;
}

template<class T>
inline void Core::Containers::Queue<T>::Push(T val)
{
    if (elems.GetSize() < (uint32_t)maxSize)
    	elems.Push(val);
}

template<class T>
inline void Core::Containers::Queue<T>::Pop()
{
    elems.PopFront();
}

template<class T>
inline const T & Core::Containers::Queue<T>::Front()
{
    // TODO: insert return statement here
    Core::Containers::LinkIterator<T> it;
    it = elems.Begin();

    return *it;
}


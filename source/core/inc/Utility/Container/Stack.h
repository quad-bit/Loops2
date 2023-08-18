#pragma once
#include <vector>
#include <stdexcept>
#include "LinkedList.h"

namespace Core
{
    namespace Containers
    {
        template <class T>
        class Stack
        {
        private:
            LinkList<T> container;
            int size;

        public:
            void Push(T const& obj);
            void Pop();
            const T& Top() const;
            bool Empty() const
            {
                return (container.GetSize() == 0 ? true : false);
            }

            //Stack<T>& operator=(Stack<T> const &);
        };
    }
}

template<class T>
inline void Core::Containers::Stack<T>::Push(T const & obj)
{
    container.Push(obj);
}

template<class T>
inline void Core::Containers::Stack<T>::Pop()
{
    if (Empty())
    	throw std::out_of_range("cannot pop from empty stack ");

    container.Pop();
}

template<class T>
inline const T& Core::Containers::Stack<T>::Top() const
{
    if (Empty())
    	throw std::out_of_range("empty stack ");

    Core::Containers::LinkIterator<T> it;
    it = container.End();

    return *it;
}

/*
template<class T>
inline Stack<T>& Stack<T>::operator=(Stack<T> const &other)
{
	if ((void*)this == (void*)&other)
		return *this;

	Stack<T> tmp(other);
	elements.clear();

	while (!tmp.Empty())
	{
		elements.push_back(tmp.Top());
		tmp.Pop();
	}

	return *this;

}
*/

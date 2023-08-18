#pragma once
#include <cstdlib>
#include <assert.h>
#include <iostream>
#include "LinkedList.h"

#include "Platform/Assertion.h"

namespace Core
{
    namespace Containers
    {
        template <typename T>
        class Vector // unordered array
        {
        private:

            enum STRUCTURE_TYPE
            {
                FIXED,
                DYNAMIC,
                NONE
            }structureType;


            T* array;
            LinkList<T> list;
            int maxSize;
            int growSize;
            int numElements;


        public:
            Vector(int size, int growBy = 1);
            Vector();
            ~Vector();
            T& operator[](int index);

            virtual void Push(T val);
            void Pop();
            bool RemoveAt(int index);
            bool Expand();
            void PrintElements();
            void Clear();
            bool Empty();
            int GetSize();
            void Insert(T data, int location);
        };
    }
}

template<typename T>
inline Core::Containers::Vector<T>::Vector(int size, int growBy) : array(NULL), maxSize(0), growSize(0), numElements(0)
{
    if (size)
    {
        maxSize = size;
        array = new T[maxSize];

        growSize = (growBy > 0) ? growBy : 0;
        structureType = STRUCTURE_TYPE::FIXED;
    }
    else
    structureType = STRUCTURE_TYPE::NONE;
}

template<typename T>
inline Core::Containers::Vector<T>::Vector()
{
    structureType = STRUCTURE_TYPE::DYNAMIC;
}

template<typename T>
inline Core::Containers::Vector<T>::~Vector()
{
    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        if (array != NULL)
        {
            delete[] array;
            array = NULL;
        }
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        list.Clear();
    }
}

template<typename T>
inline T & Core::Containers::Vector<T>::operator[](int index)
{
    ASSERT(index > 0);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);
        ASSERT(index < numElements);
        //assert(numElements >= 0);
        ASSERT_MSG_DEBUG(numElements <= 0,"test");

        for (int i = 0; i < numElements; i++)
        {
            if (i == index)
            {
                return array[i];
            }
        }
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        ASSERT_MSG_DEBUG(list.GetSize() > 0, "vector empty");
        ASSERT_MSG_DEBUG((int)list.GetSize() > index, "out of bounds");

        Core::Containers::LinkIterator<T> it;
        it = list.Begin();

        int count = 0;
        for (it = list.Begin(); it != list.End(); ++it , count++)
        {
            //cout << "  " << *it;
            if (count == index)
                return *it;
        }
    }
    
    ASSERT_MSG_DEBUG(0, "invalid vector type");
    return array[0];
}

template<typename T>
inline void Core::Containers::Vector<T>::Push(T val)
{
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if(structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);

        if (numElements >= maxSize)
        {
            //expand the array
            Expand();
        }

        array[numElements++] = val;
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        list.Push(val);
    }
}

template<typename T>
inline void Core::Containers::Vector<T>::Pop()
{
    ASSERT(Empty() != true);
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);

        if (numElements > 0)
            numElements--;
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        list.Pop();
    }
}

template<typename T>
inline bool Core::Containers::Vector<T>::RemoveAt(int index)
{
    ASSERT(Empty() != true);
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);
        if (index >= maxSize)
        {
            return false;
        }

        for (int k = index; k < maxSize; k++)
        {
            array[k] = array[k + 1];
        }

        numElements--;

        return true;
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        ASSERT(0);
        Core::Containers::LinkIterator<T> it;
        it = list.End();
    }

    return false;
}

template<typename T>
inline bool Core::Containers::Vector<T>::Expand()
{
    if (growSize <= 0)
        return false;
    T* temp = new T[maxSize + growSize];
    ASSERT(temp != NULL);

    maxSize += growSize;

    memcpy(temp, array, sizeof(T) * maxSize);

    delete[] array;

    array = temp;
    return true;
}

template<typename T>
inline void Core::Containers::Vector<T>::PrintElements()
{
    ASSERT(array != NULL);

    for (int k = 0; k < numElements; k++)
    {
        std::cout << array[k] << std::endl;
    }
}

template<typename T>
inline void Core::Containers::Vector<T>::Clear()
{
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);

        if (numElements == 0)
            return;

        while (numElements != 0)
        {
            Pop();
            numElements--;
        }
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        list.Clear();
    }
}

template<typename T>
inline bool Core::Containers::Vector<T>::Empty()
{
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);

        if (numElements == 0)
            return true;
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        if (list.GetSize() == 0)
            return true;
    }

    return false;
}
template<typename T>
inline int Core::Containers::Vector<T>::GetSize()
{
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);

        return numElements;
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        return list.GetSize();
    }

    return 0;
}

template<typename T>
inline void Core::Containers::Vector<T>::Insert(T data, int location)
{
    ASSERT(structureType != STRUCTURE_TYPE::NONE);

    if (structureType == STRUCTURE_TYPE::FIXED)
    {
        ASSERT(array != NULL);
    }
    else if (structureType == STRUCTURE_TYPE::DYNAMIC)
    {
        list.Insert(data, location);
    }
}

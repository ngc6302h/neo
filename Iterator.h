/*
 * Copyright (C) 2021  Iori Torres (shortanemoia@protonmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "TypeTraits.h"

namespace neo
{
    template<typename T>
    struct DefaultIteratorContainer
    {
        T data;

        constexpr DefaultIteratorContainer(const T& other) :
            data(other)
        {
        }

        constexpr decltype(auto) operator++(int)
        {
            return data++;
        }

        constexpr decltype(auto) operator--(int)
        {
            return data--;
        }

        constexpr decltype(auto) operator++()
        {
            return ++data;
        }

        constexpr decltype(auto) operator--()
        {
            return --data;
        }

        constexpr decltype(auto) operator*()
        {
            if constexpr (IsPointer<T>)
                return *data;
            else
                return data;
        }

        constexpr bool operator==(const DefaultIteratorContainer& other) const
        {
            return data == other.data;
        }

        constexpr bool operator!=(const DefaultIteratorContainer& other) const
        {
            return data != other.data;
        }
    };

    template<typename T, typename TContainer = DefaultIteratorContainer<T>>
    class Iterator
    {
    public:
        constexpr Iterator() = delete;
        constexpr Iterator(T element) :
            m_element(element)
        {
        }
        constexpr Iterator(const Iterator& other) :
            m_element(other.m_element)
        {
        }
        constexpr Iterator(Iterator&& other) :
            m_element(move(other.m_element))
        {
        }

        constexpr Iterator& operator=(const Iterator& other)
        {
            m_element = other.m_element;
            return *this;
        }

        constexpr Iterator& operator=(Iterator&& other)
        {
            m_element = move(other.m_element);
        }

    protected:
        TContainer m_element;
    };
    

    template<typename T, typename TContainer = DefaultIteratorContainer<T>>
    class ForwardIterator : public Iterator<T, TContainer>
    {
    public:
        constexpr ForwardIterator() = default;
        constexpr ForwardIterator(T element) :
            Iterator<T, TContainer>(element)
        {
        }
        constexpr ForwardIterator(const ForwardIterator& other) :
            Iterator<T, TContainer>(other)
        {
        }
        constexpr ForwardIterator(ForwardIterator&& other) :
            Iterator<T, TContainer>(move(other))
        {
        }

        constexpr ForwardIterator& operator=(const ForwardIterator& other)
        {
            this->m_element = other.m_element;
            return *this;
        }

        constexpr ForwardIterator& operator=(ForwardIterator&& other)
        {
            this->m_element = other.m_element;
        }

        constexpr bool operator!=(const ForwardIterator& other) const
        {
            return this->m_element != other.m_element;
        }

        constexpr decltype(auto) operator*()
        {
            return *this->m_element;
        }

        constexpr decltype(auto) operator*() const
        {
            return *this->m_element;
        }
    
        constexpr decltype(auto) operator->()
        {
            if constexpr(IsPointer<T> && !IsRvalueReference<decltype(forward<T>(*this->m_element))>)
                return &*this->m_element;
            else
                return *this->m_element;
        }
    
        constexpr decltype(auto) operator->() const
        {
            if constexpr(IsPointer<T> && !IsRvalueReference<decltype(forward<T>(*this->m_element))>)
                return &*this->m_element;
            else
                return *this->m_element;
        }
    
        constexpr const TContainer& ptr() const
        {
            if constexpr(IsPointer<TContainer>)
                return &this->m_element;
            else
                return this->m_element;
        }

        //To be implemented by the class using iterators
        constexpr ForwardIterator& operator++()
        {
            this->m_element++;
            return *this;
        }

        constexpr ForwardIterator operator++(int)
        {
            auto prev = *this;
            this->m_element++;
            return prev;
        }
    };

    template<typename T, typename TContainer = DefaultIteratorContainer<T>>
    class BidirectionalIterator : public Iterator<T, TContainer>
    {
    public:
        explicit BidirectionalIterator() = default;
        constexpr BidirectionalIterator(T element) :
            Iterator<T, TContainer>(element)
        {
        }
        constexpr BidirectionalIterator(const BidirectionalIterator& other) :
            Iterator<T, TContainer>(other)
        {
        }
        constexpr BidirectionalIterator(BidirectionalIterator&& other) :
            Iterator<T, TContainer>(move(other))
        {
        }

        constexpr BidirectionalIterator& operator=(const BidirectionalIterator& other)
        {
            this->m_element = other.m_element;
            return *this;
        }

        constexpr BidirectionalIterator& operator=(BidirectionalIterator&& other)
        {
            this->m_element = other.m_element;
            return *this;
        }

        constexpr bool operator!=(const BidirectionalIterator& other) const
        {
            return this->m_element != other.m_element;
        }
    
        constexpr bool operator==(const BidirectionalIterator& other) const
        {
            return this->m_element == other.m_element;
        }

        constexpr decltype(auto) operator*()
        {
            return *this->m_element;
        }

        constexpr decltype(auto) operator*() const
        {
            return *this->m_element;
        }
    
        constexpr decltype(auto) operator->()
        {
            if constexpr(IsPointer<T> && !IsRvalueReference<decltype(*this->m_element)>)
                return &*this->m_element;
            else
                return *this->m_element;
        }
    
        constexpr decltype(auto) operator->() const
        {
            if constexpr(IsPointer<T> && !IsRvalueReference<decltype(*this->m_element)>)
                return &*this->m_element;
            else
                return *this->m_element;
        }
    
        constexpr const TContainer& ptr() const
        {
            if constexpr(IsPointer<TContainer>)
                return &this->m_element;
            else
                return this->m_element;
        }

        //To be implemented by the class using iterators
        constexpr BidirectionalIterator& operator++()
        {
            this->m_element++;
            return *this;
        }
        constexpr BidirectionalIterator operator++(int)
        {
            auto prev = *this;
            this->m_element++;
            return prev;
        }
        constexpr BidirectionalIterator& operator--()
        {
            this->m_element--;
            return *this;
        }
        constexpr BidirectionalIterator operator--(int)
        {
            auto prev = *this;
            this->m_element--;
            return prev;
        }
    };
}

using neo::BidirectionalIterator;
using neo::ForwardIterator;
using neo::Iterator;

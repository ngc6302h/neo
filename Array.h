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
#include "Assert.h"
#include "Iterator.h"
#include "Span.h"
#include "TypeTraits.h"
#include "Types.h"

namespace neo
{
    template<typename T, size_t Size>
    struct Array
    {
        using BidIt = BidirectionalIterator<T*, DefaultIteratorContainer<T*>>;
        using ConstBidIt = BidirectionalIterator<const T*, DefaultIteratorContainer<const T*>>;

        constexpr ~Array() = default;
        
        constexpr Array& operator=(const Array& other)
        {
            if (this == &other)
                return *this;

            for (size_t i = 0; i < Size; i++)
            {
                m_storage[i] = other.m_storage[i];
            }
        }

        constexpr Array& operator=(Array&& other)
        {
            if (this == &other)
                return *this;

            for (size_t i = 0; i < Size; i++)
            {
                m_storage[i] = move(other.m_storage[i]);
            }
        }

        constexpr T& at(size_t index)
        {
            VERIFY(index < Size);
            return m_storage[index];
        }

        constexpr const T& at(size_t index) const
        {
            VERIFY(index < Size);
            return m_storage[index];
        }

        constexpr T& operator[](size_t index) const
        {
            return at(index);
        }

        constexpr const T& operator[](size_t index)
        {
            return at(index);
        }

        constexpr Span<T> span()
        {
            return { &m_storage, Size };
        }

        constexpr Span<const T> span() const
        {
            return { &m_storage, Size };
        }

        constexpr size_t size() const
        {
            return Size;
        }

        constexpr BidIt begin()
        {
            return BidIt((DecayArray<T[Size]>)&m_storage);
        }

        constexpr ConstBidIt begin() const
        {
            return ConstBidIt((DecayArray<T[Size]>)&m_storage);
        }

        constexpr BidIt end()
        {
            return BidIt(&m_storage[Size]);
        }

        constexpr ConstBidIt end() const
        {
            return ConstBidIt(&m_storage[Size]);
        }

        template<typename = EnableIf<IsInequalityComparable<T>>>
        constexpr bool operator==(const Array& other) const
        {
            return span() == other.span();
        }

        template<typename = EnableIf<IsInequalityComparable<T>>>
        constexpr bool operator!=(const Array& other) const
        {
            return span() != other.span();
        }

        T m_storage[Size] { 0 };
    };
}
using neo::Array;

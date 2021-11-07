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
#include "IterableUtil.h"
#include "Iterator.h"
#include "Span.h"
#include "TypeTags.h"
#include "TypeTraits.h"
#include "Types.h"

namespace neo
{
    template<typename T, size_t Size>
    struct Array
    {
        using type = T;
        using ArrayIterator = Iterator<Array>;
        using ConstantArrayIterator = Iterator<const Array>;

        constexpr ~Array() = default;

        //constexpr Array() = default;

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

        [[nodiscard]] constexpr T& at(size_t index)
        {
            VERIFY(index < Size);
            return m_storage[index];
        }

        [[nodiscard]] constexpr const T& at(size_t index) const
        {
            VERIFY(index < Size);
            return m_storage[index];
        }

        [[nodiscard]] constexpr T& operator[](size_t index)
        {
            return at(index);
        }

        [[nodiscard]] constexpr const T& operator[](size_t index) const
        {
            return at(index);
        }

        [[nodiscard]] constexpr Span<T> span()
        {
            return { &m_storage, Size };
        }

        [[nodiscard]] constexpr Span<const T> span() const
        {
            return { &m_storage, Size };
        }

        [[nodiscard]] constexpr ArrayIterator begin()
        {
            return BidIt(*this);
        }

        [[nodiscard]] constexpr ConstantArrayIterator begin() const
        {
            return ConstBidIt(*this);
        }

        [[nodiscard]] constexpr ArrayIterator end()
        {
            return BidIt(*this, Size);
        }

        [[nodiscard]] constexpr ConstantArrayIterator end() const
        {
            return ConstBidIt(*this, Size);
        }

        template<typename K, typename = EnableIf<InequalityComparable<K>>>
        [[nodiscard]] constexpr bool operator==(const Array& other) const
        {
            return span() == other.span();
        }
    
        template<typename K, typename = EnableIf<InequalityComparable<K>>>
        [[nodiscard]] constexpr bool operator!=(const Array& other) const
        {
            return span() != other.span();
        }

        template<size_t Index>
        constexpr T& get()
        {
            return m_storage[Index];
        }

        template<size_t Index>
        constexpr const T& get() const
        {
            return m_storage[Index];
        }

        template<typename TPredicate>
        requires CallableWithReturnType<TPredicate, bool, const T&>
        [[nodiscard]] Vector<ReferenceWrapper<T>> filter(TPredicate&& predicate) const
        {
            return neo::filter(*this, predicate);
        }

        template<typename TSelector>
        requires Callable<TSelector, T> &&(!IsSame<ReturnType<TSelector, T>, void>)
            [[nodiscard]] Vector<ReferenceWrapper<ReturnType<TSelector, const T&>>> select(TSelector&& selector) const
        {
            return neo::select(*this, selector);
        }

        template<typename U, typename TComparerFunc>
        requires CallableWithReturnType<TComparerFunc, bool, const T&, const U&>
        [[nodiscard]] constexpr bool contains(const U& what, TComparerFunc comparer = DefaultEqualityComparer<const T&>) const
        {
            return neo::contains(*this, what, comparer);
        }
    
        template<typename Identity = IdentityType<T>>
        [[nodiscard]] constexpr bool contains(const Identity& what) const requires InequalityComparable<Identity>
        {
            return neo::contains(*this, what, DefaultEqualityComparer<T>);
        }

        T m_storage[Size];
    };
}
using neo::Array;

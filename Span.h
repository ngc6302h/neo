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
#include "Concepts.h"
#include "Iterator.h"
#include "IterableUtil.h"

namespace neo
{
    template<typename T>
    class Span : IterableExtensions<Span<T>, RemoveReferenceWrapper<T>>
    {
    public:
        using SpanIterator = Iterator<Span>;
        using ConstantSpanIterator = Iterator<const Span>;

        constexpr Span() = delete;

        constexpr Span(T* data, size_t size) :
            m_data(data), m_size(size)
        {
        }

        [[nodiscard]] constexpr T* data() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr Span slice(size_t start) const
        {
            VERIFY(start < m_size);
            return { m_data + start, m_size - start };
        }

        [[nodiscard]] constexpr Span slice(size_t start, size_t length)
        {
            VERIFY(start < m_size);
            VERIFY(start + length < m_size);
            return { m_data + start, length };
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        template<typename U>
        [[nodiscard]] constexpr Span<U> as() const
        {
            VERIFY(sizeof(T) % sizeof(U) == 0);
            VERIFY(size() >= sizeof(U));
            return { reinterpret_cast<U*>(m_data), (size() * sizeof(T)) / sizeof(U) };
        }

        [[nodiscard]] constexpr Span<const T> as_readonly() const
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr T& operator[](size_t index)
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        [[nodiscard]] constexpr const T& operator[](size_t index) const
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        [[nodiscard]] constexpr SpanIterator begin()
        {
            return SpanIterator(*this);
        }

        [[nodiscard]] constexpr ConstantSpanIterator begin() const
        {
            return ConstantSpanIterator(*this);
        }

        [[nodiscard]] constexpr SpanIterator end()
        {
            return SpanIterator(*this, m_size);
        }

        [[nodiscard]] constexpr ConstantSpanIterator end() const
        {
            return ConstantSpanIterator(*this, m_size);
        }

        template<typename>
        [[nodiscard]] constexpr bool operator==(const Span other) const requires InequalityComparable<T>
        {
            if (m_size != other.m_size)
                return false;

            for (size_t i = 0; i < m_size; i++)
            {
                if (m_data[i] != other.m_data[i])
                    return false;
            }
            return true;
        }

    private : T* m_data { nullptr };
        size_t m_size {};
    };
}
using neo::Span;

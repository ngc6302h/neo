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
    template<typename T /*, typename Allocator = neo::DefaultAllocator */>
    class Vector
    {
        static constexpr size_t DEFAULT_SIZE { 16 };

    public:
        using BidIt = BidirectionalIterator<T*>;
        constexpr Vector() :
            m_capacity(DEFAULT_SIZE)
        {
            m_data = new T[DEFAULT_SIZE];
        }

        ~Vector()
        {
            delete[] m_data;
        }

        explicit constexpr Vector(size_t initial_capacity, bool resize_to_capacity = false)
        {
            VERIFY(initial_capacity > 0);
            m_data = new T[initial_capacity];
            m_capacity = initial_capacity;
            m_size = resize_to_capacity ? initial_capacity : 0;
        }

        constexpr Vector(const Vector& other) :
            m_capacity(other.m_capacity), m_size(other.m_size)
        {
            m_data = new T[other.m_capacity];
            if constexpr (IsTriviallyCopyable<T>)
                __builtin_memcpy(m_data, other.m_data, other.m_size * sizeof(T));
            else
            {
                for (size_t i = 0; i < m_size; i++)
                    m_data[i] = other.m_data[i];
            }
        }

        constexpr Vector(Vector&& other) :
            m_capacity(other.m_capacity), m_size(other.m_size)
        {
            m_data = other.m_data;
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_data = nullptr;
        }

        explicit constexpr Vector(const Span<T>& other) :
            m_capacity(other.size()), m_size(other.size())
        {
            VERIFY(other.size() > 0);
            m_data = new T[m_size];
            if constexpr (IsTriviallyCopyable<T>)
                __builtin_memcpy(m_data, other.m_data, other.m_size * sizeof(T));
            else
            {
                for (size_t i = 0; i < m_size; i++)
                    m_data[i] = other.m_data[i];
            }
        }

        constexpr Vector& operator=(const Vector& other)
        {
            if (&other == this)
                return *this;

            delete[] m_data;
            m_size = other.size();
            m_capacity = other.capacity();
            m_data = new T[m_size];
            if constexpr (IsTriviallyCopyable<T>)
                __builtin_memcpy(m_data, other.m_data, other.m_size * sizeof(T));
            else
            {
                for (size_t i = 0; i < m_size; i++)
                    m_data[i] = other.m_data[i];
            }
        }

        constexpr Vector& operator=(Vector&& other)
        {
            if (&other == this)
                return *this;

            delete[] m_data;
            m_size = other.size();
            m_capacity = other.capacity();
            m_data = other.m_data;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_data = nullptr;
            return *this;
        }

        constexpr void append(const T& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = e;
        }

        constexpr void append(T&& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = move(e);
        }

        template<typename... Args>
        constexpr T& construct(Args... args)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size] = T(forward<Args>(args)...);
            return m_data[m_size++];
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return m_capacity;
        }

        constexpr void resize(size_t new_size)
        {
            VERIFY(new_size > 0);
            if (new_size <= m_size)
            {
                m_size = new_size;
                return;
            }

            T* new_buffer = new T[new_size];
            if constexpr (IsTriviallyCopyable<T>)
                __builtin_memcpy(new_buffer, m_data, m_size * sizeof(T));
            else
            {
                for (size_t i = 0; i < min(m_size, new_size); i++)
                {
                    new_buffer[i] = move(m_data[i]);
                }
            }
            delete[] m_data;
            m_data = new_buffer;
            if (new_size > m_capacity)
                m_capacity = new_size;
            m_size = new_size;
        }

        constexpr void ensure_capacity(size_t new_capacity)
        {
            VERIFY(new_capacity > 0);
            if (m_capacity < new_capacity)
            {
                size_t current_size = m_size;
                resize(new_capacity);
                m_size = current_size;
            }
        }

        [[nodiscard]] constexpr T& operator[](size_t index) const
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        [[nodiscard]] constexpr T* data() const
        {
            return m_data;
        }

        constexpr void shrink_to_fit()
        {
            VERIFY(m_size > 0);
            resize(m_size);
        }

        [[nodiscard]] constexpr Span<T> span() const
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr Span<const T> readonly_span() const
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr const BidIt cbegin() const
        {
            return BidIt(m_data);
        }

        [[nodiscard]] constexpr BidIt begin()
        {
            return BidIt(m_data);
        }

        [[nodiscard]] constexpr const BidIt cend() const
        {
            return BidIt(m_data + m_size);
        }

        [[nodiscard]] constexpr BidIt end()
        {
            return BidIt(m_data + m_size);
        }

    private:
        T* m_data { nullptr };
        size_t m_capacity { 0 };
        size_t m_size { 0 };
    };
}
using neo::Vector;

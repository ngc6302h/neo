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
#include "Concepts.h"
#include <malloc.h>

namespace neo
{
    template<typename T /*, typename Allocator = neo::DefaultAllocator */>
    class Vector : public IContainer<Vector, T>, public IterableExtensions<Vector<T>, RemoveReferenceWrapper<T>>
    {
        static constexpr size_t DEFAULT_SIZE { 16 };

    public:
        using BidIt = BidirectionalIterator<T*>;
        using ConstBidIt = BidirectionalIterator<const T*>;
        constexpr Vector() :
            m_capacity(DEFAULT_SIZE)
        {
            allocate(DEFAULT_SIZE);
        }

        ~Vector()
        {
            clean();
            deallocate();
        }

        explicit constexpr Vector(size_t initial_capacity, bool resize_to_capacity = false) :
            m_capacity(initial_capacity), m_size(resize_to_capacity ? initial_capacity : 0)
        {
            VERIFY(initial_capacity > 0);
            allocate(initial_capacity);
        }

        constexpr Vector(const Vector& other) :
            m_capacity(other.m_capacity), m_size(other.m_size)
        {
            VERIFY(other.m_size > 0);
            allocate(other.m_size);
            for (size_t i = 0; i < m_size; i++)
            {
                m_data[i] = other.m_data[i];
            }
        }

        constexpr Vector(Vector&& other) :
            m_data(other.m_data), m_capacity(other.m_capacity), m_size(other.m_size)
        {
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_data = nullptr;
        }

        explicit constexpr Vector(const Span<T>& other) :
            m_capacity(other.size()), m_size(other.size())
        {
            VERIFY(other.size() > 0);
            allocate(other.m_size);
            for (size_t i = 0; i < m_size; i++)
            {
                m_data[i] = other.m_data[i];
            }
        }

        constexpr Vector& operator=(const Vector& other)
        {
            if (&other == this)
                return *this;

            m_capacity = other.capacity();
            m_size = other.size();
            clean();
            deallocate();
            allocate(other.m_size);
            for (size_t i = 0; i < m_size; i++)
                m_data[i] = other.m_data[i];
            return *this;
        }

        constexpr Vector& operator=(Vector&& other)
        {
            if (&other == this)
                return *this;

            clean();
            deallocate();
            m_size = other.size();
            m_capacity = other.capacity();
            m_data = other.m_data;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_data = nullptr;
            return *this;
        }

        constexpr void append(const RemoveReferenceWrapper<T>& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = e;
        }
    
        template<typename> requires MoveAssignable<T>
        constexpr void append(RemoveReferenceWrapper<T>&& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = move(e);
        }

        template<typename... Args>
        constexpr T& construct(Args... args)
        {
            ensure_capacity(m_size + 1);
            if constexpr (IsTrivial<T>)
                m_data[m_size] = T { forward<Args>(args)... };
            else
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

        constexpr void change_capacity(size_t new_capcity)
        {
            VERIFY(new_capcity > 0);
            
            T* new_buf = (T*) calloc(new_capcity, sizeof(T));
            for (size_t i = 0; i < m_size; i++)
            {
                if constexpr(MoveAssignable<T>)
                    new_buf[i] = move(m_data[i]);
                else
                    new_buf[i] = m_data[i];
            }
            clean();
            deallocate();
            m_data = new_buf;
            m_capacity = new_capcity;
        }

        constexpr void ensure_capacity(size_t new_capacity)
        {
            VERIFY(new_capacity > 0);
            if (m_capacity < new_capacity)
            {
                change_capacity(new_capacity);
            }
        }

        [[nodiscard]] constexpr T& at(size_t index)
        {
            VERIFY(index < m_size);
            return m_data[index];
        }
        [[nodiscard]] constexpr const T& at(size_t index) const
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        [[nodiscard]] constexpr const T& first() const
        {
            return at(0);
        }

        [[nodiscard]] constexpr const T& last() const
        {
            return at(m_size - 1);
        }

        [[nodiscard]] constexpr T& first()
        {
            return at(0);
        }

        [[nodiscard]] constexpr T& last()
        {
            return at(m_size - 1);
        }

        [[nodiscard]] constexpr T& operator[](size_t index)
        {
            return at(index);
        }

        [[nodiscard]] constexpr const T& operator[](size_t index) const
        {
            return at(index);
        }

        [[nodiscard]] constexpr T* data()
        {
            return m_data;
        }

        [[nodiscard]] constexpr const T* data() const
        {
            return m_data;
        }

        constexpr void shrink_to_fit()
        {
            VERIFY(m_size > 0);
            change_capacity(m_size);
        }

        [[nodiscard]] constexpr Span<T> span()
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr Span<const T> span() const
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr ConstBidIt begin() const
        {
            return ConstBidIt(m_data);
        }

        [[nodiscard]] constexpr BidIt begin()
        {
            return BidIt(m_data);
        }

        [[nodiscard]] constexpr ConstBidIt end() const
        {
            return ConstBidIt(m_data + m_size);
        }

        [[nodiscard]] constexpr BidIt end()
        {
            return BidIt(m_data + m_size);
        }

    private:
        constexpr void clean()
        {
            for (size_t i = 0; i < m_size; i++)
                m_data[i].~T();
        }

        constexpr void allocate(size_t size)
        {
            m_data = (T*) calloc(size, sizeof(T));
        }

        constexpr void deallocate()
        {
            free(m_data);
        }

        T* m_data { nullptr };
        size_t m_capacity { 0 };
        size_t m_size { 0 };
    };
}
using neo::Vector;

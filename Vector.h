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
        using VectorIterator = Iterator<Vector>;
        using VectorConstantIterator = Iterator<const Vector>;
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

        constexpr Vector(const Vector& other)
        {
            clean();
            ensure_capacity(max(1UL, other.size()));
            Copy(other.size(), other.data(), m_data);
            m_size = other.m_size;
    
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
            clean();
            ensure_capacity(max(1UL, other.size()));
            Copy(other.size(), other.data(), m_data);
            m_size = other.size();
        }

    private:
        template<size_t ItemsLeft, typename TFirst, typename... TRest>
        constexpr void initializer_list_copy_helper(size_t index, TFirst const& first, TRest const&... rest)
        {
            if constexpr(ItemsLeft != 0)
            {
                m_data[index] = first;
                if constexpr(ItemsLeft-1 != 0)
                    initializer_list_copy_helper<ItemsLeft-1, TRest...>(index+1, rest...);
            }
        }

    public:
        template<typename... Ts>
        constexpr Vector(Ts const&... items) : m_capacity(sizeof...(Ts)), m_size(sizeof...(Ts))
        {
            allocate(sizeof...(items));
            initializer_list_copy_helper<sizeof...(Ts), Ts...>(0, items...);
        }

        constexpr Vector& operator=(const Vector& other)
        {
            if (&other == this)
                return *this;
    
            clean();
            ensure_capacity(max(1UL, other.size()));
            Copy(other.size(), other.data(), m_data);
            m_size = other.size();
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
    
        template<typename> requires MoveAssignable<T>
        constexpr void append(RemoveReferenceWrapper<T>&& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = move(e);
        }
    
        constexpr void append(const RemoveReferenceWrapper<T>& e)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size++] = e;
        }
        
        constexpr void append(Span<RemoveReferenceWrapper<T>> const& items)
        {
            ensure_capacity(m_size+items.size());
            Copy(items.size(), items.data(), m_data+m_size);
            m_size+=items.size();
        }
    
        constexpr void append(Span<RemoveReferenceWrapper<T>>&& items)
        {
            ensure_capacity(m_size+items.size());
            TypedMove(items.size(), items.data(), m_data+m_size);
            m_size+=items.size();
        }
        
        constexpr void remove_at(size_t index)
        {
            VERIFY(index < m_size);
            if (index != m_size-1)
            {
                if constexpr(IsTrivial<T>)
                    OverlappingUntypedCopy(m_size-index, m_data+index+1, m_data+index);
                else
                    TypedMove(m_size-index-1, m_data+index+1, m_data+index);
            }
            m_size--;
        }

        template<typename... Args>
        constexpr T& construct(Args... args)
        {
            ensure_capacity(m_size + 1);
            m_data[m_size] = T { forward<Args>(args)... };
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

        constexpr void change_capacity(size_t new_capacity)
        {
            VERIFY(new_capacity > 0);
            
            T* new_buf = (T*) calloc(new_capacity, sizeof(T));
            if constexpr (IsTriviallyCopyable<T>)
                UntypedCopy(m_size, m_data, new_buf);
            else
                TypedMove(m_size, m_data, new_buf);
            clean();
            deallocate();
            m_data = new_buf;
            m_capacity = new_capacity;
        }

        constexpr void ensure_capacity(size_t needed_capacity)
        {
            VERIFY(needed_capacity > 0);
            if (m_capacity < needed_capacity)
            {
                change_capacity(needed_capacity*2);
            }
        }

        [[nodiscard]] constexpr T& at(size_t index)
        {
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"
    
            VERIFY(index < m_size);
            return m_data[index];
//#pragma GCC diagnostic pop
        }
        [[nodiscard]] constexpr const T& at(size_t index) const
        {
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"
            VERIFY(index < m_size);
            return m_data[index];
//#pragma GCC diagnostic pop
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
        
        constexpr T take_first()
        {
            T value = move(first());
            m_size--;
            TypedMove(m_size, m_data+1, m_data);
            return move(value);
        }
        
        constexpr T take_last()
        {
            T value = move(last());
            last().~T();
            m_size--;
            return move(value);
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
        
        constexpr void clear()
        {
            clean();
            m_size = 0;
        }

        [[nodiscard]] constexpr Span<T> span()
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr Span<const T> span() const
        {
            return { m_data, m_size };
        }

        [[nodiscard]] constexpr VectorConstantIterator begin() const
        {
            return VectorConstantIterator(*this);
        }

        [[nodiscard]] constexpr VectorIterator begin()
        {
            return VectorIterator(*this);
        }

        [[nodiscard]] constexpr VectorConstantIterator end() const
        {
            return VectorConstantIterator(*this, m_size);
        }

        [[nodiscard]] constexpr VectorIterator end()
        {
            return VectorIterator(*this, m_size);
        }

    private:
        constexpr void clean()
        {
            if constexpr(!IsTrivial<T>)
            {
                for (size_t i = 0; i < m_size; i++)
                    m_data[i].~T();
            }
        }

        //size is number of T elements
        constexpr void allocate(size_t size)
        {
            m_data = (T*) calloc(size, sizeof(T));
        }

        constexpr void deallocate()
        {
            free(m_data);
            m_data = nullptr;
        }

        T* m_data { nullptr };
        size_t m_capacity { 0 };
        size_t m_size { 0 };
    };
}
using neo::Vector;

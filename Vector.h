/*
    Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "Util.h"
#include "Assert.h"
#include "IterableUtil.h"
#include "Iterator.h"
#include "Span.h"
#include "TypeTags.h"
#include "TypeTraits.h"
#include "Types.h"
#include "Concepts.h"
#include "New.h"
#include "SmartPtr.h"
#include "Memory.h"

namespace neo
{

    namespace detail
    {
        template<typename T, size_t InlineStorage>
        struct VectorInlineStorage
        {
            u8 m_untyped_inline_storage[InlineStorage * sizeof(T)];
        };

        template<typename T>
        struct VectorInlineStorage<T, 0>
        {
        };
    }

    template<typename T>
    class Vector : public IterableExtensions<Vector<T>, RemoveReferenceWrapper<T>>
    {
    public:
        using type = T;
        using iterator = Iterator<Vector>;
        using const_iterator = Iterator<const Vector>;
        static constexpr size_t default_capacity { 16 };

        T* allocate_space(size_t capacity)
        {
            T* storage = (T*)MallocAllocator::allocate(capacity * sizeof(T));
            ENSURE(storage != nullptr);
            return storage;
        }

        Vector() :
            m_capacity(default_capacity)
        {
            m_storage = allocate_space(default_capacity);
        }

        ~Vector()
        {
            clear();
            MallocAllocator::deallocate(m_storage);
            m_storage = nullptr;
            m_capacity = 0;
            m_size = 0;
        }

        template<ConvertibleTo<T>... items>
        Vector(items&&... items_)
        {
            ensure_capacity(sizeof...(items));
            (append(items_), ...);
        }

        Vector(Vector&& other)
        {
            if (this == &other)
                return;

            clear();

            m_storage = other.m_storage;
            other.m_storage = nullptr;
            m_size = other.m_size;
            other.m_size = 0;
            m_capacity = other.m_capacity;
            other.m_capacity = 0;
        }

        Vector(Vector const& other)
        {
            if (this == &other)
                return;

            clear();
            ensure_capacity(other.m_capacity);

            m_size = other.m_size;
            m_capacity = other.m_capacity;

            for (size_t i = 0; i < m_size; i++)
                m_storage[i] = other.m_storage[i];
        }

        Vector& operator=(Vector&& other)
        {
            if (this == &other)
                return *this;

            clear();

            m_storage = other.m_storage;
            other.m_storage = nullptr;
            m_size = other.m_size;
            other.m_size = 0;
            m_capacity = other.m_capacity;
            other.m_capacity = 0;

            return *this;
        }

        Vector& operator=(Vector const& other)
        {
            if (this == &other)
                return *this;

            clear();
            ensure_capacity(other.m_capacity);

            m_size = other.m_size;
            m_capacity = other.m_capacity;

            for (size_t i = 0; i < m_size; i++)
                m_storage[i] = other.m_storage[i];

            return *this;
        }

        void clear()
        {
            for (size_t i = m_size; i-- > 0;)
                destroy_at(i);
        }

        T& operator[](size_t index)
        {
            return m_storage[index];
        }

        T const& operator[](size_t index) const
        {
            return m_storage[index];
        }

        auto begin()
        {
            return iterator { *this };
        }

        auto end()
        {
            return iterator { *this, m_size };
        }

        auto begin() const
        {
            return const_iterator { *this };
        }

        auto end() const
        {
            return const_iterator { *this, m_size };
        }

        T* data()
        {
            return m_storage;
        }

        T const* data() const
        {
            return m_storage;
        }

        Span<T> span()
        {
            return { m_storage, m_size };
        }

        Span<const T> span() const
        {
            return { m_storage, m_size };
        }

        T& first()
        {
            VERIFY(m_size > 0);
            return m_storage[0];
        }

        T const& first() const
        {
            VERIFY(m_size > 0);
            return m_storage[0];
        }

        T& last()
        {
            VERIFY(m_size > 0);
            return m_storage[m_size - 1];
        }

        T& last() const
        {
            VERIFY(m_size > 0);
            return m_storage[m_size - 1];
        }

        T take_first()
        {
            T first_ = std::move(first());
            if (m_size > 1)
            {
                for (size_t i = 0; i < m_size - 1; i++)
                    m_storage[i] = m_storage[i + 1];
            }
            m_size--;

            return first_;
        }

        T take_last()
        {
            T last_ = std::move(last());
            m_size--;
            return last_;
        }

        size_t size() const
        {
            return m_size;
        }

        bool is_empty() const
        {
            return m_size == 0;
        }

        size_t capacity() const
        {
            return m_capacity;
        }

        void destroy_at(size_t index)
        {
            VERIFY(index < m_size);
            if (m_size > 1)
            {
                for (size_t i = index; i < m_size - 1; i++)
                    m_storage[index] = std::move(m_storage[i + 1]);
            }
            else
            {
                m_storage[index].~T();
            }
            m_size--;
        }

        void append(T const& item)
        {
            if (m_size + 1 > m_capacity)
                ensure_capacity(m_capacity * 2);

            m_storage[m_size] = item;
            m_size++;
        }

        void append(T&& item)
        {
            if (m_size + 1 > m_capacity)
                ensure_capacity(m_capacity * 2);

            m_storage[m_size] = std::move(item);
            m_size++;
        }

        template<typename... TArgs>
        T& construct(TArgs&&... args)
        {
            append(T { forward<TArgs>(args)... });
            return last();
        }

        T& at(size_t index)
        {
            return (*this)[index];
        }

        T const& at(size_t index) const
        {
            return (*this)[index];
        }

        void change_capacity(size_t new_capacity)
        {
            T* new_storage = allocate_space(new_capacity);
            for (size_t i = 0; i < m_size; i++)
                new_storage[i] = std::move(m_storage[i]);
            MallocAllocator::deallocate(m_storage);
            m_storage = new_storage;
        }

        void ensure_capacity(size_t needed_capacity)
        {
            if (m_capacity < needed_capacity)
                change_capacity(needed_capacity);
        }

        void change_size(size_t needed_size)
        {
            if (needed_size < m_capacity)
                ensure_capacity(needed_size);
            m_size = needed_size;
        }

        bool operator==(Vector const& other) const
        {
            if (this == &other)
                return true;

            if (m_size != other.m_size)
                return false;

            for (size_t i = 0; i < m_size; i++)
            {
                if (m_storage[i] != other.m_storage[i])
                    return false;
            }

            return true;
        }

        bool operator!=(Vector const& other) const
        {
            return !((*this) != other);
        }

        template<IteratorLike TIterator>
        static Vector from_range(TIterator begin, TIterator end)
        {
            Vector vec;
            while (begin != end)
            {
                vec.append(*begin++);
            }

            return vec;
        }

        static Vector create_with_capacity(size_t capacity)
        {
            Vector vec;
            vec.ensure_capacity(capacity);
            return vec;
        }

    private:
        T* m_storage { nullptr };
        size_t m_size { 0 };
        size_t m_capacity { 0 };
    };

    template<typename T>
    using OwnPtrVector = Vector<OwnPtr<T>>;

    template<typename T>
    using NullableOwnPtrVector = Vector<NullableOwnPtr<T>>;

    template<typename T>
    using RefPtrVector = Vector<RefPtr<T>>;

    template<typename T>
    using NullableRefPtrVector = Vector<NullableRefPtr<T>>;

    // todo: fix vector for smart ptrs
}
using neo::NullableOwnPtrVector;
using neo::NullableRefPtrVector;
using neo::OwnPtrVector;
using neo::RefPtrVector;
using neo::Vector;

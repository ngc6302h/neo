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
#include "Types.h"
#include "Assert.h"
#include "Span.h"
#include "Iterator.h"
#include "New.h"
#include "Optional.h"

namespace neo
{
    template<typename T>
    class Buffer : public IterableExtensions<Buffer<T>, T>
    {
    public:
        using type = T;
        using iterator = Iterator<Buffer>;
        using const_iterator = Iterator<const Buffer>;

        static constexpr Optional<Buffer> create_uninitialized(size_t size)
        {
            auto ptr = (T*)__builtin_malloc(sizeof(T) * size);
            if (ptr == nullptr)
                return {};
            return Buffer(ptr, size);
        }

        template<typename... Ts>
        static constexpr Optional<Buffer> create_initialized(size_t size, Ts... args)
        {
            auto ptr = (T*)__builtin_malloc(sizeof(T) * size);
            if (ptr == nullptr)
                return {};
            Buffer mem(ptr, size);
            for (size_t i = 0; i < size; ++i)
                new (mem.m_data + i) T(forward<Ts>(args)...);
            return mem;
        }

        static constexpr Optional<Buffer> create_zero_initialized(size_t size)
        {
            auto ptr = (T*)__builtin_malloc(sizeof(T) * size);
            if (ptr == nullptr)
                return {};
            Buffer mem(ptr, size);
            __builtin_memset((char*)mem.m_data, 0, size * sizeof(T));
            return mem;
        }

        constexpr ~Buffer()
        {
            if constexpr (!IsTriviallyDestructible<T>)
            {
                for (size_t i = 0; i < m_size; ++i)
                    m_data[i].~T();
            }
            __builtin_free(m_data);
        }

        constexpr Buffer(Buffer const& other)
        {
            m_size = other.m_size;
            auto ptr = (T*)__builtin_malloc(sizeof(T) * other.m_size);
            VERIFY(ptr != nullptr);
            m_data = ptr;
            Copy(other.m_size, other.m_data, m_data);
        }

        constexpr Buffer(Buffer&& other) :
            m_data(other.m_data), m_size(other.m_size)
        {
            other.m_data = nullptr;
            other.m_size = 0;
        }

        constexpr Buffer& operator=(Buffer const& other)
        {
            if (this == &other)
                return *this;

            this->~Buffer();
            new (this) Buffer(other);

            return *this;
        }

        constexpr Buffer& operator=(Buffer&& other)
        {
            if (this == &other)
                return *this;

            this->~Buffer();
            new (this) Buffer(move(other));

            return *this;
        }

        constexpr T& operator[](size_t index)
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        constexpr T const& operator[](size_t index) const
        {
            VERIFY(index < m_size);
            return m_data[index];
        }

        constexpr size_t size() const
        {
            return m_size;
        }

        constexpr T* data()
        {
            return m_data;
        }

        constexpr T const* data() const
        {
            return m_data;
        }

        constexpr Span<T> span()
        {
            return { m_data, m_size };
        }

        constexpr Span<const T> span() const
        {
            return { m_data, m_size };
        }

        constexpr void destroy_all()
        {
            for (size_t i = 0; i < m_size; ++i)
                m_data[i].~T();
        }

        constexpr iterator begin()
        {
            return { *this };
        }

        constexpr const_iterator begin() const
        {
            return { *this };
        }

        constexpr iterator end()
        {
            return { *this, m_size };
        }

        constexpr const_iterator end() const
        {
            return { *this, m_size };
        }

    private:
        constexpr explicit Buffer(T* backing, size_t size) :
            m_size(size), m_data(backing)
        {
        }

        T* m_data { nullptr };
        size_t m_size { 0 };
    };
}
using neo::Buffer;

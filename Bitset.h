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
#include <Types.h>
#include <Optional.h>

namespace neo
{
    template<size_t InlineStorage = 0>
    requires(InlineStorage % 8 == 0) class Bitset
    {
    public:
        constexpr Bitset() = delete;

        explicit constexpr Bitset(size_t size, bool initial_state) :
            m_size(size)
        {
            if (size > InlineStorage)
                m_storage = new u8[size / 8 + (size % 8 > 0) - sizeof(InlineStorage)];
            if (size > InlineStorage)
            {
                __builtin_memset(m_inline_storage, initial_state ? 0xFF : 0, sizeof(InlineStorage));
                __builtin_memset(m_storage, initial_state ? 0xFF : 0, size / 8 + (size % 8 > 0) - sizeof(InlineStorage));
            }
            else
            {
                __builtin_memset(m_inline_storage, initial_state ? 0xFF : 0, sizeof(InlineStorage));
            }
        }

        constexpr Bitset(Bitset const& other) :
            m_size(other.m_size)
        {
            if constexpr (sizeof(other.m_inline_storage) != 0)
                __builtin_memcpy(m_inline_storage, other.m_inline_storage, sizeof(other.m_inline_storage));
            if (other.m_size > sizeof(other.m_inline_storage))
            {
                m_storage = new u8[other.m_size - sizeof(other.m_inline_storage)];
                __builtin_memcpy(m_storage, other.m_storage, other.m_size - sizeof(other.m_inline_storage));
            }
        }

        constexpr Bitset(Bitset&& other) :
            m_size(other.m_size), m_storage(other.m_storage)
        {
            other.m_storage = nullptr;
            other.m_size = 0;
            if constexpr (sizeof(other.m_inline_storage) != 0)
                __builtin_memcpy(m_inline_storage, other.m_inline_storage, sizeof(other.m_inline_storage));
        }
        
        constexpr Bitset operator=(Bitset const& other)
        {
            if (this == &other) [[unlikely]]
                return *this;
            
            m_size = other.m_size;
            if constexpr (sizeof(other.m_inline_storage) != 0)
                __builtin_memcpy(m_inline_storage, other.m_inline_storage, sizeof(other.m_inline_storage));
            if (other.m_size > sizeof(other.m_inline_storage))
            {
                m_storage = new u8[other.m_size - sizeof(other.m_inline_storage)];
                __builtin_memcpy(m_storage, other.m_storage, other.m_size - sizeof(other.m_inline_storage));
            }
        }
        
        constexpr Bitset operator=(Bitset&& other)
        {
            if (this == &other) [[unlikely]]
                return *this;
            
            m_storage = other.m_storage;
            m_size = other.m_size;
            if constexpr (sizeof(other.m_inline_storage) != 0)
                __builtin_memcpy(m_inline_storage, other.m_inline_storage, sizeof(other.m_inline_storage));
            
            other.m_storage = nullptr;
            other.m_size = 0;
            
            return *this;
        }

        constexpr bool set(size_t index, bool value)
        {
            u8* where;
            const auto byte_offset = index / 8;
            if constexpr (InlineStorage != 0)
            {
                if (byte_offset < InlineStorage)
                    where = m_inline_storage + byte_offset;
                else
                    where = m_storage + byte_offset - sizeof(m_inline_storage);
            }
            else
                where = m_storage + byte_offset;

            bool old = (*where >> (index % 8)) & 1;
            if (value)
                *where |= value << (index % 8);
            else
                *where &= ~(value << (index % 8));

            return old;
        }

        [[nodiscard]] constexpr bool get(size_t index) const
        {
            u8* where;
            const auto byte_offset = index / 8;
            if constexpr (InlineStorage != 0)
            {
                if (byte_offset < InlineStorage)
                    where = m_inline_storage + byte_offset;
                else
                    where = m_storage + byte_offset - sizeof(m_inline_storage);
            }
            else
                where = m_storage + byte_offset;

            return (*where >> (index % 8)) & 1;
        }

        constexpr bool operator[](size_t index)
        {
            return get(index);
        }

        [[nodiscard]] constexpr bool all_set() const
        {
            if constexpr (InlineStorage != 0)
            {
                for (size_t i = 0; i < sizeof(m_inline_storage); ++i)
                    if (m_inline_storage[i] != 0xFF)
                        return false;
            }
            for (size_t i = 0; i < m_size - InlineStorage; ++i)
                if (m_storage[i] != 0xFF)
                    return false;

            return true;
        }

        [[nodiscard]] constexpr bool any_set() const
        {
            if constexpr (InlineStorage != 0)
            {
                for (size_t i = 0; i < sizeof(m_inline_storage); ++i)
                    if (m_inline_storage[i] != 0x00)
                        return true;
            }
            for (size_t i = 0; i < m_size - InlineStorage; ++i)
                if (m_storage[i] != 0x00)
                    return true;

            return false;
        }

        [[nodiscard]] constexpr Optional<size_t> find_first_set() const
        {
            size_t index = 0;
            if constexpr (InlineStorage != 0)
            {
                for (size_t i = 0; i < sizeof(m_inline_storage); ++i)
                {
                    if (m_inline_storage[i] != 0x00)
                    {
                        for (int offset = 0; offset < 8; ++offset)
                            if ((m_inline_storage[i] >> offset & 1))
                                return index + offset;
                    }
                    else
                        index += 8;
                }
            }
            if constexpr (InlineStorage < m_size)
            {
                for (size_t i = 0; i < m_size - sizeof(m_inline_storage); ++i)
                {
                    if (m_storage[i] != 0x00)
                    {
                        for (int offset = 0; offset < 8; ++offset)
                            if ((m_storage[i] >> offset & 1))
                                return index + offset;
                    }
                    else
                        index += 8;
                }
            }
        }

        [[nodiscard]] constexpr size_t find_first_not_set() const
        {
            size_t index = 0;
            if constexpr (InlineStorage != 0)
            {
                for (size_t i = 0; i < sizeof(m_inline_storage); ++i)
                {
                    if (m_inline_storage[i] != 0xFF)
                    {
                        for (int offset = 0; offset < 8; ++offset)
                            if ((m_inline_storage[i] >> offset & 1) == 0)
                                return index + offset;
                    }
                    else
                        index += 8;
                }
            }
            if constexpr (InlineStorage < m_size)
            {
                for (size_t i = 0; i < m_size - sizeof(m_inline_storage); ++i)
                {
                    if (m_storage[i] != 0xFF)
                    {
                        for (int offset = 0; offset < 8; ++offset)
                            if ((m_storage[i] >> offset & 1) == 0)
                                return index + offset;
                    }
                    else
                        index += 8;
                }
            }
        }

    private:
        size_t m_size { 0 };
        u8* m_storage { nullptr };
        u8 m_inline_storage[InlineStorage / 8];
    };
}
using neo::Bitset;

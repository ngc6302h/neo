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

#include "Assert.h"
#include "Iterator.h"
#include "Optional.h"
#include "StringIterator.h"
#include "StringView.h"
#include "Types.h"
#include "Vector.h"
#include "Span.h"

namespace neo
{
    class String : public IString<String, StringIterator>
    {
    public:
        using character_type = Utf32Char;
        using storage_type = char;
        using iterator = StringIterator;

        inline String() :
            m_buffer(nullptr), m_byte_length(0) { }
        inline ~String()
        {
            if (sizeof(m_inline) <= m_byte_length && m_buffer != nullptr)
                MallocAllocator::deallocate(m_buffer);
            m_buffer = nullptr;
        }

        inline String(String const& other) :
            m_byte_length(other.m_byte_length)
        {
            if (sizeof(m_inline) > other.m_byte_length)
            {
                __builtin_memcpy(m_inline, other.m_inline, sizeof(m_inline));
            }
            else
            {
				m_buffer = (char*)MallocAllocator::allocate_and_zero(other.m_byte_length + 1);
                m_buffer[other.m_byte_length] = 0;
                __builtin_memcpy(m_buffer, other.m_buffer, other.m_byte_length);
            }
        }

        inline String(String&& other) :
            m_buffer(other.m_buffer), m_byte_length(other.m_byte_length)
        {
            other.m_buffer = nullptr;
            other.m_byte_length = 0;
        }

        inline String(const char* cstring) :
            m_buffer(nullptr)
        {
            size_t length = __builtin_strlen(cstring);
            m_byte_length = length;

            if (sizeof(m_inline) > length)
            {
                __builtin_memcpy(m_inline, cstring, length);
            }
            else
            {
                m_buffer = (char*)MallocAllocator::allocate_and_zero(length + 1);
                m_buffer[length] = 0;
                __builtin_memcpy(m_buffer, cstring, length);
            }
        }

        inline String(const char* cstring, size_t length) :
            m_buffer(nullptr)
        {
            m_byte_length = length;

            if (sizeof(m_inline) > length)
            {
                __builtin_memcpy(m_inline, cstring, length);
            }
            else
            {
                m_buffer = (char*)MallocAllocator::allocate_and_zero(length + 1);
                m_buffer[length] = 0;
                __builtin_memcpy(m_buffer, cstring, length);
            }
        }

        inline String(StringIterator begin, StringIterator end) :
            m_byte_length((size_t)(end.m_current - begin.m_current))
        {
            VERIFY(begin.m_base == end.m_base);
            VERIFY(begin.m_current <= end.m_current);
            new (this) String(begin.m_current, (size_t)(end.m_current - begin.m_current));
        }

        inline String(StringView const& other) :
            m_buffer(nullptr), m_byte_length(other.byte_size())
        {
            if (sizeof(m_inline) > other.byte_size())
            {
                __builtin_memcpy(m_inline, other.data(), other.byte_size());
            }
            else
            {
                m_buffer = (char*)MallocAllocator::allocate_and_zero(other.byte_size() + 1);
                m_buffer[other.byte_size()] = 0;
                __builtin_memcpy(m_buffer, other.span().data(), other.byte_size());
            }
        }

        [[nodiscard]] inline StringView to_view() const
        {
            return { data(), m_byte_length };
        }

        inline operator StringView() const
        {
            return { data(), m_byte_length };
        }

        inline String& operator=(const String& other)
        {
            if (&other == this)
                return *this;

            this->~String();
            new (this) String(other);

            return *this;
        }

        inline String& operator=(String&& other)
        {
            if (&other == this)
                return *this;

            this->~String();
            new (this) String(std::move(other));

            return *this;
        }

        // Size in bytes
        [[nodiscard]] inline size_t byte_size() const
        {
            return m_byte_length;
        }

        [[nodiscard]] inline char const* null_terminated_characters() const
        {
            return data();
        }

        [[nodiscard]] inline storage_type* data() const
        {
            return (char*)(sizeof(m_inline) > m_byte_length ? m_inline : m_buffer);
        }

    private:
        union
        {
            char* m_buffer = nullptr;
            char m_inline[sizeof(char*)];
        };
        size_t m_byte_length { 0 };
    };

    inline String operator+(StringView const& left, StringView const& right)
    {
        String sum(left.data(), left.byte_size() + right.byte_size());
        __builtin_memcpy(sum.data() + left.byte_size(), right.data(), right.byte_size());
        return sum;
    }

    [[nodiscard]] inline String operator""_s(const char* cstring, size_t length)
    {
        return { cstring, length };
    }

#ifndef NEO_DO_NOT_DEFINE_STD
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wliteral-suffix"

    [[nodiscard]] inline String operator""s(const char* cstring, size_t length)
    {
        return String { cstring, length };
    }
    #pragma GCC diagnostic pop
#endif

    template<typename>
    struct StringHasher;

    template<>
    struct StringHasher<String>
    {
        static inline size_t hash(String const& str)
        {
            VERIFY(str.length() != 0);

            char const* data = str.null_terminated_characters();
            size_t size = str.byte_size();
            size_t result = data[size - 1];
            while (size--)
                result += result ^ data[size] ^ (~(result * result + 3241));
            return result;
        }
    };

    template<typename T>
    struct DefaultHasher;

    template<>
    struct DefaultHasher<String>
    {
        static inline size_t hash(const String& str)
        {
            return StringHasher<String>::hash(str);
        }
    };
}
using neo::String;
using neo::operator""_s;
#ifndef NEO_DO_NOT_DEFINE_STD
using neo::operator""s;
#endif
using neo::StringHasher;

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

#include "AsciiString.h"
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
        using character_type = Utf8Char;
        using storage_type = char;
        using iterator = StringIterator;

        constexpr String() = default;
        constexpr ~String()
        {
            delete m_buffer;
            m_buffer = nullptr;
        }

        constexpr String(const String& other) :
            m_byte_length(other.m_byte_length)
        {
            m_buffer = new char[other.m_byte_length + 1];
            m_buffer[other.m_byte_length] = 0;

            __builtin_memcpy(m_buffer, other.m_buffer, other.m_byte_length);
        }

        constexpr String(String&& other) :
            m_buffer(other.m_buffer), m_byte_length(other.m_byte_length)
        {
            other.m_buffer = nullptr;
            other.m_byte_length = 0;
        }

        constexpr String(const AsciiString& other) :
            m_byte_length(other.length())
        {
            m_buffer = new char[other.length() + 1];
            m_buffer[other.length()] = 0;
            m_byte_length = other.length();
            __builtin_memcpy(m_buffer, other.m_buffer, other.length());
        }

        constexpr String(AsciiString&& other) :
            m_byte_length(other.length())
        {
            m_buffer = other.m_buffer;
            m_byte_length = other.m_length;
            other.m_length = 0;
            other.m_buffer = nullptr;
        }

        constexpr String(const char* cstring)
        {
            size_t length = __builtin_strlen(cstring);

            m_buffer = new char[length + 1];
            m_buffer[length] = 0;
            m_byte_length = length;
            __builtin_memcpy(m_buffer, cstring, length);
        }

        constexpr String(const char* cstring, size_t length)
        {
            size_t size = length;

            m_buffer = new char[size + 1];
            m_buffer[size] = 0;
            m_byte_length = size;
            __builtin_memcpy(m_buffer, cstring, size);
        }

        constexpr String(const StringView& other) :
            m_byte_length(other.byte_size())
        {
            m_buffer = new char[other.byte_size() + 1];
            m_buffer[other.byte_size()] = 0;
            __builtin_memcpy(m_buffer, other.span().data(), other.byte_size());
        }

        [[nodiscard]] constexpr StringView to_view() const
        {
            return { m_buffer, m_byte_length };
        }

        constexpr operator StringView() const
        {
            return { m_buffer, m_byte_length };
        }

        constexpr String& operator=(const String& other)
        {
            if (&other == this)
                return *this;

            delete[] m_buffer;
            m_buffer = new char[other.m_byte_length + 1];
            m_buffer[other.m_byte_length] = 0;
            m_byte_length = other.m_byte_length;
            __builtin_memcpy(m_buffer, other.m_buffer, other.m_byte_length);
            return *this;
        }

        constexpr String& operator=(String&& other)
        {
            if (&other == this)
                return *this;

            delete[] m_buffer;
            m_buffer = other.m_buffer;
            other.m_buffer = nullptr;
            m_byte_length = other.m_byte_length;
            other.m_byte_length = 0;
            return *this;
        }

        // Size in bytes
        [[nodiscard]] constexpr size_t byte_size() const
        {
            return m_byte_length;
        }

        [[nodiscard]] constexpr char* null_terminated_characters() const
        {
            return m_buffer;
        }

        [[nodiscard]] constexpr storage_type* data() const
        {
            return m_buffer;
        }

    private:
        char* m_buffer { nullptr };
        size_t m_byte_length { 0 };
    };

    [[nodiscard]] constexpr String operator""_s(const char* cstring, size_t length)
    {
        return String(cstring, length);
    }

    template<typename>
    struct StringHasher;

    template<>
    struct StringHasher<String>
    {
        static constexpr size_t hash(String const& str)
        {
            VERIFY(str.length() != 0);

            char* data = str.null_terminated_characters();
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
        static constexpr size_t hash(const String& str)
        {
            return StringHasher<String>::hash(str);
        }
    };
}
using neo::String;
using neo::operator""_s;
using neo::StringHasher;

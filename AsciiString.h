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

#include "AsciiStringView.h"
#include "Assert.h"
#include "Iterator.h"
#include "Optional.h"
#include "Span.h"
#include "StringCommon.h"
#include "StringIterator.h"
#include "Text.h"
#include "Types.h"
#include "Vector.h"

namespace neo
{
    class String;

    class AsciiString
    {
        friend String;
    public:
        using StringIterator = Iterator<AsciiString>;
    
        constexpr ~AsciiString()
        {
            delete m_buffer;
            m_buffer = nullptr;
        }

        explicit constexpr AsciiString() = default;

        constexpr AsciiString(const char* cstring)
        {
            size_t size = __builtin_strlen(cstring);

            m_buffer = new char[size + 1];
            m_buffer[size] = 0;
            m_length = size;
            __builtin_memcpy(m_buffer, cstring, size);
        }

        constexpr AsciiString(const char* cstring, size_t length)
        {
            size_t size = length;

            m_buffer = new char[size + 1];
            m_buffer[size] = 0;
            m_length = size;
            for (size_t i = 0; i < size; i++)
                m_buffer[i] = cstring[i];
        }

        constexpr AsciiString& operator=(const AsciiString& other)
        {
            if (&other == this)
                return *this;

            delete[] m_buffer;
            m_buffer = new char[other.m_length + 1];
            m_buffer[other.m_length] = 0;
            m_length = other.m_length;

            __builtin_memcpy(m_buffer, other.m_buffer, other.m_length);
            return *this;
        }

        constexpr AsciiString& operator=(AsciiString&& other)
        {
            if (&other == this)
                return *this;

            delete[] m_buffer;
            m_buffer = other.m_buffer;
            m_length = other.m_length;
            other.m_buffer = nullptr;
            other.m_length = 0;
            return *this;
        }

        constexpr operator char*() const
        {
            return m_buffer;
        }

        [[nodiscard]] constexpr bool operator==(const AsciiString& other) const
        {
            if (m_length != other.m_length)
                return false;
            return __builtin_memcmp(m_buffer, other.m_buffer, min(m_length, other.m_length) + 1) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const AsciiString& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const AsciiString& other) const
        {
            if (m_length < other.m_length)
                return -1;
            else if (m_length > other.m_length)
                return 1;

            return clamp(-1, 1, __builtin_memcmp(m_buffer, other.m_buffer, min(m_length, other.m_length) + 1));
        }

        constexpr AsciiString(const AsciiString& other) :
            m_length(other.m_length)
        {
            m_buffer = new char[other.m_length + 1];
            m_buffer[other.m_length] = 0;
            __builtin_memcpy(m_buffer, other.m_buffer, other.m_length);
        }

        constexpr AsciiString(AsciiString&& other)
        {
            m_buffer = other.m_buffer;
            m_length = other.m_length;
            other.m_buffer = nullptr;
            other.m_length = 0;
        }

        [[nodiscard]]  constexpr StringIterator begin() const
        {
            return {*this };
        }

        [[nodiscard]] constexpr StringIterator end() const
        {
            return {*this, m_length};
        }

        [[nodiscard]] constexpr size_t length() const
        {
            return m_length;
        }
    
        [[nodiscard]] constexpr size_t size() const
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return m_length;
#pragma GCC diagnostic pop
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_length == 0 || m_buffer == nullptr;
        }

        [[nodiscard]] constexpr char operator[](size_t index) const
        {
            VERIFY(index < length());
            return m_buffer[index];
        }

        [[nodiscard]] constexpr AsciiStringView to_view() const
        {
            return { m_buffer, m_length };
        }

        constexpr operator AsciiStringView() const
        {
            return { m_buffer, m_length };
        }

        [[nodiscard]] constexpr AsciiString substring(StringIterator const& start) const
        {
            VERIFY(!start.is_end());
            
            return  AsciiString { &m_buffer[start.index()], m_length - start.index() };
        }

        [[nodiscard]] constexpr AsciiString substring(size_t start) const
        {
            VERIFY(start <= m_length);
            return substring(start, m_length - start);
        }

        [[nodiscard]] constexpr AsciiString substring(StringIterator start, size_t length) const
        {
            VERIFY(!start.is_end());
            return substring(start.index(), length);
        }

        [[nodiscard]] constexpr AsciiString substring(size_t start, size_t length) const
        {
            VERIFY(length <= m_length - start);
            return { m_buffer + start, length };
        }

        [[nodiscard]] Vector<AsciiString> split(char by) const
        {
            Vector<AsciiString> strings;
            auto _begin = begin();
            auto current = begin();
            auto _end = end();
            do
            {
                ++current;
                if (*current == by)
                {
                    strings.construct(&m_buffer[_begin.index()], current.index() - _begin.index());
                    while (*current == by)
                        ++current;
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(&m_buffer[_begin.index()], current.index() - _begin.index());
            return strings;
        }

        [[nodiscard]] Vector<AsciiString> split(const AsciiStringView& by) const
        {
            VERIFY(!by.is_empty());
            Vector<AsciiString> strings;
            auto _begin = begin();
            auto current = begin();
            auto _end = end();
            do
            {
                ++current;
                if (AsciiStringView(&m_buffer[current.index()], min(by.length(), (size_t)(&m_buffer[_end.index()] - &m_buffer[current.index()]))).starts_with(by))
                {
                    strings.construct(&m_buffer[_begin.index()], current.index() - _begin.index());
                    do
                    {
                        for (auto to_skip = by.length(); to_skip > 0; to_skip--)
                            ++current;
                    } while (AsciiStringView(&m_buffer[current.index()], min(by.length(), (size_t)(&m_buffer[_end.index()] - &m_buffer[current.index()]))).starts_with(by));
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(&m_buffer[_begin.index()], current.index() - _begin.index());
            return strings;
        }

        [[nodiscard]] constexpr bool starts_with(const AsciiString& other)
        {
            if (!length() || !other.length() || other.length() > length())
                return false;
    
            return __builtin_memcmp(m_buffer, other.m_buffer, other.m_length) == 0;
        }

        [[nodiscard]] constexpr bool ends_with(const AsciiString& other)
        {
            if (!length() || !other.length() || other.length() > length())
                return false;
    
            return __builtin_memcmp(m_length - other.m_length + m_buffer, other.m_buffer, other.m_length) == 0;
        }

        [[nodiscard]] constexpr StringIterator find(const AsciiString& other)
        {
            if (!length() || !other.length() || length() < other.length())
                return end();
            
            char* hit = __builtin_strstr(m_buffer, other.m_buffer);
            if (!hit)
                return end();
            return { m_buffer, size_t(hit - m_buffer) };
        }
    
        [[nodiscard]] constexpr bool contains(const AsciiString& other)
        {
            return find(other) != end();
        }
    
        [[nodiscard]] constexpr AsciiString trim_whitespace(TrimMode from_where) const
        {
            size_t length = m_length;
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                while (length > 0 && isspace(m_buffer[length]))
                    length--;
            }

            const char* start = m_buffer;
            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                while ((size_t)(start - m_buffer) < m_length && isspace(*start))
                    ++start;
            }
            return AsciiString(start, length);
        }

        [[nodiscard]] constexpr Span<char> span()
        {
            return { m_buffer, m_length };
        }

        [[nodiscard]] constexpr char* null_terminated_characters() const
        {
            return m_buffer;
        }

    private:
        char* m_buffer { nullptr };
        size_t m_length { 0 };
    };

    [[nodiscard]] constexpr AsciiString operator""_as(const char* cstring, size_t length)
    {
        return AsciiString(cstring, length);
    }

    template<typename TStr>
    struct StringHasher;

    template<>
    struct StringHasher<AsciiString>
    {
        static constexpr size_t hash(const AsciiString& str)
        {
            VERIFY(str.length() != 0);

            char* data = (char*)str;
            size_t size = str.length();
            size_t result = data[size - 1];
            while (size--)
                result += result ^ data[size] ^ (~(result * result + 3241));
            return result;
        }
    };
}
using neo::AsciiString;
using neo::operator""_as;

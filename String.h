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

namespace neo
{
    class String
    {
    public:
        constexpr String() = default;
        constexpr ~String()
        {
            delete m_buffer;
        }

        constexpr String(const String& other) :
            m_byte_length(other.m_byte_length)
        {
            m_buffer = new char[other.m_byte_length + 1];
            m_buffer[other.m_byte_length] = 0;

            __builtin_memcpy(m_buffer, other.m_buffer, other.m_byte_length);
        }

        constexpr String(String&& other)
        {
            m_buffer = other.m_buffer;
            m_byte_length = other.m_byte_length;
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
            m_buffer = new char[other.m_byte_length];
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

        [[nodiscard]] constexpr operator char*() const
        {
            return m_buffer;
        }

        [[nodiscard]] constexpr bool operator==(const String& other) const
        {
            if (m_byte_length != other.m_byte_length)
                return false;
            return __builtin_memcmp(m_buffer, other.m_buffer, min(m_byte_length, other.m_byte_length) + 1) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const String& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const String& other) const
        {
            if (m_byte_length < other.m_byte_length)
                return -1;
            else if (m_byte_length > other.m_byte_length)
                return 1;

            return clamp(-1, 1, __builtin_memcmp(m_buffer, other.m_buffer, min(m_byte_length, other.m_byte_length) + 1));
        }

        [[nodiscard]] constexpr const StringBidIt cbegin() const
        {
            return StringBidIt(m_buffer);
        }

        [[nodiscard]] constexpr StringBidIt begin() const
        {
            return StringBidIt(m_buffer);
        }

        [[nodiscard]] constexpr const StringBidIt cend() const
        {
            return StringBidIt(m_buffer + m_byte_length);
        }

        [[nodiscard]] constexpr StringBidIt end() const
        {
            return StringBidIt(m_buffer + m_byte_length);
        }

        //Size in bytes
        [[nodiscard]] constexpr size_t byte_size() const
        {
            return m_byte_length;
        }

        [[nodiscard]] constexpr size_t length() const
        {
            auto begin = cbegin();
            auto end = cend();
            size_t count = 0;
            while (begin++ != end)
                count++;
            return count;
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_byte_length == 0 || m_buffer == nullptr;
        }

        [[nodiscard]] constexpr String substring(StringBidIt start) const
        {
            return { start->data, static_cast<size_t>(m_buffer + m_byte_length - start->data) };
        }

        [[nodiscard]] constexpr String substring(size_t index_codepoint_start) const
        {
            VERIFY(index_codepoint_start <= m_byte_length);

            auto start = cbegin();
            auto end = cend();
            while (index_codepoint_start-- && start++ != end)
                ;

            return { start->data, static_cast<size_t>(m_buffer + m_byte_length - start->data) };
        }

        [[nodiscard]] constexpr String substring(StringBidIt start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length <= m_byte_length);
            auto last = start;
            auto end = cend();
            while (codepoint_length-- && last++ != end)
                ;

            return { start->data, static_cast<size_t>(last->data - start->data) };
        }

        [[nodiscard]] constexpr String substring(size_t codepoint_start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length <= m_byte_length);
            auto start = cbegin();
            auto end = cend();

            while (codepoint_start-- && start++ != end)
                ;

            auto last = start;
            while (codepoint_length-- && last++ != end)
                ;

            return { start->data, static_cast<size_t>(last->data - start->data) };
        }

        [[nodiscard]] Vector<String> split(Utf8Char by) const
        {
            Vector<String> strings;
            auto _begin = begin();
            auto current = _begin;
            auto _end = end();
            do
            {
                ++current;
                if (*current == by)
                {
                    strings.construct(_begin->data, current->data - _begin->data);
                    while (*current == by)
                        ++current;
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(_begin->data, current->data - _begin->data);
            return strings;
        }

        [[nodiscard]] Vector<String> split(const StringView& by) const
        {
            VERIFY(!by.is_empty());
            Vector<String> strings;
            auto _begin = begin();
            auto current = _begin;
            auto _end = end();
            do
            {
                ++current;
                if (StringView(current->data, min(by.byte_size(), (size_t)(_end->data - current->data))).starts_with(by))
                {
                    strings.construct(_begin->data, current->data - _begin->data);
                    do
                    {
                        for (auto to_skip = by.length(); to_skip > 0; to_skip--)
                            ++current;
                    } while (StringView(current->data, min(by.byte_size(), (size_t)(_end->data - current->data))).starts_with(by));
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(_begin->data, current->data - _begin->data);
            return strings;
        }

        [[nodiscard]] constexpr bool starts_with(const String& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            return __builtin_memcmp(m_buffer, other.m_buffer, other.m_byte_length) == 0;
        }

        [[nodiscard]] constexpr bool ends_with(const String& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            return __builtin_memcmp(m_byte_length - other.m_byte_length + m_buffer, other.m_buffer, other.m_byte_length) == 0;
        }

        [[nodiscard]] constexpr Optional<StringBidIt> contains(const String& other) const
        {
            if (!byte_size() || !other.byte_size() || byte_size() < other.byte_size())
                return {};

            char* hit = __builtin_strstr(m_buffer, other.m_buffer);
            return hit != nullptr ? StringBidIt(hit) : Optional<StringBidIt>();
        }

        [[nodiscard]] constexpr String trim_whitespace(TrimMode from_where) const
        {
            size_t length = m_byte_length;
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                auto _end = end();
                --_end;
                while (isspace(*_end))
                    --_end;
                length -= _end->data - m_buffer;
            }

            const char* start = m_buffer;
            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                auto _start = begin();
                while (isspace(*_start))
                    ++_start;
                length -= _start->data - m_buffer;
                start = _start->data;
            }
            return String(start, length);
        }

        [[nodiscard]] constexpr Span<char> span()
        {
            return { m_buffer, m_byte_length };
        }

        [[nodiscard]] constexpr char* null_terminated_characters() const
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
        static constexpr size_t hash(const String& str)
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
}
using neo::String;
using neo::operator""_s;
using neo::StringHasher;

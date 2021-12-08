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
#include "StringCommon.h"
#include "StringIterator.h"
#include "Text.h"
#include "Types.h"
#include "Vector.h"

namespace neo
{
    class StringView
    {
    public:
        constexpr StringView() = default;
        constexpr StringView(const StringView& other) = default;
        constexpr ~StringView() = default;

        constexpr StringView(const char* cstring) :
            m_view(cstring), m_byte_length(__builtin_strlen(cstring))
        {
        }

        constexpr StringView(const char* cstring, size_t byte_length) :
            m_view(cstring), m_byte_length(min(byte_length, __builtin_strlen(cstring)))
        {
        }

        constexpr StringView(StringView&& other) :
            m_view(other.m_view), m_byte_length(other.m_byte_length)
        {
            other.m_view = nullptr;
            other.m_byte_length = 0;
        }

        constexpr StringView& operator=(const StringView& other) = default;
        constexpr StringView& operator=(StringView&& other)
        {
            if (*this == other)
                return *this;

            m_view = other.m_view;
            m_byte_length = other.m_byte_length;
            other.m_view = nullptr;
            other.m_byte_length = 0;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const StringView& other) const
        {
            if (m_byte_length != other.m_byte_length)
                return false;
            return __builtin_memcmp(m_view, other.m_view, min(m_byte_length, other.m_byte_length)) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const StringView& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const StringView& other) const
        {
            if (m_byte_length < other.m_byte_length)
                return -1;
            else if (m_byte_length > other.m_byte_length)
                return 1;

            return clamp(-1, 1, __builtin_memcmp(m_view, other.m_view, min(m_byte_length, other.m_byte_length)));
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_byte_length == 0 || m_view == nullptr;
        }

        [[nodiscard]] constexpr size_t byte_size() const
        {
            return m_byte_length;
        }

        [[nodiscard]] constexpr size_t length() const
        {
            auto start = begin();
            size_t count { 0 };
            do
                count++;
            while(!(++start).is_end());
            return count;
        }

        [[nodiscard]] constexpr const StringViewIterator begin() const
        {
            return StringViewIterator(m_view, m_view + m_byte_length, m_view);
        }

        [[nodiscard]] constexpr const StringViewIterator end() const
        {
            return StringViewIterator(m_view, m_view + m_byte_length, m_view + m_byte_length);
        }

        [[nodiscard]] constexpr StringView substring_view(StringViewIterator start) const
        {
            VERIFY(start != end());
            return {start.ptr(), static_cast<size_t>(m_view + m_byte_length - start.ptr()) };
        }

        [[nodiscard]] constexpr StringView substring_view(size_t index_codepoint_start) const
        {
            VERIFY(index_codepoint_start < m_byte_length);

            auto start = begin();
            auto _end = end();
            while (index_codepoint_start-- && start++ != _end)
                ;
            VERIFY(start != _end);

            return {start.ptr(), static_cast<size_t>(m_view + m_byte_length - start.ptr()) };
        }

        [[nodiscard]] constexpr StringView substring_view(StringViewIterator start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length < m_byte_length);
            VERIFY(codepoint_length != 0);
            auto last = start;
            auto _end = end();
            while (codepoint_length-- && last++ != _end)
                ;
            VERIFY(last != _end);

            return {start.ptr(), static_cast<size_t>(last.ptr() - start.ptr()) };
        }

        [[nodiscard]] constexpr StringView substring_view(size_t codepoint_start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length < m_byte_length);
            VERIFY(codepoint_length != 0);
            auto start = begin();
            auto _end = end();

            //if (codepoint_start != 0)
            while (codepoint_start-- && start++ != _end)
                ;
            VERIFY(start != _end);

            auto last = start;
            while (codepoint_length-- && last++ != _end)
                ;
            VERIFY(last != _end);

            return {start.ptr(), static_cast<size_t>(last.ptr() - start.ptr()) };
        }
    
        [[nodiscard]] static constexpr StringView substring_view(StringViewIterator const& start, StringViewIterator const& end)
        {
            VERIFY(!start.is_end());
            VERIFY(start.ptr() < end.ptr());
            return StringView {start.ptr(), static_cast<size_t>(end.ptr() - start.ptr())};
        }

        [[nodiscard]] Vector<StringView> split(Utf8Char by) const
        {
            Vector<StringView> strings;
            auto _begin = begin();
            auto current = _begin;
            auto _end = end();
            do
            {
                ++current;
                if (*current == by)
                {
                    strings.construct(_begin.ptr(), static_cast<size_t>(current.ptr() - _begin.ptr()));
                    while (*current == by)
                        ++current;
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(_begin.ptr(), static_cast<size_t>(current.ptr() - _begin.ptr()));
            return strings;
        }

        [[nodiscard]] Vector<StringView> split(const StringView& by) const
        {
            VERIFY(!by.is_empty());
            Vector<StringView> strings;
            auto _begin = begin();
            auto current = _begin;
            auto _end = end();
            do
            {
                ++current;
                if (StringView(current.ptr(), min(by.byte_size(), (size_t)(_end.ptr() - current.ptr()))).starts_with(by))
                {
                    strings.construct(_begin.ptr(),static_cast<size_t>(current.ptr() - _begin.ptr()));
                    do
                    {
                        for (auto to_skip = by.length(); to_skip > 0; to_skip--)
                            ++current;
                    } while (StringView(current.ptr(), min(by.byte_size(), (size_t)(_end.ptr() - current.ptr()))).starts_with(by));
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(_begin.ptr(), static_cast<size_t>(current.ptr() - _begin.ptr()));
            return strings;
        }

        [[nodiscard]] constexpr StringView trim_whitespace(TrimMode from_where) const
        {
            StringView copy = *this;
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                auto _end = copy.end();
                --_end;
                while (isspace(*_end))
                    --_end;
                copy.m_byte_length = _end.ptr() - m_view + 1;
            }

            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                auto start = copy.begin();
                while (isspace(*start))
                    ++start;
                copy.m_view = start.ptr();
            }
            return copy;
        }

        [[nodiscard]] constexpr Span<const char> span() const
        {
            return { m_view, m_byte_length };
        }

        [[nodiscard]] constexpr const char* non_null_terminated_buffer() const
        {
            return m_view;
        }
    
        [[nodiscard]] constexpr StringViewIterator find(const StringView& other) const
        {
            if (is_empty() || other.is_empty() || byte_size() < other.byte_size())
                return end();
        
            char* hit = __builtin_strstr(m_view, other.m_view);
            if (!hit)
                return end();
            return StringViewIterator(m_view, m_view+m_byte_length, hit);
        }

        [[nodiscard]] constexpr bool contains(const StringView& other) const
        {
            return !find(other).is_end();
        }
        
        [[nodiscard]] constexpr bool starts_with(const StringView& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            return __builtin_memcmp(m_view, other.m_view, other.m_byte_length) == 0;
        }

        [[nodiscard]] constexpr bool ends_with(const StringView& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            return __builtin_memcmp(m_byte_length - other.m_byte_length + m_view, other.m_view, other.m_byte_length) == 0;
        }

    private:
        const char* m_view { nullptr };
        size_t m_byte_length { 0 };
    };

    [[nodiscard]] constexpr StringView operator""_sv(const char* cstring, size_t length)
    {
        return StringView(cstring, length);
    }

    template<typename>
    struct StringHasher;

    template<>
    struct StringHasher<StringView>
    {
        static constexpr size_t hash(const StringView& str)
        {
            VERIFY(str.length() != 0);

            char* data = (char*)str.non_null_terminated_buffer();
            size_t size = str.byte_size();
            size_t result = data[size - 1];
            while (size--)
                result += result ^ data[size] ^ (~(result * result + 3241));
            return result;
        }
    };
}
using neo::StringView;
using neo::operator""_sv;

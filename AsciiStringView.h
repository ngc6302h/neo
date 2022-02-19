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
#include "Span.h"
#include "StringCommon.h"
#include "StringIterator.h"
#include "Text.h"
#include "Vector.h"

namespace neo
{
    class AsciiStringView
    {
    public:
        using type = const char;
        using AsciiStringIterator = Iterator<AsciiStringView>;

        constexpr AsciiStringView() = default;
        constexpr AsciiStringView(const AsciiStringView& other) = default;
        constexpr ~AsciiStringView() = default;

        constexpr AsciiStringView(const char* unsafe_cstring) :
            m_view(unsafe_cstring), m_length(__builtin_strlen(unsafe_cstring))
        {
        }

        constexpr AsciiStringView(const char* cstring, size_t byte_length) :
            m_view(cstring), m_length(min(byte_length, __builtin_strlen(cstring)))
        {
        }

        constexpr AsciiStringView(AsciiStringView&& other) :
            m_view(other.m_view), m_length(other.m_length)
        {
            other.m_view = nullptr;
            other.m_length = 0;
        }

        constexpr AsciiStringView& operator=(const AsciiStringView& other) = default;
        constexpr AsciiStringView& operator=(AsciiStringView&& other)
        {
            if (*this == other)
                return *this;

            m_view = other.m_view;
            m_length = other.m_length;
            other.m_view = nullptr;
            other.m_length = 0;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const AsciiStringView& other) const
        {
            if (m_length != other.m_length)
                return false;
            return __builtin_memcmp(m_view, other.m_view, min(m_length, other.m_length)) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const AsciiStringView& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const AsciiStringView& other) const
        {
            if (m_length < other.m_length)
                return -1;
            else if (m_length > other.m_length)
                return 1;

            return clamp(-1, 1, __builtin_memcmp(m_view, other.m_view, min(m_length, other.m_length)));
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_length == 0 || m_view == nullptr;
        }

        [[nodiscard]] constexpr size_t length() const
        {
            return m_length;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_length;
        }

        [[nodiscard]] constexpr AsciiStringIterator begin() const
        {
            return { *this };
        }

        [[nodiscard]] constexpr AsciiStringIterator end() const
        {
            return { *this, m_length };
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(AsciiStringIterator const& start) const
        {
            VERIFY(!start.index());
            return substring_view(start, m_length - start.index());
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(size_t start) const
        {
            VERIFY(start <= m_length);
            return { m_view + start, m_length - start };
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(AsciiStringIterator const& start, size_t length) const
        {
            VERIFY(!start.is_end());
            return substring_view(start.index(), length);
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(size_t start, size_t length) const
        {
            VERIFY(length <= m_length - start);
            return { m_view + start, length };
        }

        [[nodiscard]] Vector<AsciiStringView> split(char by) const
        {
            Vector<AsciiStringView> strings;
            auto _begin = begin();
            auto current = begin();
            auto _end = end();
            do
            {
                ++current;
                if (*current == by)
                {
                    strings.construct(&m_view[_begin.index()], current.index() - _begin.index());
                    while (*current == by)
                        ++current;
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(&m_view[_begin.index()], current.index() - _begin.index());
            return strings;
        }

        [[nodiscard]] Vector<AsciiStringView> split(const AsciiStringView& by) const
        {
            VERIFY(!by.is_empty());
            Vector<AsciiStringView> strings;
            auto _begin = begin();
            auto current = begin();
            auto _end = end();
            do
            {
                ++current;
                if (substring_view(current, min(by.length(), (size_t)end().index() - _begin.index())).starts_with(by))
                {
                    strings.construct(&m_view[_begin.index()], current.index() - _begin.index());
                    do
                    {
                        for (auto to_skip = by.length(); to_skip > 0; to_skip--)
                            ++current;
                    } while (substring_view(current, min(by.length(), (size_t)(current.index() - _begin.index()))).starts_with(by));
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(&m_view[_begin.index()], current.index() - _begin.index());
            return strings;
        }

        [[nodiscard]] constexpr AsciiStringView trim_whitespace(TrimMode from_where) const
        {
            size_t length = m_length;
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                while (length > 0 && isspace(m_view[length]))
                    length--;
            }

            const char* start = m_view;
            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                while ((size_t)(start - m_view) < m_length && isspace(*start))
                    ++start;
            }
            return AsciiStringView(start, length);
        }

        [[nodiscard]] constexpr Span<const char> span() const
        {
            return { m_view, m_length };
        }

        [[nodiscard]] constexpr const char* non_null_terminated_buffer() const
        {
            return m_view;
        }

        [[nodiscard]] constexpr char operator[](size_t index) const
        {
            VERIFY(index < m_length);
            return m_view[index];
        }

        constexpr bool contains(const AsciiStringView& other) const
        {
            return !find(other).is_end();
        }

        constexpr AsciiStringIterator find(const AsciiStringView& other) const
        {
            if (is_empty() || other.is_empty() || length() < other.length())
                return end();

            char* hit = __builtin_strstr(m_view, other.m_view);
            if (!hit)
                return end();
            return { *this, size_t(hit - m_view) };
        }

        [[nodiscard]] constexpr bool starts_with(const AsciiStringView& other) const
        {
            if (!length() || !other.length() || other.length() > length())
                return false;

            return __builtin_memcmp(m_view, other.m_view, other.m_length) == 0;
        }

        [[nodiscard]] constexpr bool ends_with(const AsciiStringView& other) const
        {
            if (!length() || !other.length() || other.length() > length())
                return false;

            return __builtin_memcmp(m_length - other.m_length + m_view, other.m_view, other.m_length) == 0;
        }

    private:
        const char* m_view { nullptr };
        size_t m_length { 0 };
    };

    [[nodiscard]] constexpr AsciiStringView operator""_asv(const char* cstring, size_t length)
    {
        return AsciiStringView(cstring, length);
    }

    template<typename>
    struct StringHasher;

    template<>
    struct StringHasher<AsciiStringView>
    {
        static constexpr size_t hash(const AsciiStringView& str)
        {
            VERIFY(str.length() != 0);
            char* data = (char*)str.non_null_terminated_buffer();
            size_t size = str.length();
            size_t result = data[size - 1];
            while (size--)
                result += result ^ data[size] ^ (~(result * result + 3241));
            return result;
        }
    };

    using neo::AsciiStringView;
    using neo::operator""_asv;
}
using neo::AsciiStringView;
using neo::operator""_asv;

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
#include "Vector.h"
#include "StringIterator.h"

namespace neo
{
    enum TrimMode
    {
        Start = 1,
        End = 2,
        Both = Start | End
    };

    template<typename T, typename TIterator>
    struct IString
    {
        [[nodiscard]] constexpr Span<char> span()
        {
            auto& o = static_cast<T const&>(*this);
            return { o.data(), o.byte_size() };
        }

        [[nodiscard]] constexpr Span<const char> span() const
        {
            auto& o = static_cast<T const&>(*this);
            return { o.data(), o.byte_size() };
        }

        [[nodiscard]] constexpr operator char const*() const
        {
            auto& o = static_cast<T const&>(*this);
            return o.data();
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr bool operator==(U const& other) const
        {
            auto& o = static_cast<T const&>(*this);
            if (o.byte_size() != other.byte_size())
                return false;
            return __builtin_memcmp(o.data(), other.data(), min(o.byte_size(), other.byte_size()) + 1) == 0;
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr int operator<=>(U const& other) const
        {
            auto& o = static_cast<T const&>(*this);
            if (o.byte_size() < other.byte_size())
                return -1;
            else if (o.byte_size() > other.byte_size())
                return 1;

            return clamp(-1, 1, __builtin_memcmp(o.data(), other.data(), min(o.byte_size(), other.byte_size()) + 1));
        }

        [[nodiscard]] constexpr TIterator begin() const
        {
            auto& o = static_cast<T const&>(*this);
            return TIterator(o.data(), o.data() + o.byte_size(), o.data());
        }

        [[nodiscard]] constexpr TIterator end() const
        {
            auto& o = static_cast<T const&>(*this);
            return TIterator(o.data(), o.data() + o.byte_size(), o.data() + o.byte_size());
        }

        [[nodiscard]] constexpr size_t length() const
        {
            auto start = begin();
            size_t count = 0;
            do
                count++;
            while (!(++start).is_end());

            return count;
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            auto& o = static_cast<T const&>(*this);
            return o.byte_size() == 0 || o.data() == nullptr;
        }

        [[nodiscard]] constexpr T substring(TIterator start) const
        {
            auto& o = static_cast<T const&>(*this);
            return T { start.ptr(), size_t(o.data() + o.byte_size() - start.ptr()) };
        }

        [[nodiscard]] constexpr T substring(size_t index_codepoint_start) const
        {
            auto& o = static_cast<T const&>(*this);
            VERIFY(index_codepoint_start <= o.byte_size());

            auto start = begin();
            while (index_codepoint_start-- && !start++.is_end())
                ;

            return T { start.ptr(), static_cast<size_t>(o.data() + o.byte_size() - start.ptr()) };
        }

        [[nodiscard]] static constexpr T substring(TIterator const& start, TIterator const& end)
        {
            VERIFY(!start.is_end());
            VERIFY(start.ptr() < end.ptr());
            return { start.ptr(), static_cast<size_t>(end.ptr() - start.ptr()) };
        }

        [[nodiscard]] constexpr T substring(TIterator start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length <= length());
            VERIFY(codepoint_length != 0);
            auto last = start;
            auto _end = end();
            VERIFY(start != _end);
            while (codepoint_length-- && last++ != _end)
                ;

            return T { start.ptr(), static_cast<size_t>(last.ptr() - start.ptr()) };
        }

        [[nodiscard]] constexpr T substring(size_t codepoint_start, size_t codepoint_length) const
        {
            auto& o = static_cast<T const&>(*this);

            VERIFY(codepoint_length <= o.byte_size());
            auto start = begin();
            auto _end = end();

            while (codepoint_start-- && start++ != _end)
                ;

            auto last = start;
            while (codepoint_length-- && last++ != _end)
                ;

            return { start.ptr(), static_cast<size_t>(last.ptr() - start.ptr()) };
        }

        [[nodiscard]] Vector<T> split(Utf8Char by) const
        {
            Vector<T> strings;
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

        template<typename U>
        [[nodiscard]] Vector<T> split(U const& by) const
        {
            VERIFY(!by.is_empty());
            Vector<T> strings;
            auto _begin = begin();
            auto current = _begin;
            auto _end = end();
            do
            {
                ++current;
                if (StringView(current.ptr(), min(by.byte_size(), static_cast<size_t>(_end.ptr() - current.ptr()))).starts_with(by))
                {
                    strings.construct(_begin.ptr(), static_cast<size_t>(current.ptr() - _begin.ptr()));
                    do
                    {
                        for (auto to_skip = by.length(); to_skip > 0; to_skip--)
                            ++current;
                    } while (StringView(current.ptr(), min(by.byte_size(), static_cast<size_t>(_end.ptr() - current.ptr()))).starts_with(by));
                    _begin = current;
                }
            } while (current != _end);
            if (_begin != _end)
                strings.construct(_begin.ptr(), static_cast<size_t>(current.ptr() - _begin.ptr()));
            return strings;
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr bool starts_with(U const& other) const
        {
            auto& o = static_cast<T const&>(*this);

            if (!o.byte_size() || !other.byte_size() || other.byte_size() > o.byte_size())
                return false;

            return __builtin_memcmp(o.data(), other.data(), other.byte_size()) == 0;
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr bool ends_with(U const& other) const
        {
            auto& o = static_cast<T const&>(*this);

            if (!o.byte_size() || !other.byte_size() || other.byte_size() > o.byte_size())
                return false;

            return __builtin_memcmp(o.byte_size() - other.byte_size() + o.data(), other.data(), other.byte_size()) == 0;
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr TIterator find(U const& other) const
        {
            auto& o = static_cast<T const&>(*this);

            if (!o.byte_size() || !other.byte_size() || o.byte_size() < other.byte_size())
                return end();

            char* hit = __builtin_strstr(o.data(), other.data());
            if (!hit)
                return end();
            return TIterator(o.data(), o.data() + o.byte_size(), hit);
        }

        [[nodiscard]] constexpr bool contains(Utf8Char c) const
        {
            auto& o = static_cast<T const&>(*this);

            if (!o.byte_size())
                return false;

            for (const auto ch : o)
            {
                if (c == ch)
                    return true;
            }
            return false;
        }

        template<typename U>
        requires BaseOf<IString<U, StringIteratorContainer>, U>
        [[nodiscard]] constexpr bool contains(U const& other) const
        {
            return !find(other).is_end();
        }

        [[nodiscard]] constexpr T trim_whitespace(TrimMode from_where) const
        {
            auto& o = static_cast<T const&>(*this);

            size_t length = o.byte_size();
            if ((from_where & TrimMode::End) == TrimMode::End)
            {
                auto _end = end();
                --_end;
                while (isspace(*_end))
                    --_end;
                length -= _end.ptr() - o.data();
            }

            const char* start = o.data();
            if ((from_where & TrimMode::Start) == TrimMode::Start)
            {
                auto _start = begin();
                while (isspace(*_start))
                    ++_start;
                length -= _start.ptr() - o.data();
                start = _start.ptr();
            }
            return T(start, length);
        }
    };
}

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
    class StringView : public IString<StringView, StringViewIterator>
    {
    public:
        using character_type = Utf8Char;
        using storage_type = char;
        using iterator = StringViewIterator;

        constexpr StringView() = default;
        constexpr StringView(const StringView& other) = default;
        constexpr ~StringView() = default;

        constexpr StringView(const char* cstring) :
            m_view(cstring), m_byte_length(__builtin_strlen(cstring))
        {
        }

        constexpr StringView(const char* cstring, size_t byte_length) :
            m_view(cstring), m_byte_length(byte_length)
        {
        }

        constexpr StringView(StringView&& other) :
            m_view(other.m_view), m_byte_length(other.m_byte_length)
        {
            other.m_view = nullptr;
            other.m_byte_length = 0;
        }

        constexpr StringView& operator=(StringView const& other) = default;
        constexpr StringView& operator=(StringView&& other)
        {
            if (this == &other)
                return *this;

            m_view = other.m_view;
            m_byte_length = other.m_byte_length;
            other.m_view = nullptr;
            other.m_byte_length = 0;
            return *this;
        }

        [[nodiscard]] constexpr size_t byte_size() const
        {
            return m_byte_length;
        }

        [[nodiscard]] constexpr const storage_type* non_null_terminated_buffer() const
        {
            return m_view;
        }

        [[nodiscard]] constexpr const storage_type* data() const
        {
            return m_view;
        }

    private:
        const char* m_view { nullptr };
        size_t m_byte_length { 0 };
    };

    [[nodiscard]] constexpr StringView operator""_sv(const char* cstring, size_t length)
    {
        return StringView(cstring, length);
    }

    // BEGIN STRINGCOMMON DEFINITIONS

    template<typename T, typename TIterator>
    constexpr Span<char> IString<T, TIterator>::span()
    {
        auto& o = static_cast<T const&>(*this);
        return { o.data(), o.byte_size() };
    }

    template<typename T, typename TIterator>
    constexpr Span<const char> IString<T, TIterator>::span() const
    {
        auto& o = static_cast<T const&>(*this);
        return { o.data(), o.byte_size() };
    }

    template<typename T, typename TIterator>
    constexpr IString<T, TIterator>::operator char const*() const
    {
        auto& o = static_cast<T const&>(*this);
        return o.data();
    }

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::operator==(const StringView& other) const
    {
        auto& o = static_cast<T const&>(*this);
        if (o.byte_size() != other.byte_size())
            return false;
        return __builtin_memcmp(o.data(), other.data(), min(o.byte_size(), other.byte_size())) == 0;
    }

    template<typename T, typename TIterator>
    constexpr int IString<T, TIterator>::operator<=>(const StringView& other) const
    {
        auto& o = static_cast<T const&>(*this);
        if (o.byte_size() < other.byte_size())
            return -1;
        else if (o.byte_size() > other.byte_size())
            return 1;

        return clamp(-1, 1, __builtin_memcmp(o.data(), other.data(), min(o.byte_size(), other.byte_size()) + 1));
    }

    template<typename T, typename TIterator>
    constexpr TIterator IString<T, TIterator>::begin() const
    {
        auto& o = static_cast<T const&>(*this);
        return TIterator(o.data(), o.data() + o.byte_size(), o.data());
    }

    template<typename T, typename TIterator>
    constexpr TIterator IString<T, TIterator>::end() const
    {
        auto& o = static_cast<T const&>(*this);
        return TIterator(o.data(), o.data() + o.byte_size(), o.data() + o.byte_size());
    }

    template<typename T, typename TIterator>
    constexpr size_t IString<T, TIterator>::length() const
    {
        auto start = begin();
        size_t count = 0;
        do
            count++;
        while (!(++start).is_end());

        return count;
    }

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::is_empty() const
    {
        auto& o = static_cast<T const&>(*this);
        return o.byte_size() == 0 || o.data() == nullptr;
    }

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::substring(TIterator start) const
    {
        auto& o = static_cast<T const&>(*this);
        return T { start.ptr(), size_t(o.data() + o.byte_size() - start.ptr()) };
    }

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::substring(size_t index_codepoint_start) const
    {
        auto& o = static_cast<T const&>(*this);
        VERIFY(index_codepoint_start <= o.byte_size());

        auto start = begin();
        while (index_codepoint_start-- && !start++.is_end())
            ;

        return T { start.ptr(), static_cast<size_t>(o.data() + o.byte_size() - start.ptr()) };
    }

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::substring(const TIterator& start, const TIterator& end)
    {
        VERIFY(!start.is_end());
        VERIFY(start.ptr() < end.ptr());
        return { start.ptr(), static_cast<size_t>(end.ptr() - start.ptr()) };
    }

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::substring(TIterator start, size_t codepoint_length) const
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

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::substring(size_t codepoint_start, size_t codepoint_length) const
    {
        VERIFY(codepoint_length <= static_cast<T const&>(*this).byte_size());
        auto start = begin();
        auto _end = end();

        while (codepoint_start-- && start++ != _end)
            ;

        auto last = start;
        while (codepoint_length-- && last++ != _end)
            ;

        return { start.ptr(), static_cast<size_t>(last.ptr() - start.ptr()) };
    }

    template<typename T, typename TIterator>
    Vector<T> IString<T, TIterator>::split(Utf8Char by) const
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

    template<typename T, typename TIterator>
    Vector<T> IString<T, TIterator>::split(StringView const& by) const
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

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::starts_with(const StringView& other) const
    {
        auto& o = static_cast<T const&>(*this);

        if (!o.byte_size() || !other.byte_size() || other.byte_size() > o.byte_size())
            return false;

        return __builtin_memcmp(o.data(), other.data(), other.byte_size()) == 0;
    }

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::ends_with(const StringView& other) const
    {
        auto& o = static_cast<T const&>(*this);

        if (!o.byte_size() || !other.byte_size() || other.byte_size() > o.byte_size())
            return false;

        return __builtin_memcmp(o.byte_size() - other.byte_size() + o.data(), other.data(), other.byte_size()) == 0;
    }

    template<typename T, typename TIterator>
    constexpr TIterator IString<T, TIterator>::find(const StringView& other) const
    {
        auto& o = static_cast<T const&>(*this);

        if (!o.byte_size() || !other.byte_size() || o.byte_size() < other.byte_size())
            return end();

        char* hit = __builtin_strstr(o.data(), other.data());
        if (!hit)
            return end();
        return TIterator(o.data(), o.data() + o.byte_size(), hit);
    }

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::contains(Utf8Char c) const
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

    template<typename T, typename TIterator>
    constexpr bool IString<T, TIterator>::contains(const StringView& other) const
    {
        return !find(other).is_end();
    }

    template<typename T, typename TIterator>
    constexpr T IString<T, TIterator>::trim_whitespace(TrimMode from_where) const
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

    // END STRINGCOMMON DEFINITIONS

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

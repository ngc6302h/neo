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
#include "StringIterator.h"
#include "Types.h"

namespace neo
{
    class StringView
    {
    public:
        constexpr StringView() = default;
        constexpr StringView(const StringView& other) = default;

        constexpr StringView(const char* cstring) :
            m_data(cstring), m_byte_length(__builtin_strlen(cstring))
        {
        }

        constexpr StringView(const char* cstring, size_t byte_length) :
            m_data(cstring), m_byte_length(min(byte_length, __builtin_strlen(cstring)))
        {
        }

        constexpr StringView(StringView&& other) :
            m_data(other.m_data), m_byte_length(other.m_byte_length)
        {
            other.m_data = nullptr;
            other.m_byte_length = 0;
        }

        constexpr StringView& operator=(const StringView& other) = default;
        constexpr StringView& operator=(StringView&& other)
        {
            if (*this == other)
                return *this;

            m_data = other.m_data;
            m_byte_length = other.m_byte_length;
            other.m_data = nullptr;
            other.m_byte_length = 0;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const StringView& other) const
        {
            if (m_byte_length != other.m_byte_length)
                return false;
            return __builtin_memcmp(m_data, other.m_data, m_byte_length) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const StringView& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const StringView& other) const
        {
            auto begin = cbegin();
            auto other_begin = other.cbegin();
            auto end = cend();
            auto other_cend = other.cend();
            for (; (begin != end || other_begin != other_cend) && *begin == *other_begin; begin++, other_begin++)
            {
            }
            if (!(begin != end) || !(other_begin != other_cend))
                return 0;
            return *begin < *other_begin ? -1 : 1;
            //TODO: could use memcmp here?
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_byte_length == 0 || m_data == nullptr;
        }

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

        [[nodiscard]] constexpr const StringViewBidIt begin() const
        {
            return StringViewBidIt(m_data);
        }

        [[nodiscard]] constexpr const StringViewBidIt cbegin() const
        {
            return StringViewBidIt(m_data);
        }

        [[nodiscard]] constexpr const StringViewBidIt end() const
        {
            return StringViewBidIt(m_data + m_byte_length);
        }

        [[nodiscard]] constexpr const StringViewBidIt cend() const
        {
            return StringViewBidIt(m_data + m_byte_length);
        }

        [[nodiscard]] constexpr StringView substring_view(StringViewBidIt start) const
        {
            VERIFY(start != cend());
            return { start->data, static_cast<size_t>(m_data + m_byte_length - start->data) };
        }

        [[nodiscard]] constexpr StringView substring_view(size_t index_codepoint_start) const
        {
            VERIFY(index_codepoint_start < m_byte_length);

            auto start = cbegin();
            auto end = cend();
            while (index_codepoint_start-- && start++ != end)
                ;
            VERIFY(start != end);

            return { start->data, static_cast<size_t>(m_data + m_byte_length - start->data) };
        }

        [[nodiscard]] constexpr StringView substring_view(StringViewBidIt start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length < m_byte_length);
            VERIFY(codepoint_length != 0);
            auto last = start;
            auto end = cend();
            while (codepoint_length-- && last++ != end)
                ;
            VERIFY(last != end);

            return { start->data, static_cast<size_t>(last->data - start->data) };
        }

        [[nodiscard]] constexpr StringView substring_view(size_t codepoint_start, size_t codepoint_length) const
        {
            VERIFY(codepoint_length < m_byte_length);
            VERIFY(codepoint_length != 0);
            auto start = cbegin();
            auto end = cend();

            //if (codepoint_start != 0)
            while (codepoint_start-- && start++ != end)
                ;
            VERIFY(start != end);

            auto last = start;
            while (codepoint_length-- && last++ != end)
                ;
            VERIFY(last != end);

            return { start->data, static_cast<size_t>(last->data - start->data) };
        }

        [[nodiscard]] constexpr Span<const char> span() const
        {
            return { m_data, m_byte_length };
        }

        [[nodiscard]] constexpr const char* non_null_terminated_buffer() const
        {
            return m_data;
        }

        //Optional can't be constexpr yet so we return an iterator past the end if it isn't found
        [[nodiscard]] constexpr StringViewBidIt contains(const StringView& other) const
        {
            if (is_empty() || other.is_empty() || byte_size() < other.byte_size())
                return cend();

            auto end = cend();
            for (auto my_char = cbegin(); my_char != end; my_char++)
            {
                bool found = true;
                for (auto other_char : other)
                {
                    if (*my_char != other_char)
                    {
                        found = false;
                        break;
                    }
                    my_char++;
                }
                if (found)
                    return my_char;
            }
            return end;
        }

        [[nodiscard]] constexpr bool starts_with(const StringView& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            for (auto other_char : other)
            {
                for (auto my_char : *this)
                {
                    if (my_char != other_char)
                        return false;
                }
            }
            return true;
        }

        [[nodiscard]] constexpr bool ends_with(const StringView& other) const
        {
            if (!byte_size() || !other.byte_size() || other.byte_size() > byte_size())
                return false;

            for (auto other_char = other.cend(); other_char != other.cbegin(); other_char--)
            {
                for (auto my_char = cend(); my_char != begin(); my_char--)
                {
                    if (my_char != other_char)
                        return false;
                    other_char--;
                }
            }
            return true;
        }

    private:
        const char* m_data { nullptr };
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
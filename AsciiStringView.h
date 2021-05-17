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
#include "StringIterator.h"

namespace neo
{
    class AsciiStringView
    {
    public:
        constexpr AsciiStringView() = default;
        constexpr AsciiStringView(const AsciiStringView& other) = default;

        constexpr AsciiStringView(const char* cstring) :
            m_buffer(cstring), m_length(__builtin_strlen(cstring))
        {
        }

        constexpr AsciiStringView(const char* cstring, size_t byte_length) :
            m_buffer(cstring), m_length(min(byte_length, __builtin_strlen(cstring)))
        {
        }

        constexpr AsciiStringView(AsciiStringView&& other) :
            m_buffer(other.m_buffer), m_length(other.m_length)
        {
            other.m_buffer = nullptr;
            other.m_length = 0;
        }

        constexpr AsciiStringView& operator=(const AsciiStringView& other) = default;
        constexpr AsciiStringView& operator=(AsciiStringView&& other)
        {
            if (*this == other)
                return *this;

            m_buffer = other.m_buffer;
            m_length = other.m_length;
            other.m_buffer = nullptr;
            other.m_length = 0;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(const AsciiStringView& other) const
        {
            if (m_length != other.m_length)
                return false;
            return __builtin_memcmp(m_buffer, other.m_buffer, m_length) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const AsciiStringView& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const AsciiStringView& other) const
        {
            return __builtin_memcmp(m_buffer, other.m_buffer, min(m_length, other.m_length));
        }

        [[nodiscard]] constexpr bool is_empty() const
        {
            return m_length == 0 || m_buffer == nullptr;
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

        [[nodiscard]] constexpr const AsciiStringViewBidIt begin() const
        {
            return AsciiStringViewBidIt(m_buffer);
        }

        [[nodiscard]] constexpr const AsciiStringViewBidIt cbegin() const
        {
            return AsciiStringViewBidIt(m_buffer);
        }

        [[nodiscard]] constexpr const AsciiStringViewBidIt end() const
        {
            return AsciiStringViewBidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr const AsciiStringViewBidIt cend() const
        {
            return AsciiStringViewBidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(AsciiStringViewBidIt start) const
        {
            VERIFY(start != cend());
            return { start->data, static_cast<size_t>(m_buffer + m_length - start->data) };
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(size_t start) const
        {
            VERIFY(start < m_length);
            return { m_buffer + start, m_length - start };
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(AsciiStringViewBidIt start, size_t length) const
        {
            VERIFY(length < m_length - (size_t)(start->data - m_buffer));
            return { start->data, length };
        }

        [[nodiscard]] constexpr AsciiStringView substring_view(size_t start, size_t length) const
        {
            VERIFY(length < m_length - start);
            return { m_buffer + start, length };
        }

        [[nodiscard]] constexpr Span<const char> span() const
        {
            return { m_buffer, m_length };
        }

        [[nodiscard]] constexpr const char* non_null_terminated_buffer() const
        {
            return m_buffer;
        }

        [[nodiscard]] constexpr char operator[](size_t index) const
        {
            VERIFY(index < m_length);
            return m_buffer[index];
        }

        //Optional can't be constexpr yet so we return an iterator past the end if it isn't found
        [[nodiscard]] constexpr AsciiStringViewBidIt contains(const AsciiStringView& other) const
        {
            if (is_empty() || other.is_empty() || length() < other.length())
                return cend();

            for (size_t i = 0; i < length() - other.length(); i++)
            {
                bool found = true;
                for (size_t j = 0; j < other.length(); j++)
                {
                    if ((*this)[i + j] != other[j])
                    {
                        found = false;
                        break;
                    }
                }
                if (found)
                    return AsciiStringViewBidIt(m_buffer + i);
            }
            return cend();
        }

        [[nodiscard]] constexpr bool starts_with(const AsciiStringView& other) const
        {
            if (!length() || !other.length() || other.length() > length())
                return false;

            for (size_t i = 0; i < other.length(); i++)
            {
                if ((*this)[i] != other[i])
                    return false;
            }
            return true;
        }

        [[nodiscard]] constexpr bool ends_with(const AsciiStringView& other) const
        {
            if (!length() || !other.length() || other.length() > length())
                return false;

            for (size_t i = 0; i < other.length(); i++)
            {
                if ((*this)[length() - other.length() + i] != other[i])
                    return false;
            }
            return true;
        }

    private:
        const char* m_buffer { nullptr };
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
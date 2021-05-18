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
#include "Optional.h"
#include "Span.h"
#include "StringIterator.h"
#include "Types.h"
#include "AsciiStringView.h"

namespace neo
{
    class String;

    class AsciiString
    {
        friend String;

    public:
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
            other.m_buffer = nullptr;
            m_length = other.m_length;
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
            return __builtin_memcmp(m_buffer, other.m_buffer, m_length) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const AsciiString& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const AsciiString& other) const
        {
            return __builtin_memcmp(m_buffer, other.m_buffer, min(m_length, other.m_length));
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
            other.m_buffer = nullptr;
            other.m_length = 0;
        }

        [[nodiscard]] constexpr const AsciiStringBidIt cbegin() const
        {
            return AsciiStringBidIt(m_buffer);
        }

        [[nodiscard]] AsciiStringBidIt begin() const
        {
            return AsciiStringBidIt(m_buffer);
        }

        [[nodiscard]] AsciiStringBidIt end() const
        {
            return AsciiStringBidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr const AsciiStringBidIt cend() const
        {
            return AsciiStringBidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr size_t length() const
        {
            return m_length;
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
    
        [[nodiscard]] constexpr AsciiString substring(AsciiStringBidIt start) const
        {
            VERIFY(start != cend());
            return { start->data, static_cast<size_t>(m_buffer + m_length - start->data) };
        }
    
        [[nodiscard]] constexpr AsciiString substring(size_t start) const
        {
            VERIFY(start < m_length);
            return { m_buffer + start, m_length - start };
        }
    
        [[nodiscard]] constexpr AsciiString substring(AsciiStringBidIt start, size_t length) const
        {
            VERIFY(length < m_length - (size_t)(start->data - m_buffer));
            return { start->data, length };
        }
    
        [[nodiscard]] constexpr AsciiString substring(size_t start, size_t length) const
        {
            VERIFY(length < m_length - start);
            return { m_buffer + start, length };
        }

        [[nodiscard]] constexpr bool starts_with(const AsciiString& other)
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

        [[nodiscard]] constexpr bool ends_with(const AsciiString& other)
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

        [[nodiscard]] constexpr Optional<size_t> contains(const AsciiString& other)
        {
            if (!length() || !other.length() || length() < other.length())
                return {};

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
                    return { i };
            }
            return {};
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

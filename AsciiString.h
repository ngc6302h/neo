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
#include "Types.h"
#include <cstring>

namespace neo
{
    class String;

    class AsciiString
    {
        friend String;

    public:
        using BidIt = BidirectionalIterator<char*>;

        explicit constexpr AsciiString() = default;

        AsciiString(const char* cstring)
        {
            size_t size = strlen(cstring);
            VERIFY(size != 0);

            m_buffer = new char[size + 1];
            m_buffer[size] = 0;
            m_length = size;
            memcpy(m_buffer, cstring, size);
        }

        constexpr AsciiString(const char* cstring, size_t length)
        {
            size_t size = length;
            VERIFY(size != 0);

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
            if (this_is_constexpr())
            {
                for (size_t i = 0; i < other.m_length; i++)
                    m_buffer[i] = other.m_buffer[i];
            }
            else
            {
                memcpy(m_buffer, other.m_buffer, other.m_length);
            }
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
            for (size_t i = 0; i < m_length; i++)
            {
                if (m_buffer[i] != other.m_buffer[i])
                    return false;
            }
            return true;
        }

        [[nodiscard]] constexpr bool operator!=(const AsciiString& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr int operator<=>(const AsciiString& other) const
        {
            size_t i;
            for (i = 0; m_buffer[i] == other.m_buffer[i]; i++)
            {
                if (m_buffer[i] == 0)
                    return 0;
            }

            return m_buffer[i] < other.m_buffer[i] ? -1 : 1;
        }

        constexpr AsciiString(const AsciiString& other) :
            m_length(other.m_length)
        {
            m_buffer = new char[other.m_length + 1];
            m_buffer[other.m_length] = 0;
            if (this_is_constexpr())
            {
                for (size_t i = 0; i < other.m_length; i++)
                    m_buffer[i] = other.m_buffer[i];
            }
            else
            {
                memcpy(m_buffer, other.m_buffer, other.m_length);
            }
        }

        constexpr AsciiString(AsciiString&& other)
        {
            m_buffer = other.m_buffer;
            other.m_buffer = nullptr;
            other.m_length = 0;
        }

        [[nodiscard]] constexpr const BidIt cbegin() const
        {
            return BidIt(m_buffer);
        }

        [[nodiscard]] BidIt begin() const
        {
            return BidIt(m_buffer);
        }

        [[nodiscard]] BidIt end() const
        {
            return BidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr const BidIt cend() const
        {
            return BidIt(m_buffer + m_length);
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_length;
        }

        [[nodiscard]] constexpr char operator[](size_t index) const
        {
            VERIFY(index < size());
            return m_buffer[index];
        }

        [[nodiscard]] constexpr bool starts_with(const AsciiString& other)
        {
            if (!size() || !other.size() || other.size() > size())
                return false;

            for (size_t i = 0; i < other.size(); i++)
            {
                if ((*this)[i] != other[i])
                    return false;
            }
            return true;
        }

        [[nodiscard]] constexpr bool ends_with(const AsciiString& other)
        {
            if (!size() || !other.size() || other.size() > size())
                return false;

            for (size_t i = 0; i < other.size(); i++)
            {
                if ((*this)[size() - other.size() + i] != other[i])
                    return false;
            }
            return true;
        }

        [[nodiscard]] constexpr Optional<size_t> contains(const AsciiString& other)
        {
            if (!size() || !other.size() || size() < other.size())
                return {};

            for (size_t i = 0; i < size() - other.size(); i++)
            {
                bool found = true;
                for (size_t j = 0; j < other.size(); j++)
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

    private:
        char* m_buffer { nullptr };
        size_t m_length { 0 };
    };

    [[nodiscard]] constexpr AsciiString operator""_as(const char* cstring, size_t length)
    {
        return AsciiString(cstring, length);
    }
}
using neo::AsciiString;
using neo::operator""_as;

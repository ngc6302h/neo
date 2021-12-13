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

/*
    Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    class StringView;

    template<typename T, typename TIterator>
    struct IString
    {
        [[nodiscard]] constexpr Span<const char> span() const;
        [[nodiscard]] constexpr explicit operator char const*() const;
        [[nodiscard]] constexpr bool operator==(StringView const& other) const;
        [[nodiscard]] constexpr int operator<=>(StringView const& other) const;
        [[nodiscard]] constexpr TIterator begin() const;
        [[nodiscard]] constexpr TIterator end() const;
        [[nodiscard]] constexpr size_t length() const;
        [[nodiscard]] constexpr bool is_empty() const;
        template<IteratorLike TIterator_>
        [[nodiscard]] constexpr T substring(TIterator_ start) const;
        [[nodiscard]] constexpr T substring(size_t index_codepoint_start) const;
        template<IteratorLike TIterator_>
        [[nodiscard]] static constexpr T substring(TIterator_ const& start, TIterator_ const& end);
        template<IteratorLike TIterator_>
        [[nodiscard]] constexpr T substring(TIterator_ start, size_t codepoint_length) const;
        [[nodiscard]] constexpr T substring(size_t codepoint_start, size_t codepoint_length) const;
        [[nodiscard]] Vector<T> split(Utf32Char by) const;
        [[nodiscard]] Vector<T> split(StringView const& by) const;
        [[nodiscard]] constexpr bool starts_with(StringView const& other) const;
        [[nodiscard]] constexpr bool ends_with(StringView const& other) const;
        [[nodiscard]] constexpr TIterator find(StringView const& other) const;
        [[nodiscard]] constexpr bool contains(Utf32Char c) const;
        [[nodiscard]] constexpr bool contains(StringView const& other) const;
        [[nodiscard]] constexpr T trim_whitespace(TrimMode from_where) const;
        [[nodiscard]] constexpr Utf32Char operator[](size_t index) const;
        template<typename I>
        [[nodiscard]] constexpr Optional<I> to();
        template<typename I>
        [[nodiscard]] constexpr Optional<I> checked_to();
    };
}

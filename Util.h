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
#include "Types.h"
#define this_is_constexpr() __builtin_is_constant_evaluated()

template<typename T>
constexpr T max(const T& a, const T& b)
{
    return a > b ? a : b;
}

template<typename T, typename... Rest>
constexpr T max(T x, T y, Rest... values)
{
    return max(max(x, y), values...);
}

template<typename T>
constexpr T min(const T& a, const T& b)
{
    return a < b ? a : b;
}

template<typename T, typename... Rest>
constexpr T min(T x, T y, Rest... values)
{
    return min(min(x, y), values...);
}

template<typename T>
constexpr T clamp(T minval, T maxval, T value)
{
    return value > maxval ? maxval : value < minval ? minval :
                                                      value;
}

template<typename TFirst, typename TSecond, typename... TRest>
constexpr bool are_equal(TFirst first, TSecond second, TRest const&... rest)
{
    if constexpr (sizeof...(TRest) > 0)
        return first == second && are_equal(second, rest...);
    else
        return first == second;
}

constexpr void* neo_memmem(void* string, size_t string_length, void* substring, size_t substring_length)
{
    const char* haystack = static_cast<const char*>(string);
    const char* needle = static_cast<const char*>(substring);

    for (size_t i = 0; i <= string_length - substring_length; i++)
    {
        if (__builtin_memcmp(haystack + i, needle, substring_length) == 0)
            return (void*)(haystack + i);
    }
    return nullptr;
}

// true if the architecture is little endian
static constexpr bool LittleEndian = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
// true if the architecture is big endian
static constexpr bool BigEndian = __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;

static constexpr size_t KiB = 1024;
static constexpr size_t MiB = KiB * 1024;
static constexpr size_t GiB = MiB * 1024;
static constexpr size_t TiB = GiB * 1024;
static constexpr size_t PiB = TiB * 1024;
static constexpr size_t EiB = PiB * 1024;

static constexpr size_t KB = 1000;
static constexpr size_t MB = KB * 1000;
static constexpr size_t GB = MB * 1000;
static constexpr size_t TB = GB * 1000;
static constexpr size_t PB = TB * 1000;
static constexpr size_t EB = PB * 1000;

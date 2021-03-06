/*
    Copyright (C) 2022  César Torres (shortanemoia@protonmail.com)
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
#include "Types.h"
#include "Assert.h"
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

// Returns the index of target, or -1 if not found. This means this function works with array of size at most max(size_t)-1
template<typename T>
constexpr size_t bsearch(T const* array, size_t size, T const& target)
{
    size_t low = 0;
    size_t high = size - 1;
    while (low <= high)
    {
        size_t middle = (low + high) / 2;
        if (array[middle] < target)
        {
            low = middle + 1;
        }
        else if (array[middle] > target)
        {
            high = middle - 1;
        }
        else
            return middle;
    }
    return -1;
}

// Returns the index of the first element that is greater or equal than target.
// If all elements are less than target, this means it returns an index equal to the size of the array.
template<typename T>
constexpr size_t lower_bound(T const* array, size_t size, T const& target)
{
    size_t low = 0;
    size_t high = size - 1;
    while (low <= high)
    {
        size_t middle = (low + high) / 2;
        if (array[middle] < target)
        {
            low = middle + 1;
        }
        else if (array[middle] > target)
        {
            high = middle - 1;
        }
    }
    return low + 1;
}

constexpr u32 get_next_power_of_2(u32 x)
{
    VERIFY(x != 0);
    return 1 << (32 - __builtin_clz(x - 1));
}

constexpr u64 get_next_power_of_2(u64 x)
{
    VERIFY(x != 0);
    return 1 << (64 - __builtin_clzl(x - 1));
}

constexpr u32 log2(u32 x)
{
    return 32 - __builtin_clz(x);
}

constexpr u32 log2(u64 x)
{
    return 64 - __builtin_clzl(x);
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

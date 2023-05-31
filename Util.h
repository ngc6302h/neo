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
#include "Types.h"
#include "Assert.h"
constexpr bool compile_time()
{
    if
        consteval
        {
            return true;
        }
    else
    {
        return false;
    }
}

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

// Returns the index of the first element that is greater or equal than target.
// If all elements are less than target, this means it returns an index equal to the size of the array.
template<typename T>
constexpr size_t lower_bound(T const* array, size_t size, T const& target)
{
    VERIFY(size != 0);
    size_t low = 0;
    size_t high = size - 1;
    while (low < high)
    {
        size_t mid = (low + high) / 2 + 1;
        if (array[mid] < target)
        {
            low = mid + 1;
        }
        else if (array[mid] > target)
        {
            high = mid - 1;
        }
    }
    return low + 1;
}

// Returns the index of the first element that is greater or equal than target.
// If all elements are less than target, this means it returns an index equal to the size of the array.
template<typename T, typename TComparer>
constexpr size_t lower_bound(T const* array, size_t size, T const& target, TComparer comparer)
{
    VERIFY(size != 0);
    size_t low = 0;
    size_t high = size - 1;
    while (low < high)
    {
        size_t mid = (low + high) / 2 + 1;
        auto comparison = comparer(array[mid], target);
        if (comparison)
        {
            low = mid + 1;
        }
        else if (array[mid] != target)
        {
            high = mid - 1;
        }
    }
    return low + 1;
}

// Returns the index of target, or -1 if not found. This means this function works with array of size at most max(size_t)-1
template<typename T>
constexpr size_t bsearch(T const* array, size_t size, T const& target)
{
    return lower_bound(array, size, target) - 1;
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

template<typename T, typename TFirst, typename... TRest>
constexpr bool is_any_of(T const& value, TFirst const& first, TRest const&... values)
{
    if (value == static_cast<T>(first))
        return true;
    else
    {
        if constexpr (sizeof...(TRest) > 0)
            return is_any_of(value, values...);
        else
            return false;
    }
}

// true if the architecture is little endian
static constexpr bool LittleEndian = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
// true if the architecture is big endian
static constexpr bool BigEndian = __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;

static constexpr auto bswap(auto value)
{
    if constexpr (sizeof(value) == 1)
        return value;
    if constexpr (sizeof(value) == 2)
        return __builtin_bswap16(value);
    if constexpr (sizeof(value) == 4)
        return __builtin_bswap32(value);
    if constexpr (sizeof(value) == 8)
        return __builtin_bswap64(value);
    if constexpr (sizeof(value) == 16)
        return __builtin_bswap128(value);
}

static constexpr auto HostToBigEndian(auto value)
{
    if constexpr (BigEndian)
        return value;

    return bswap(value);
}

static constexpr auto HostToLittleEndian(auto value)
{
    if constexpr (LittleEndian)
        return value;

    return bswap(value);
}

static constexpr auto BigToHostEndian(auto value)
{
    if constexpr (BigEndian)
        return value;
    return bswap(value);
}

static constexpr auto LittleToHostEndian(auto value)
{
    if constexpr (LittleEndian)
        return value;
    return bswap(value);
}

static constexpr auto LittleToBigEndian(auto value)
{
    return bswap(value);
}

static constexpr auto BigToLittleEndian(auto value)
{
    return bswap(value);
}

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

#ifdef __x86_64__
static constexpr size_t L1DataChacheLineSize = 64;
#endif

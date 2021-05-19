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
    return value > maxval ? maxval : value < minval ? minval
                                                    : value;
}

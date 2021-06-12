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

#include "Concepts.h"
#include "TypeTraits.h"

namespace neo
{
    template<Iterable TContainer, EqualityComparable T>
    constexpr auto find(const TContainer& where, const T& what)
    {
        auto begin = where.begin();
        auto end = where.end();
        do
        {
            if (*begin == what)
                return begin;
        } while (begin++ != end);
    }

    template<Iterable TContainer, EqualityComparable T>
    constexpr bool contains(const TContainer& where, const T& what)
    {
        for (const auto& x : where)
        {
            if (what == x)
            {
                return true;
            }
        }
        return false;
    }

    template<Iterable TContainer, typename TSortingFunc>
    constexpr void sort(const TContainer& what, TSortingFunc predicate) requires Callable<TSortingFunc, decltype(declval<TContainer>()[0]), decltype(declval<TContainer>()[0])>
    {
        for (auto& x : what)
        {
            for (auto& y : what)
            {
                if (predicate(x, y))
                {
                    swap(x, y);
                }
            }
        }
    }
}

using neo::contains;
using neo::find;
using neo::sort;

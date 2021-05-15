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

#include "TypeTraits.h"

namespace neo
{
    template<typename T, typename TConstIter>
    constexpr Iter find(TConstIter begin, TConstIter end, const T& element)
    {
        do
        {
            if (*begin == element)
                return begin;
        }
        while (begin++ != end);
    }
    
    template<typename TConstIter, typename T>
    constexpr bool contains(TConstIter begin, TConstIter end, const T& element)
    {
        do
        {
            if (*begin == element)
                return true;
        }
        while (begin++ != end);
    }
    
    template<typename TIter, typename TSortingFunc>
    constexpr bool sort(TIter begin, TIter end, TSortingFunc predicate) //TODO: Conceptify this
    {
        for (TIter x = begin; x != end; x++)
        {
            for (TIter y = begin; y != end; y++)
            {
                if (predicate(*x, *y))
                {
                    swap(*x, *y);
                }
            }
        }
    }
}

using neo::find;
using neo::contains;
using neo::sort;
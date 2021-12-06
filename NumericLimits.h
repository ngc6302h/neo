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
#include "Concepts.h"

namespace neo
{
    template<Arithmetic T>
    struct NumericLimits;
    
    template<Integral T>
    struct NumericLimits<T>
    {
        static constexpr T max()
        {
            if constexpr(Unsigned<T>)
                return ~static_cast<T>(0);
            else
                return (static_cast<size_t>(1) << (sizeof(T)*8-1))-1;
        }
        
        static constexpr T min()
        {
            if constexpr(Signed<T>)
                return static_cast<size_t>(1) << (sizeof(T)*8-1);
            else
                return static_cast<T>(0);
        }
    };
    
    template<>
    struct NumericLimits<float>
    {
        static constexpr float max()
        {
            return __FLT_MAX__;
        }
        
        static constexpr float min()
        {
            return __FLT_MIN__;
        }
        
        static constexpr float epsilon()
        {
            return __FLT_EPSILON__;
        }
    };
    
    template<>
    struct NumericLimits<double>
    {
        static constexpr float max()
        {
            return __DBL_MAX__;
        }
        
        static constexpr float min()
        {
            return __DBL_MIN__;
        }
        
        static constexpr float epsilon()
        {
            return __DBL_EPSILON__;
        }
    };
}
using neo::NumericLimits;

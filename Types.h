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
#include "Util.h"

using i8 = __INT8_TYPE__;
using u8 = __UINT8_TYPE__;
using i16 = __INT16_TYPE__;
using u16 = __UINT16_TYPE__;
using i32 = __INT32_TYPE__;
using u32 = __UINT32_TYPE__;
using i64 = __INT64_TYPE__;
using u64 = __UINT64_TYPE__;

using int8_t = i8;
using uint8_t = u8;
using int16_t = i16;
using uint16_t = u16;
using int32_t = i32;
using uint32_t = u32;
using int64_t = i64;
using uint64_t = u64;

namespace neo
{
    
    template<typename T>
    struct make_signed
    {
        using type = Conditional<sizeof(T) == 1, int8_t,
                Conditional<sizeof(T) == 2, int16_t,
                        Conditional<sizeof(T) == 4, int32_t, int64_t>>>;
    };
    
    template<typename T>
    using make_signed_t = typename make_signed<T>::type;
    
    template<typename T>
    struct ReferenceWrapper
    {
        constexpr ReferenceWrapper(T& obj) : ref(obj)
        {}
        
        constexpr operator T&()
        {
            return ref;
        }
        
        T& ref;
    };
    
    template<typename T>
    constexpr ReferenceWrapper<T> ref(T& obj) { return ReferenceWrapper(obj); }
}
using size_t = __SIZE_TYPE__;
using ssize_t = neo::make_signed_t<size_t>;

using neo::make_signed;
using neo::make_signed_t;
using neo::ReferenceWrapper;

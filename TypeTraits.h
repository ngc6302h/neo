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

namespace neo::detail
{
    template<typename T>
    struct __RemoveReference
    {
        using type = T;
    };
    
    template<typename T>
    struct __RemoveReference<T&>
    {
        using type = T;
    };
    
    template<typename T>
    struct __RemoveReference<T&&>
    {
        using type = T;
    };
}

namespace std
{
    template<typename T>
    constexpr T&& move(T& value)
    {
        return static_cast<T &&>(value);
    }
}
using std::move;

namespace neo
{
    
    template<bool TBool, typename TrueType, typename FalseType>
    struct conditional_t
    {
        using type = TrueType;
    };
    template<typename TrueType, typename FalseType>
    struct conditional_t<false, TrueType, FalseType>
    {
        using type = FalseType;
    };
    template<bool TBool, typename TrueType, typename FalseType>
    using Conditional = typename conditional_t<TBool, TrueType, FalseType>::type;
    
    struct TrueType;
    struct FalseType;
    
    template<typename T>
    struct RemovePointer
    {
        using type = T;
    };
    
    template<typename T>
    struct RemovePointer<T*>
    {
        using type = T;
    };
    
    template<typename T>
    using RemovePointer_t = typename RemovePointer<T>::type;
    
    
    template<typename T>
    using RemoveReference = typename detail::__RemoveReference<T>::type;
    
    template<class T>
    constexpr bool IsLvalueReference = false;
    template<class T>
    constexpr bool IsLvalueReference<T&> = true;
    
    template<class T>
    constexpr bool IsRvalueReference = false;
    template<class T>
    constexpr bool IsRvalueReference<T&&> = true;
    
    template<typename T>
    constexpr bool IsPointer = false;
    template<typename T>
    constexpr bool IsPointer<T*> = true;
    
    template<typename T>
    constexpr T&& forward(RemoveReference<T>& arg)
    {
        return static_cast<T &&>(arg);
    }
    
    template<typename T>
    constexpr T&& forward(RemoveReference<T>&& arg)
    {
        static_assert(!IsLvalueReference<T>, "can't forward a rvalue as an lvalue!");
        return static_cast<T&&>(arg);
    }
    
    template<int N>
    struct IntegralConstant
    {
        static constexpr auto value = N;
        constexpr operator int()
        {
            return N;
        }
    };
    
    template<bool TBool, typename T>
    struct enable_if {};
    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };
    template<bool TBool, typename T = void>
    using EnableIf = typename enable_if<TBool, T>::type;
    
    template<typename T>
    constexpr T&& declval();
    
    template<typename T, typename U>
    struct is_same { static constexpr bool value = false; };
    template<typename T>
    struct is_same<T, T>{ static constexpr bool value = true; };
    template<typename T, typename U>
    using is_same_v = typename is_same<T, U>::value;
    
    template<typename T, typename... Pack>
    constexpr bool PackContains = (is_same<T, Pack>::value || ...);
    
    template<typename T, typename... Pack>
    struct IndexOfType_t { static constexpr auto value = 0;};
    template<typename T, typename... Pack>
    struct IndexOfType_t<T, T, Pack...> { static constexpr auto value = 0; };
    template<typename T, typename U, typename... Pack>
    struct IndexOfType_t<T, U, Pack...> { static constexpr auto value = 1 + IndexOfType_t<T, Pack...>::value; };
    template<typename T, typename... Pack>
    static constexpr auto IndexOfType = IndexOfType_t<T, Pack...>::value;
    
    template<typename T, typename... Ts>
    struct FirstType_t { using type = typename FirstType_t<Ts...>::type; };
    template<typename T>
    struct FirstType_t<T> { using type =  T; };
    
    template<int Index, typename T, typename... Types>
    struct TypeOfIndex_t { using type = typename TypeOfIndex_t<Index-1, Types...>::type ; };
    template<typename T, typename... Types>
    struct TypeOfIndex_t<0, T, Types...> { using type = T; };
    template<int Index, typename... Types>
    using TypeOfIndex = typename TypeOfIndex_t<Index, Types...>::type;
    
    template<typename T>
    constexpr void swap(T& a, T&b)
    {
        T& temp = move(a);
        a = move(b);
        b = move(temp);
    }
}

using neo::conditional_t;
using neo::Conditional;
using neo::enable_if;
using neo::EnableIf;
using neo::IndexOfType_t;
using neo::IndexOfType;
using neo::TypeOfIndex_t;
using neo::TypeOfIndex;
using neo::PackContains;
using neo::IsLvalueReference;
using neo::IsRvalueReference;
using neo::IsPointer;
using neo::RemoveReference;

using neo::forward;
using neo::declval;
using neo::swap;


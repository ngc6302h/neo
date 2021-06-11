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
#include "Types.h"

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

//required for the compiler
namespace std
{
    template<typename T>
    constexpr T&& move(T& value)
    {
        return static_cast<T&&>(value);
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
    using remove_pointer_t = typename RemovePointer<T>::type;

    template<typename T>
    using RemoveReference = typename detail::__RemoveReference<T>::type;

    template<typename T>
    struct make_signed
    {
        using type = Conditional<sizeof(T) == 1, int8_t,
            Conditional<sizeof(T) == 2, int16_t,
                Conditional<sizeof(T) == 4, int32_t, int64_t>>>;
    };

    template<typename T>
    using MakeSigned = typename make_signed<T>::type;

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
    constexpr bool IsArray = false;
    template<typename T>
    constexpr bool IsArray<T[]> = true;
    template<typename T, size_t N>
    constexpr bool IsArray<T[N]> = true;

    template<typename T>
    constexpr T&& forward(RemoveReference<T>& arg)
    {
        return static_cast<T&&>(arg);
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
    struct enable_if
    {
    };
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
    struct is_same
    {
        static constexpr bool value = false;
    };
    template<typename T>
    struct is_same<T, T>
    {
        static constexpr bool value = true;
    };
    template<typename T, typename U>
    constexpr bool IsSame = is_same<T, U>::value;

    template<typename T, typename... Pack>
    constexpr bool PackContains = (is_same<T, Pack>::value || ...);

    template<typename T, typename... Pack>
    struct index_of_type
    {
        static constexpr auto value = 0;
    };
    template<typename T, typename... Pack>
    struct index_of_type<T, T, Pack...>
    {
        static constexpr auto value = 0;
    };
    template<typename T, typename U, typename... Pack>
    struct index_of_type<T, U, Pack...>
    {
        static constexpr auto value = 1 + index_of_type<T, Pack...>::value;
    };
    template<typename T, typename... Pack>
    static constexpr auto IndexOfType = index_of_type<T, Pack...>::value;

    template<typename T, typename... Ts>
    struct first_type_t
    {
        using type = typename first_type_t<Ts...>::type;
    };
    template<typename T>
    struct first_type_t<T>
    {
        using type = T;
    };
    template<typename... Ts>
    using FirstType = typename first_type_t<Ts...>::type;

    template<int Index, typename T, typename... Types>
    struct type_of_index
    {
        using type = typename type_of_index<Index - 1, Types...>::type;
    };
    template<typename T, typename... Types>
    struct type_of_index<0, T, Types...>
    {
        using type = T;
    };
    template<int Index, typename... Types>
    using TypeOfIndex = typename type_of_index<Index, Types...>::type;

    template<typename T>
    constexpr void swap(T& a, T& b)
    {
        T& temp = move(a);
        a = move(b);
        b = move(temp);
    }

    template<typename T>
    constexpr bool IsTrivial = __is_trivial(T) && IsSame<T, T>;
    template<typename T, typename U = T>
    constexpr bool IsTriviallyAssignable = __is_trivially_assignable(T, U) && IsSame<T, T>;
    template<typename T>
    constexpr bool IsTriviallyConstructible = __is_trivially_constructible(T) && IsSame<T, T>;
    template<typename T>
    constexpr bool IsTriviallyCopyable = __is_trivially_copyable(T) && IsSame<T, T>;
    template<typename T>
    constexpr bool IsTriviallyDestructible = __has_trivial_destructor(T);

    template<typename T>
    constexpr bool IsBoolean = IsSame<T, bool>;

    template<typename T>
    constexpr bool IsNullptr = IsSame<T, decltype(nullptr)>;

    template<typename T>
    constexpr bool IsCharacter = IsSame<T, char> || IsSame<T, unsigned char> || IsSame<T, wchar_t> || IsSame<T, char8_t> || IsSame<T, char16_t> || IsSame<T, char32_t>;

#if __has_builtin(__is_integral)
    template<typename T>
    constexpr bool IsIntegral
        = __is_integral(T);
#else
    template<typename T>
    constexpr bool IsIntegral = IsSame<T, unsigned char> || IsSame<T, unsigned short> || IsSame<T, unsigned int> || IsSame<T, unsigned long> || IsSame<T, long long> || IsSame<T, char> || IsSame<T, short> || IsSame<T, int> || IsSame<T, long> || IsSame<T, long long> || IsCharacter<T>;
#endif
#if __has_builtin(__is_floating_point)
    template<typename T>
    constexpr bool IsFloatingPoint = __is_floating_point(T);
#else
    template<typename T>
    constexpr bool IsFloatingPoint = IsSame<T, float> || IsSame<T, double>;
#endif

#if __has_builtin(__is_fundamental)
    template<typename T>
    constexpr bool IsFundamental = __is_fundamental(T);
#else
    template<typename T>
    constexpr bool IsFundamental = IsIntegral<T> || IsFloatingPoint<T> || IsNullptr<T>;
#endif

    namespace detail
    {
        template<typename T>
        struct decay_array
        {
            using type = T*;
        };
        
        template<typename T>
        struct decay_array<T[]>
        {
            using type = T*;
        };
    
        template<typename T>
        struct decay_array<const T[]>
        {
            using type = const T*;
        };
        
        template<typename T, size_t N>
        struct decay_array<T[N]>
        {
            using type = T*;
        };
    
        template<typename T, size_t N>
        struct decay_array<const T[N]>
        {
            using type = const T*;
        };
    }
    template<typename T>
    using DecayArray = typename detail::decay_array<T>::type;
    
    namespace detail
    {
        template<typename T>
        constexpr bool IsInequalityComparable() { return false; }
        template<InequalityComparable T>
        constexpr bool IsInequalityComparable() { return true; }
    }
    template<typename T>
    constexpr bool IsInequalityComparable = detail::IsInequalityComparable<T>();
}
using ssize_t = neo::MakeSigned<size_t>;

using neo::Conditional;
using neo::EnableIf;
using neo::FirstType;
using neo::IndexOfType;
using neo::IsBoolean;
using neo::IsCharacter;
using neo::IsFloatingPoint;
using neo::IsFundamental;
using neo::IsIntegral;
using neo::IsLvalueReference;
using neo::IsNullptr;
using neo::IsPointer;
using neo::IsRvalueReference;
using neo::IsTrivial;
using neo::IsTriviallyAssignable;
using neo::IsTriviallyConstructible;
using neo::IsTriviallyCopyable;
using neo::IsTriviallyDestructible;
using neo::MakeSigned;
using neo::PackContains;
using neo::RemovePointer;
using neo::RemoveReference;
using neo::TypeOfIndex;

using neo::declval;
using neo::forward;
using neo::swap;

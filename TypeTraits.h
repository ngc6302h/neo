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

#include "Preprocessor.h"
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

#ifndef NEO_DO_NOT_DEFINE_STD
// move must live in std, required for the compiler

namespace std
{
    template<typename T>
    constexpr T&& move(T& value)
    {
        return static_cast<T&&>(value);
    }

    template<class T, T v>
    struct integral_constant
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;
        constexpr operator value_type() const noexcept
        {
            return value;
        }

        constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };

    template<typename T>
    struct tuple_size : std::integral_constant<size_t, T::size()>
    {
    };
}
#endif
#ifndef NEO_NO_MOVE_IN_GLOBAL_NAMESPACE
using std::move;
#endif

namespace neo
{
    namespace detail
    {
        template<typename T>
        struct identity_t
        {
            using type = T;
        };
    }
    template<typename T>
    using IdentityType = typename detail::identity_t<T>::type;

    namespace detail
    {
        template<bool TBool, typename TTrueType, typename TFalseType>
        struct conditional_t
        {
            using type = TTrueType;
        };
        template<typename TTrueType, typename TFalseType>
        struct conditional_t<false, TTrueType, TFalseType>
        {
            using type = TFalseType;
        };
    }

    template<bool TBool, typename TTrueType, typename TFalseType>
    using Conditional = typename detail::conditional_t<TBool, TTrueType, TFalseType>::type;

    namespace detail
    {
        template<bool VBool, auto vtrue, auto vfalse>
        struct value_conditional_t
        {
        };

        template<auto vtrue, auto vfalse>
        struct value_conditional_t<true, vtrue, vfalse>
        {
            static constexpr decltype(auto) value = vtrue;
        };

        template<auto vtrue, auto vfalse>
        struct value_conditional_t<false, vtrue, vfalse>
        {
            static constexpr decltype(auto) value = vfalse;
        };
    }

    template<bool VBool, auto VTrueType, auto VFalseType>
    static constexpr auto ValueConditional = detail::value_conditional_t<VBool, VTrueType, VFalseType>::value;

    template<typename T>
    struct remove_pointer_t
    {
        using type = T;
    };

    template<typename T>
    struct remove_pointer_t<T*>
    {
        using type = T;
    };

    template<typename T>
    using RemovePointer = typename remove_pointer_t<T>::type;

    template<typename T>
    using RemoveReference = typename detail::__RemoveReference<T>::type;

    namespace detail
    {
        template<typename T>
        struct remove_const_t
        {
            using type = T;
        };

        template<typename T>
        struct remove_const_t<const T>
        {
            using type = T;
        };

        template<typename T>
        struct remove_volatile_t
        {
            using type = T;
        };

        template<typename T>
        struct remove_volatile_t<volatile T>
        {
            using type = T;
        };

    }

    template<typename T>
    using RemoveConst = typename detail::remove_const_t<T>::type;

    template<typename T>
    using RemoveVolatile = typename detail::remove_volatile_t<T>::type;

    template<typename T>
    using RemoveCV = typename detail::remove_volatile_t<typename detail::remove_const_t<T>::type>::type;

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
        static_assert(!IsLvalueReference<T>, "Can't forward a rvalue as an lvalue!");
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

    template<auto t, auto u>
    constexpr bool IsSameValue = t == u;

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

    template<typename T>
    struct type_of_index<0, T>
    {
        using type = T;
    };

    template<int Index, typename... Types>
    using TypeOfIndex = typename type_of_index<Index, Types...>::type;

    template<typename T, typename U, typename... Ts>
    constexpr size_t TypeContainsN = IsSame<T, U> + TypeContainsN<T, Ts...>;
    template<typename T, typename U>
    constexpr size_t TypeContainsN<T, U> = IsSame<T, U> ? 1 : 0;
    template<typename T, typename U, typename... Ts>
    constexpr bool TypeContains = TypeContainsN<T, U, Ts...> > 0;
}

namespace neo
{
    namespace detail
    {
        template<typename T, typename... Ts>
        constexpr size_t UniqueType = TypeContainsN<T, Ts...>;
        template<typename T>
        constexpr size_t UniqueType<T> = 0;
    }

    template<typename T, typename... Ts>
    constexpr bool UniqueType = detail::UniqueType<T, Ts...> == 1;

    template<typename T, typename... Ts>
    constexpr size_t PackSize = 1 + PackSize<Ts...>;
    template<typename T>
    constexpr size_t PackSize<T> = 1;

    template<typename T>
    constexpr void swap(T& a, T& b)
    {
        T temp = move(a);
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
    constexpr bool IsCharacter = IsSame<T, char> || IsSame<T, signed char> || IsSame<T, unsigned char> || IsSame<T, wchar_t> || IsSame<T, char8_t> || IsSame<T, char16_t> || IsSame<T, char32_t>;

#if __has_builtin(__is_integral)
    template<typename T>
    constexpr bool IsIntegral
        = __is_integral(T);
#else
    template<typename T>
    constexpr bool IsIntegral = IsSame<T, signed char> || IsSame<T, unsigned char> || IsSame<T, unsigned short> || IsSame<T, unsigned int> || IsSame<T, unsigned long> || IsSame<T, long long> || IsSame<T, char> || IsSame<T, short> || IsSame<T, int> || IsSame<T, long> || IsSame<T, long long> || IsCharacter<T>;
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

    template<typename T>
    constexpr bool IsSigned = static_cast<T>(-1) < static_cast<T>(0);

    template<typename T>
    constexpr bool IsUnsigned = !IsSigned<T>;

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

    template<typename T>
    struct remove_reference_wrapper_t
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference_wrapper_t<ReferenceWrapper<T>>
    {
        using type = T;
    };

    template<typename T>
    using RemoveReferenceWrapper = typename remove_reference_wrapper_t<T>::type;

    template<typename T>
    using RewrapReference = ReferenceWrapper<RemoveReferenceWrapper<T>>;

    template<typename F, typename... Args>
    using ReturnType = decltype(declval<F>()(forward<Args>(declval<Args>())...));

    template<typename T>
    using Naked = RemoveCV<RemoveReferenceWrapper<RemoveCV<RemoveReference<T>>>>;

    template<typename T>
    static constexpr void TypedCopy(size_t num, T const* from, T* to)
    {
        for (size_t i = 0; i < num; ++i)
            to[i] = from[i];
    }

    template<typename T>
    static constexpr void UntypedCopy(size_t num, T const* from, T* to)
    {
        __builtin_memcpy(to, from, num * sizeof(T));
    }

    template<typename T>
    static constexpr void OverlappingUntypedCopy(size_t num, T const* from, T* to)
    {
        __builtin_memmove(to, from, num * sizeof(T));
    }

    template<typename T>
    static constexpr void Copy(size_t num, T const* from, T* to)
    {
        if constexpr (IsTriviallyCopyable<T>)
            UntypedCopy(num, from, to);
        else
            TypedCopy(num, from, to);
    }

    template<typename T>
    static constexpr void TypedMove(size_t num, T const* from, T* to)
    {
        for (size_t i = 0; i < num; ++i)
            to[i] = move(from[i]);
    }

    template<typename T>
    static constexpr void MoveOrCopy(size_t num, T const* from, T* to)
    {
        if constexpr (IsTriviallyCopyable<T>)
            Copy(num, from, to);
        else
            TypedMove(num, from, to);
    }

    template<typename TFrom, typename TTo>
    requires(IsTriviallyConstructible<TTo>&& IsTriviallyCopyable<TFrom> && sizeof(TFrom) == sizeof(TTo)) constexpr TTo bit_cast(TFrom const& value)
    {
#if __has_builtin(__builtin_bit_cast)
        return __builtin_bit_cast(TTo, value);
#else
        TTo dest;
        __builtin_memcpy(&dest, &value, sizeof(TFrom));
        return dest;
#endif
    }

    template<typename TTo, typename TFrom>
    requires(!IsLvalueReference<TFrom> && !IsRvalueReference<TFrom>) constexpr TTo virtual_cast(TFrom& from)
    {
#ifdef DEBUG_ASSERTS
        return dynamic_cast<TTo>(from);
#else
        return static_cast<TTo>(from);
#endif
    }

}

// required for tuple structured binding support

namespace std
{
    template<size_t I, typename TTuple>
    struct tuple_element
    {
        using raw_type = typename TTuple::template TypeOfElementAtIndex<I>;
        using type = neo::Conditional<neo::IsSame<raw_type, neo::RemoveReferenceWrapper<raw_type>>, raw_type, neo::RemoveReferenceWrapper<raw_type>&&>;
    };
}

using ssize_t = neo::MakeSigned<size_t>;

using neo::Conditional;
using neo::Copy;
using neo::DecayArray;
using neo::EnableIf;
using neo::FirstType;
using neo::IdentityType;
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
using neo::IsSame;
using neo::IsSameValue;
using neo::IsSigned;
using neo::IsTrivial;
using neo::IsTriviallyAssignable;
using neo::IsTriviallyConstructible;
using neo::IsTriviallyCopyable;
using neo::IsTriviallyDestructible;
using neo::IsUnsigned;
using neo::MakeSigned;
using neo::Naked;
using neo::OverlappingUntypedCopy;
using neo::PackContains;
using neo::RemovePointer;
using neo::RemoveReference;
using neo::RemoveReferenceWrapper;
using neo::ReturnType;
using neo::RewrapReference;
using neo::TypeContains;
using neo::TypeContainsN;
using neo::TypedCopy;
using neo::TypedMove;
using neo::TypeOfIndex;
using neo::UntypedCopy;
using neo::ValueConditional;

using neo::bit_cast;
using neo::declval;
using neo::forward;
using neo::make_signed;
using neo::swap;
using neo::virtual_cast;

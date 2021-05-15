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
template <typename T>
struct __RemoveReference
{
    using type = T;
};

template <typename T>
struct __RemoveReference<T&>
{
    using type = T;
};

template <typename T>
struct __RemoveReference<T&&>
{
    using type = T;
};
}

namespace std
{
template <typename T>
constexpr T&& move(T& value)
{
    return static_cast<T&&>(value);
}
}
using std::move;

namespace neo
{

template <bool TBool, typename TrueType, typename FalseType>
struct conditional_t
{
    using type = TrueType;
};
template <typename TrueType, typename FalseType>
struct conditional_t<false, TrueType, FalseType>
{
    using type = FalseType;
};
template <bool TBool, typename TrueType, typename FalseType>
using Conditional = typename conditional_t<TBool, TrueType, FalseType>::type;

struct TrueType;
struct FalseType;

template <typename T>
struct RemovePointer
{
    using type = T;
};

template <typename T>
struct RemovePointer<T*>
{
    using type = T;
};

template <typename T>
using remove_pointer_t = typename RemovePointer<T>::type;

template <typename T>
using RemoveReference = typename detail::__RemoveReference<T>::type;

template <class T>
constexpr bool IsLvalueReference = false;
template <class T>
constexpr bool IsLvalueReference<T&> = true;

template <class T>
constexpr bool IsRvalueReference = false;
template <class T>
constexpr bool IsRvalueReference<T&&> = true;

template <typename T>
constexpr bool IsPointer = false;
template <typename T>
constexpr bool IsPointer<T*> = true;

template <typename T>
constexpr T&& forward(RemoveReference<T>& arg)
{
    return static_cast<T&&>(arg);
}

template <typename T>
constexpr T&& forward(RemoveReference<T>&& arg)
{
    static_assert(!IsLvalueReference<T>, "can't forward a rvalue as an lvalue!");
    return static_cast<T&&>(arg);
}

template <int N>
struct IntegralConstant
{
    static constexpr auto value = N;
    constexpr operator int()
    {
        return N;
    }
};

template <bool TBool, typename T>
struct enable_if
{
};
template <typename T>
struct enable_if<true, T>
{
    using type = T;
};
template <bool TBool, typename T = void>
using EnableIf = typename enable_if<TBool, T>::type;

template <typename T>
constexpr T&& declval();

template <typename T, typename U>
struct is_same
{
    static constexpr bool value = false;
};
template <typename T>
struct is_same<T, T>
{
    static constexpr bool value = true;
};
template <typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

template <typename T, typename... Pack>
constexpr bool PackContains = (is_same<T, Pack>::value || ...);

template <typename T, typename... Pack>
struct index_of_t
{
    static constexpr auto value = 0;
};
template <typename T, typename... Pack>
struct index_of_t<T, T, Pack...>
{
    static constexpr auto value = 0;
};
template <typename T, typename U, typename... Pack>
struct index_of_t<T, U, Pack...>
{
    static constexpr auto value = 1 + index_of_t<T, Pack...>::value;
};
template <typename T, typename... Pack>
static constexpr auto IndexOfType = index_of_t<T, Pack...>::value;

template <typename T, typename... Ts>
struct first_type_t
{
    using type = typename first_type_t<Ts...>::type;
};
template <typename T>
struct first_type_t<T>
{
    using type = T;
};
template <typename... Ts>
using FirstType = typename first_type_t<Ts...>::type;

template <int Index, typename T, typename... Types>
struct type_of_index_t
{
    using type = typename type_of_index_t<Index - 1, Types...>::type;
};
template <typename T, typename... Types>
struct type_of_index_t<0, T, Types...>
{
    using type = T;
};
template <int Index, typename... Types>
using TypeOfIndex = typename type_of_index_t<Index, Types...>::type;

template <typename T>
constexpr void swap(T& a, T& b)
{
    T& temp = move(a);
    a = move(b);
    b = move(temp);
}

template <typename T>
constexpr bool IsTrivial = __is_trivial(T) && is_same_v<T, T>;
template <typename T, typename U = T>
constexpr bool IsTriviallyAssignable = __is_trivially_assignable(T, U) && is_same_v<T, T>;
template <typename T>
constexpr bool IsTriviallyConstructible = __is_trivially_constructible(T) && is_same_v<T, T>;
template <typename T>
constexpr bool IsTriviallyCopyable = __is_trivially_copyable(T) && is_same_v<T, T>;
template <typename T>
constexpr bool IsTriviallyDestructible = __has_trivial_destructor(T);

template <typename T>
constexpr bool IsBoolean = is_same_v<T, bool>;

template <typename T>
constexpr bool IsNullptr = is_same_v<T, decltype(nullptr)>;

template <typename T>
constexpr bool IsCharacter = is_same_v<T, char> || is_same_v<T, unsigned char> || is_same_v<T, wchar_t> || is_same_v<T, char8_t> || is_same_v<T, char16_t> || is_same_v<T, char32_t>;

#if __has_builtin(__is_integral)
template <typename T>
constexpr bool IsIntegral
    = __is_integral(T);
#else
template <typename T>
constexpr bool IsIntegral = is_same_v<T, unsigned char> || is_same_v<T, unsigned short> || is_same_v<T, unsigned int> || is_same_v<T, unsigned long> || is_same_v<T, long long> || is_same_v<T, char> || is_same_v<T, short> || is_same_v<T, int> || is_same_v<T, long> || is_same_v<T, long long> || IsCharacter<T>;
#endif
#if __has_builtin(__is_floating_point)
template <typename T>
constexpr bool IsFloatingPoint = __is_floating_point(T);
#else
template <typename T>
constexpr bool IsFloatingPoint = is_same_v<T, float> || is_same_v<T, double>;
#endif

#if __has_builtin(__is_fundamental)
template <typename T>
constexpr bool IsFundamental = __is_fundamental(T);
#else
template <typename T>
constexpr bool IsFundamental = IsIntegral<T> || IsFloatingPoint<T> || IsNullptr<T>;
#endif
}

using neo::Conditional;
using neo::conditional_t;
using neo::enable_if;
using neo::EnableIf;
using neo::first_type_t;
using neo::FirstType;
using neo::index_of_t;
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
using neo::PackContains;
using neo::remove_pointer_t;
using neo::RemovePointer;
using neo::RemoveReference;
using neo::type_of_index_t;
using neo::TypeOfIndex;

using neo::declval;
using neo::forward;
using neo::swap;
